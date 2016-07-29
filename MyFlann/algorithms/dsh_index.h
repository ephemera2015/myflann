#pragma once
#ifndef _MYFLANN_DSH_INDEX_H_
#define _MYFLANN_DSH_INDEX_H_
#include<vector>
#include<algorithm>
#include<set>
#include<cmath>
#include "nn_index.h"
#include "../util/matrix.h"
#include"../util/kmeans.h"
#include"../util/dsh_table.h"
namespace myflann
{

	struct DshIndexParams : public IndexParams
	{
		DshIndexParams(unsigned int table_number = 20, unsigned int key_size = 40,float alpha=1.5,int kmeans_it_times=3,int r=3)
		{
			(*this)["algorithm"] = FLANN_DSH_TYPE;
			(*this)["table_number"] = table_number;
			(*this)["key_size"] = key_size;
			(*this)["alpha"] = alpha;
			(*this)["kmeans_it_times"] = kmeans_it_times;
			(*this)["r"] = r;
		}
	};

	template<typename Distance>
	class DshIndex : public NNIndex<Distance>
	{
	public:
		typedef typename Distance::ElementType ElementType;
		typedef typename Distance::ResultType DistanceType;
		typedef NNIndex<Distance> BaseClass;
	public:
		DshIndex(const Matrix<ElementType>& features, const IndexParams& params = DshIndexParams(), Distance d = Distance()) :
			BaseClass(features, params, d)
		{
			table_number_ = get_param<unsigned int>(index_params_, "table_number");
			key_size_ = get_param<unsigned int>(index_params_, "key_size");
			alpha_ = get_param<float>(index_params_, "alpha");
			kmeans_it_times_ = get_param<int>(index_params_, "kmeans_it_times");
			r_ = get_param<int>(index_params_, "r");
			int i;
			for (i = 0; i < table_number_;++i)
			{
				tables_.push_back(new  DshTable<ElementType>(features.cols, (ElementType*)features.ptr()));
			}
			int j;
			hashing_vectors_ = new float**[table_number_];
			for (i = 0; i < table_number_;++i)
			{
				hashing_vectors_[i] = new float*[key_size_];
				for (j = 0; j < key_size_;++j)
				{
					hashing_vectors_[i][j] = new float[feature_col_+1];
				}
			}
		}
		~DshIndex()
		{
			int i, j;
			for (i = 0; i < table_number_; ++i)
			{
				for (j = 0; j < key_size_; ++j)
				{
					delete[] hashing_vectors_[i][j];
				}
				delete[] hashing_vectors_[i];
				delete tables_[i];
			}
			delete[] hashing_vectors_;
	
		}
		void buildIndex()
		{
			int i;
			#pragma omp parallel for
			for (i = 0; i < table_number_;++i)
			{
				Kmeans<Distance> kmeans(features_);
				std::vector<std::pair<ElementType*, size_t>> result;
				kmeans.iterate(kmeans_it_times_, alpha_*(key_size_), result);
				int j, k;
				//求r-相邻组对
				std::vector<std::pair<int, int>> pairs;
				find_rn_pairs(result, pairs);
				//计算投影向量
				cal_vectors(i,result, pairs);
				KeyType key;
				for (j = 0; j < feature_row_; ++j)
				{
					key = genKey(i,features_[j]);
					tables_[i]->add(key, j);
				}
				for (j = 0; j < result.size(); ++j)
				{
					delete[] result[j].first;
				}
			}

		}

		int knnSearch(const Matrix<ElementType>& queries,
			std::vector<std::vector<int>>& indices,
			std::vector<std::vector<DistanceType>>& dists,
			size_t knn, SearchParam param)
		{
			int rows = queries.rows;
			indices.resize(rows);
			dists.resize(rows);
			int i, goal = param.scale*knn;
			int cnt = 0;
			vector<int> cnts;
			cnts.resize(rows);
            #pragma omp parallel for
			for (i = 0; i < rows; ++i)
			{
			
				table_manager<DistanceType> cand(param.search_table_num, param.search_table_gap);
				int j;
				for (j = 0; j < table_number_&&cand.item_cnt < goal; ++j)
				{
					KeyType key = genKey(j, queries[i]);
					tables_[j]->retrieveBucket(key, (ElementType*)queries[i], cand, goal, distance_);

				}
				cnts[i] = cand.item_cnt;
				cand.retrive(dists[i], indices[i], knn, 0, true);
			}
			for (i = 0; i < rows;++i)
			{
				cnt += cnts[i];
			}
			return cnt / (float)rows;
		}
		int usedMemory()
		{
			int bytes = 0;
			int i;
			for (i = 0; i < table_number_;++i)
			{
				bytes += tables_[i]->usedMemory();
			}
			return bytes;
		}
		int radiusSearch(const Matrix<ElementType>& queries,
			vector<vector<int>>& indices,
			vector<vector<DistanceType>>& dists,
			float radius, SearchParam param)
		{
			return 0;
		}
	private:

		void find_rn_pairs(std::vector<std::pair<ElementType*, size_t>>&groups, std::vector<std::pair<int, int>>&pairs)
		{
			struct cmp
			{
				int index;
				float dis;
				cmp() {}
				cmp(int index_, float dis_) :index(index_), dis(dis_) {}
				bool operator <(const cmp& b) const
				{
					return dis < b.dis;
				}
			};
			int i,j,k;
			int len = groups.size();
			L2<float> dis;
			bool* _b = new bool[len*len];
			memset(_b, false, sizeof(bool)*len*len);
			Matrix<bool> mat(_b, len, len);
			cmp* sp = new cmp[len - 1];
			cmp* psp;
			for (i = 0; i < len;++i)
			{
				psp= sp;
				for (j = 0; j < len;++j)
				{
					if (i != j)
					{
						*psp++=cmp(j,dis(groups[i].first, groups[j].first, feature_col_));
					}
				}
				std::partial_sort(sp, sp + r_, sp+len-1);
				psp = sp;
				for (k = 0; k < r_;++k)
				{
					mat[i][(*psp++).index] = true;
				}
				
			}
			for (i = 0; i < len-1;++i)
			{
				for (j = i+1; j < len;++j)
				{
					if (mat[i][j] || mat[j][i])
					{
						pairs.push_back(std::pair<int, int>(i, j));
					}
				}
			}
			delete[] sp;
			delete[] _b;
			
		}
		inline KeyType genKey(int index,ElementType* point)
		{
				register KeyType key = 0;
				register int i, j;
				register float** v = hashing_vectors_[index];
				register float sum;
				register float* pv,*pp;
				register KeyType mask = 1;
				for (i = 0; i < key_size_; ++i)
				{
					sum = 0.0;
					pv = *v++;
					pp = point;
					for (j = 0; j < feature_col_; ++j)
					{
						sum += *pv++ * *pp++;
					}
					if (sum >= *pv)
					{
						key |= mask;
					}
					mask <<= 1;
				}				
				return key;
		}

		void cal_vectors(int index,std::vector<std::pair<ElementType*, size_t>>& groups, std::vector<std::pair<int, int>>& pairs)
		{
				int i, j, k, gl = groups.size();
				float** cand_vectors=new float*[pairs.size()];
				for (i = 0; i < pairs.size();++i)
				{
					cand_vectors[i] = new float[feature_col_+1];
				}
				for (i = 0; i < pairs.size(); ++i)
				{
					float* vf=cand_vectors[i];
					float* a = groups[pairs[i].first].first, *b = groups[pairs[i].second].first;
					float intercept = 0.0;
					float temp;
					for (j = 0; j < feature_col_; ++j)
					{
						temp = a[j] - b[j];
						*vf++=temp;
						intercept += ((a[j] + b[j]) / 2)*temp;
					}
					*vf=intercept;
				}
				float* weights = new float[gl];
				for (i = 0; i < gl; ++i)
				{
					weights[i] = groups[i].second / (float)feature_row_;
				}
				struct cmp
				{
					int index;
					float weight;
					cmp() {}
					cmp(int index_, float weight_) :index(index_), weight(weight_) {}
					bool operator <(const cmp& b)const
					{
						return weight > b.weight;
					}
				};
				cmp* cand_weights=new cmp[pairs.size()],*pc=cand_weights;
				for (i = 0; i < pairs.size(); ++i)
				{
					float a = 0.0;
					float* pw = weights;
					for (j = 0; j < gl; ++j)
					{
						float sum = 0.0;
						float *pb = groups[j].first;
						float* pa = cand_vectors[i];
						for (k = 0; k < feature_col_; ++k)
						{
							sum += pa[k] * pb[k];
						}
						if (sum >= pa[k])
						{
							a += *pw++;
						}
					}
					float b = 1.0 - a;
					*pc++=cmp(i, -a*(log(a)) - b*(log(b)));
				}
				std::partial_sort(cand_weights, cand_weights + key_size_ , cand_weights+pairs.size());
				for (i = 0; i < key_size_; ++i)
				{
					memcpy(hashing_vectors_[index][i], cand_vectors[cand_weights[i].index], sizeof(float)*(feature_col_+1));
				}

				delete[] weights;
				for (i = 0; i < pairs.size(); ++i)
				{
					delete[] cand_vectors[i];
				}
				delete[] cand_weights;
				delete[] cand_vectors;
			}
	private:
		int table_number_;
		int key_size_;
		float alpha_;
		int kmeans_it_times_;
		int r_;
		ElementType*** hashing_vectors_;
		vector<DshTable<ElementType>*> tables_;
	};
};
#endif

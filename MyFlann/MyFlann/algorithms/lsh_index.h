#pragma once
#ifndef FLANN_LSH_INDEX_H_
#define FLANN_LSH_INDEX_H_

#include <algorithm>
#include <cassert>
#include <cstring>
#include <map>
#include <vector>
#include<bitset>
#include<list>
#include<set>
#include<omp.h>
#include<iostream>
using namespace std;
#include "nn_index.h"
#include "../util/matrix.h"
#include "../util/lsh_table.h"
#include"../util/random.h"
const int MAX_INT = 0x7fffffff;

namespace myflann
{

	struct LshIndexParams : public IndexParams
	{
		LshIndexParams(unsigned int table_number = 20, unsigned int key_size = 40)
		{
			(*this)["algorithm"] = FLANN_LSH_TYPE;
			(*this)["table_number"] = table_number;
			(*this)["key_size"] = key_size;
		}
	};

	template<typename Distance>
	class LshIndex : public NNIndex<Distance>
	{
	public:
		typedef typename Distance::ElementType ElementType;
		typedef typename Distance::ResultType DistanceType;
		typedef NNIndex<Distance> BaseClass;

		LshIndex(const Matrix<ElementType>& features, const IndexParams& params = LshIndexParams(), Distance d = Distance()) :
			BaseClass(features,params, d)
		{
			table_number_ = get_param<unsigned int>(index_params_, "table_number");
			key_size_ = get_param<unsigned int>(index_params_, "key_size");
			size_t i = 0;
			int k = key_size_;
			std::srand((unsigned)time(NULL));
			for (; i < table_number_;++i)
			{
	
				tables_.push_back(new LshTable<ElementType>(feature_col_,k, 
					(ElementType*)features.ptr()));
					k--;
				if (k < 18) {
					k = 18;
				}
			}
		}

		void buildIndex()
		{
			int j = 0;
			#pragma omp parallel for
			for (j = 0; j < table_number_; ++j)//并行为每张哈希表创建索引
			{
				int i;
				for (i = 0; i < feature_row_; ++i)
				{
					tables_[j]->add(i);
				}
			}
		
		}

		//返回哈希表占用的字节数
		int usedMemory() 
		{
			int total = 0;
			int i = 0;
			for (i = 0; i < table_number_;++i)
			{
				total += tables_[i]->usedMemory();
			}
			return total;
		}

		int knnSearch(const Matrix<ElementType>& queries,
			vector<vector<int>>& indices,
			vector<vector<DistanceType>>& dists,
			size_t knn,
			SearchParam param
			) 
		{

			int rows = queries.rows;
			indices.resize(rows);
			dists.resize(rows);
			int i,k, cnt=0;
			int found,goal;
			if (param.scale == 0.0)
			{
				goal = MAX_INT;
			}
			else
			{
				goal = param.scale*knn;
			}
			#pragma omp parallel for
			for (i = 0; i < rows; ++i)
			{
					int j = 0;
					table_manager<DistanceType> cand(param.search_table_num, param.search_table_gap);
					for (j = 0; j < table_number_&&cand.item_cnt<goal; ++j)
					{
						tables_[j]->retrieveBucket(queries[i], &cand, goal, distance_);
					}
					cnt +=cand.item_cnt;
					cand.retrive(dists[i], indices[i], knn,0,true);
			}
			return (int)cnt/(float)rows;
		}
		int radiusSearch(const Matrix<ElementType>& queries,
			vector<vector<int>>& indices,
			vector<vector<DistanceType>>& dists,
			float radius, SearchParam param)
		{

			int rows = queries.rows;
			indices.resize(rows);
			dists.resize(rows);
			int i, k, cnt = 0;
			#pragma omp parallel for
			for (i = 0; i < rows; ++i)
			{
				int j = 0;
				table_manager<DistanceType> cand(param.search_table_num, param.search_table_gap);
				for (j = 0; j < table_number_; ++j)
				{
					tables_[j]->retrieveBucket(queries[i], &cand, MAX_INT, distance_);
				}
				cnt += cand.item_cnt;
				cand.retrive(dists[i], indices[i], 0,radius,false);
			}
			return (int)cnt / (float)rows;
		}

		~LshIndex()
		{
			int i = 0;
			for (i = 0; i < table_number_;++i)
			{
				delete tables_[i];
			}
		}
	private:
		std::vector<LshTable<ElementType>* > tables_;
		int multi_probe_;
		unsigned int table_number_;
		unsigned int key_size_;
	};
}

#endif 
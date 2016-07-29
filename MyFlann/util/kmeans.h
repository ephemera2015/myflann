#pragma once
#ifndef _MYFLANN_KMEANS_2016_07_20_
#define _MYFLANN_KMEANS_2016_07_20_
#include<vector>
#include<set>
#include<omp.h>

namespace myflann
{
	static const float EPS = 0.01;

	template<typename Distance>
	class Kmeans
	{
	public:
		typedef unsigned int IndexType;
		typedef typename Distance::ElementType ElementType;
		typedef typename Distance::ResultType DistanceType;
	public:
		Kmeans(const Matrix<ElementType>& features,Distance distance = Distance()) :features_(features),distance_(distance),
		rows_(features.rows),cols_(features.cols){}
		void iterate(int max_iteration_times, int nb_groups, std::vector<std::pair<ElementType*, size_t>>& groups);
	private:
		void init(int nb_groups, std::vector<std::pair<ElementType*, size_t>>& pre_groups, std::vector<std::pair<ElementType*, size_t>>& groups);
		void update(std::vector<std::pair<ElementType*, size_t>>& groups);
		bool isConverged(std::vector<std::pair<ElementType*, size_t>>& pre_groups,std::vector<std::pair<ElementType*, size_t>>& groups);
	private:
		const Matrix<ElementType>& features_;
		Distance distance_;
		int rows_, cols_;
		int groups_;
	};
	template<typename Distance>
	void  Kmeans<Distance>::init(int nb_groups, std::vector<std::pair<ElementType*, size_t>>& pre_groups, std::vector<std::pair<ElementType*, size_t>>& groups)
	{
		srand(clock());
		pre_groups.resize(nb_groups);
		groups.resize(nb_groups);
		int i,j;
		std::set<IndexType>::iterator it;
		std::set<IndexType> initial_centers;
		while (initial_centers.size() < nb_groups)
		{
			initial_centers.insert(rand() % rows_);
		}
		for (i = 0, it = initial_centers.begin(); i < nb_groups; ++i, ++it)
		{
			pre_groups[i].first = new ElementType[cols_];
			groups[i].first = new ElementType[cols_];
			for (j = 0; j < cols_;++j)
			{
				groups[i].first[j] = features_[*it][j];
			}
		}
	}
	template<typename Distance>
	bool  Kmeans<Distance>::isConverged(std::vector<std::pair<ElementType*, size_t>>& pre_groups,std::vector<std::pair<ElementType*, size_t>>& groups)
	{
		int i,j;
		for (i = 0; i < pre_groups.size();++i)
		{
			for (j = 0; j < cols_;++j)
			{
				if (abs(pre_groups[i].first[j] - groups[i].first[j])>EPS)
				{
					return false;
				}
			}

		}
		return true;
	}
	template<typename Distance>
	void Kmeans<Distance>::update(std::vector<std::pair<ElementType*, size_t>>& groups)
	{
		int k = groups.size();
		int* classes = new int[rows_];
		ElementType* sum = new ElementType[k*cols_];
		memset(sum, 0, k*cols_*sizeof(float));
		int i;
		for (i = 0; i <k;++i)
		{
			groups[i].second = 0;
		}
		
		#pragma omp parallel for
		for (i = 0; i < rows_;++i)
		{
			int j;
			DistanceType min_dis = distance_(features_[i], groups[0].first, cols_),tem;
			int groupid = 0;
			for (j = 1; j < k;++j)
			{
				tem = distance_(features_[i], groups[j].first, cols_);
				if (tem < min_dis)
				{
					min_dis = tem;
					groupid = j;
				}
			}
			classes[i] = groupid;
			
		}
		int j;
		for (i = 0; i < rows_;++i)
		{
			float* pd = features_[i];
			float* ps = sum + classes[i] * cols_;
			for (j = 0; j < cols_; ++j)
			{
				*ps++ += *pd++;
			}
			groups[classes[i]].second++;
		}
		for (i = 0; i < k;++i)
		{
			float* pd = groups[i].first;
			for (j = 0; j < cols_;++j)
			{
				*pd++ = sum[i*cols_ + j] / (float)groups[i].second;
			}
		}
		delete[] sum;
		delete[] classes;
	}
	template<typename Distance>
	void  Kmeans<Distance>::iterate(int max_iteration_times, int nb_groups, std::vector<std::pair<ElementType*, size_t>>& groups)
	{
		int i, j, k;
		std::vector<std::pair<ElementType*, size_t>>  pre_groups;
		init(nb_groups, pre_groups, groups);//随机选取nb_groups个中心
		i = 0;
		do
		{
			for (j = 0; j < nb_groups; ++j)
			{
				memcpy(pre_groups[j].first, groups[j].first, sizeof(ElementType)*cols_);
			}
			update(groups);
			++i;
		} while (i < max_iteration_times&&!isConverged(pre_groups, groups));
		for (i = 0; i < nb_groups;++i)
		{
			delete pre_groups[i].first;
		}
		
	}
};
#endif

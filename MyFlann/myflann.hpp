#pragma once
#ifndef MYFLANN_HPP_
#define MYFLANN_HPP_


#include <vector>
#include <string>
#include <cassert>
#include <cstdio>


#include "util/matrix.h"
#include "algorithms/all_indices.h"

namespace myflann
{
	template<typename Distance>
	class Index
	{
	public:
		typedef typename Distance::ElementType ElementType;
		typedef typename Distance::ResultType DistanceType;
		typedef NNIndex<Distance> IndexType;


		Index(const Matrix<ElementType>& features, const IndexParams& params, Distance distance = Distance())
		{
			flann_algorithm_t index_type = get_param<flann_algorithm_t>(params, "algorithm");
			nnIndex_ = create_index_by_type<Distance>(index_type, features, params, distance);
		}


		virtual ~Index()
		{
			delete nnIndex_;
		}


		void buildIndex()
		{
			nnIndex_->buildIndex();
		}

		int usedMemory() const
		{
			return nnIndex_->usedMemory();
		}

		int knnSearch(const Matrix<ElementType>& queries,
			vector<vector<int>>& indices,
			vector<vector<DistanceType>>& dists,
			size_t knn,
			SearchParam param)
		{
			return nnIndex_->knnSearch(queries, indices, dists, knn,param);
		}

		int radiusSearch(const Matrix<ElementType>& queries,
			vector<vector<int>>& indices,
			vector<vector<DistanceType>>& dists,
			float radius,
			SearchParam param)
		{
			return nnIndex_->radiusSearch(queries, indices, dists, radius,param);
		}
	private:

		IndexType* nnIndex_;
	};
}
#endif /* MYFLANN_HPP_ */


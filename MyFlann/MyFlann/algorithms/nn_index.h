#pragma once

#ifndef MYFLANN_NNINDEX_H
#define MYFLANN_NNINDEX_H

#include <vector>
#include"../algorithms/dist.h"
#include "../util/matrix.h"
#include"../util/params.h"

namespace myflann
{

	class IndexBase
	{
	public:
		virtual ~IndexBase() {};

		virtual int usedMemory()  = 0;

	};
	struct SearchParam
	{
		float scale;
		int   search_table_num;
		int   search_table_gap;
		SearchParam(float scale_, int search_table_num_, int search_table_gap_) :
			scale(scale_),search_table_num(search_table_num_),search_table_gap(search_table_gap_)
		{}
	};
	template <typename Distance>
	class NNIndex : public IndexBase
	{
	public:
		typedef typename Distance::ElementType ElementType;
		typedef typename Distance::ResultType DistanceType;

		NNIndex(const Matrix<ElementType>& features,const IndexParams& params,
			Distance d=Distance()) : features_(features),distance_(d), index_params_(params),feature_row_(features.rows),feature_col_(features.cols)
		{

		}

		virtual ~NNIndex()
		{
		}

		virtual void buildIndex() = 0;

		virtual int knnSearch(const Matrix<ElementType>& queries,
			std::vector<std::vector<int>>& indices,
			std::vector<std::vector<DistanceType>>& dists,
			size_t knn, SearchParam param) = 0;

		virtual int radiusSearch(const Matrix<ElementType>& queries,
			std::vector<std::vector<int>>& indices,
			std::vector<std::vector<DistanceType>>& dists,
			float radius, SearchParam param) = 0;

	protected:

		const Matrix<ElementType>& features_;
		size_t feature_row_;
		size_t feature_col_;
		Distance distance_;

		IndexParams index_params_;

	};




}


#endif 
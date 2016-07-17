#pragma once
#ifndef MYFLANN_ALL_INDICES_H_
#define MYFLANN_ALL_INDICES_H_


#include "nn_index.h"
#include "lsh_index.h"
#include<iostream>

namespace myflann
{

	template<typename Distance>
	inline NNIndex<Distance>*
		create_index_by_type(const flann_algorithm_t index_type,
			const Matrix<typename Distance::ElementType>& dataset, const IndexParams& params, const Distance& distance)
	{
		typedef typename Distance::ElementType ElementType;

		NNIndex<Distance>* nnIndex;

		switch (index_type) {
		case FLANN_LSH_TYPE:
			nnIndex =new  LshIndex<Distance>(dataset, params, distance);
			break;
		default:
			nnIndex = NULL;
		}

		if (nnIndex == NULL) {
			std::cerr << "该索引类型暂不支持" << std::endl;
		}
		return nnIndex;
	}

}

#endif 
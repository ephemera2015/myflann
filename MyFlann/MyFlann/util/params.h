#pragma once
#ifndef FLANN_PARAMS_H_
#define FLANN_PARAMS_H_

#include "any.h"
#include <iostream>
#include <map>
#include<vector>
#include<iostream>

namespace myflann
{

	typedef std::map<std::string, any> IndexParams;
	enum flann_algorithm_t
	{
		FLANN_LSH_TYPE = 0,
		FLANN_DSH_TYPE = 1
	};

	template<typename T>
	T get_param(const IndexParams& params, std::string name)
	{
		IndexParams::const_iterator it = params.find(name);
		if (it != params.end()) {
			return it->second.cast<T>();
		}
		else {
			std::cerr << "不存在参数:" + name << std::endl;
			throw;
		}
	}

}


#endif 
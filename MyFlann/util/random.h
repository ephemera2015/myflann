#pragma once
#ifndef FLANN_RANDOM_H_
#define FLANN_RANDOM_H_
#include<time.h>
#include<iostream>
namespace myflann
{

	template<typename T>
	class Gauss
	{
	public:
		Gauss(T exp = (T) 0 ,T variance = (T)1)
		{
			std::cerr << "该类型的高斯分布暂未支持" << std::endl;
			throw;
		}
		T get()
		{
			;
		}
	private:
		T exp_;
		T variance_;
	};
	template<>
	Gauss<float>::Gauss(float exp,float variance)
	{
		exp_ = exp;
		variance_ = variance;
	}
	template<>
	float Gauss<float>::get()
	{
		static float V1, V2, S;
		static int phase = 0;
		double X;

		if (phase == 0) {
			do {
				float U1 = (double)rand() / RAND_MAX;
				float U2 = (double)rand() / RAND_MAX;

				V1 = 2 * U1 - 1;
				V2 = 2 * U2 - 1;
				S = V1 * V1 + V2 * V2;
			} while (S >= 1 || S == 0);

			X = V1 * sqrt(-2 * log(S) / S);
		}
		else
			X = V2 * sqrt(-2 * log(S) / S);

		phase = 1 - phase;

		return X*variance_+exp_;
	}
	template<typename T>
	class Random
	{
	public:
		Random(T begin=(T)0,T end=(T)RAND_MAX)
		{
			std::cerr << "该类型的Random暂未支持" << std::endl;
			throw;
		}
		T get()
		{
			return 0;
		}
	private:
		T begin_;
		T end_;
	};
	template<>
	Random<int>::Random(int begin,int end )
	{
		begin_ = begin;
		end_ = end;
	}
	template<>
	int Random<int>::get()
	{

		return (unsigned int)(rand() % (end_ - begin_) + begin_);
	}
};
#endif
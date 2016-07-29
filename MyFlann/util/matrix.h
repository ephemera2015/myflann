#pragma once
#ifndef MYFLANN_DATASET_H_
#define MYFLANN_DATASET_H_

#include <stdio.h>

namespace myflann
{

	typedef unsigned char uchar;

	class Matrix_
	{
	public:

		Matrix_() : rows(0), cols(0), stride(0), data(NULL)
		{
		};

		Matrix_(void* data_, size_t rows_, size_t cols_,size_t stride_) :
			rows(rows_), cols(cols_), stride(stride_)
		{
			data = static_cast<uchar*>(data_);
		}

		inline void* operator[](size_t index) const
		{
			return data + index*stride;
		}

		void* ptr() const
		{
			return data;
		}

		size_t rows;
		size_t cols;
		size_t stride;
	protected:
		uchar* data;
	};


	template <typename T>
	class Matrix : public Matrix_
	{
	public:
		typedef T type;

		Matrix() : Matrix_()
		{
		}

		Matrix(T* data_, size_t rows_, size_t cols_) :
			Matrix_(data_, rows_, cols_, sizeof(T)*cols_)
		{

		}

		inline T* operator[](size_t index) const
		{
			return reinterpret_cast<T*>(data + index*stride);
		}


		T* ptr() const
		{
			return reinterpret_cast<T*>(data);
		}
	};

}
#endif
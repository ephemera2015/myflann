#pragma once
#ifndef MYFLANN_TEST_H_
#define MYFLANN_TEST_H_
#include<stdio.h>
#include<string>
#include<vector>
#include<iostream>
#include"./MyFlann/algorithms/dist.h"
#include"MyFlann\util\matrix.h"
using namespace std;
namespace myflann
{
	
	vector<float> precise(Matrix<int>& truth, vector<vector<int>>&found)
	{
		vector<float> rates;
		int i, j, k;

		for (i = 0; i < truth.rows; ++i)
		{
			int cnt = 0;
			for (j = 0; j < found[i].size(); ++j)
			{

				for (k = 0; k < 100; ++k)
				{
					if (found[i][j] == truth[i][k])
					{
						cnt++;
					}
				}
			}
			rates.push_back(cnt);
		}

		return rates;
	}
	template<typename BaseType>
	class DataReader
	{
	public:
		Matrix<BaseType> read(string datafile, int& cnt)
		{
			FILE* f = fopen(datafile.c_str(), "rb");
			if (f == NULL)
			{
				cerr << "打开文件失败！" << endl;
			}
			int dim;
			int t;
			cnt = 0;
			fread(&dim, sizeof(int), 1, f);
			fseek(f, 0, SEEK_END);
			cnt = ftell(f) / (sizeof(int) + dim*sizeof(BaseType));
			fseek(f,0, SEEK_SET);
			BaseType* buf = new BaseType[dim*cnt];
			BaseType* p = buf;
			int i = 0;
			for (i = 0; i < cnt; ++i)
			{
				fread(&t, sizeof(int), 1, f);
				fread(p, sizeof(BaseType), dim, f);
				p += dim;
			}
			Matrix<BaseType> mat(buf, cnt, dim);
			return mat;
		}
	};

}
#endif

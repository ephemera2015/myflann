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

	void precise_recall_rate(Matrix<int>&truth, vector<vector<int>>&found,float& precise,float& recall,int cnt)
	{
		int i, j, k;
		int found_cnt = 0;
		int total_cnt = 0;
		for (i = 0; i < found.size();++i)
		{
			total_cnt += found[i].size();
			for (j = 0; j < found[i].size()&&j<cnt;++j)
			{
				
				for (k = 0; k < cnt;++k)
				{
					if (found[i][j] == truth[i][k])
					{
						found_cnt++;
					}
				}
			}
		}
		recall= found_cnt / ((float)cnt*found.size());
		precise =found_cnt/(float)total_cnt ;
	}
	float precise_rate(Matrix<int>&truth,vector<vector<int>>&found,int cnt)
	{
		int i, j, k;
		int truth_cnt = 0;
		//int found_cnt = 0;
		for (i = 0; i < found.size();++i)
		{
		//	cout << found[i].size() << endl;
			//found_cnt += found[i].size();
			for (k = 0; k < found[i].size()&&k<cnt;++k)
			{
				for (j = 0; j < cnt; ++j)
				{
					if (found[i][k] == truth[i][j])
					{
						truth_cnt++;
						break;
					}
				}
			}
		}
		return truth_cnt/((float)found.size());
	}
	float precise(Matrix<int>& truth, vector<vector<int>>&found,int knn)
	{
		int i, j, k;
		float rates = 0.0;
		for (i = 0; i < truth.rows; ++i)
		{
			for (j = 0; j < found[i].size()&&j<knn; ++j)
			{

				for (k = 0; k < knn; ++k)
				{
					if (found[i][j] == truth[i][k])
					{
						rates=rates+1;
					}
				}
			}
		}
		return rates/(knn*truth.rows);
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

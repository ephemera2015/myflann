#include"MyFlann\myflann.hpp"
#include<time.h>
#include<iomanip>
#include"test.h"
#include<string>
#include<iostream>
#include<fstream>
using namespace std;
using namespace myflann;
//浮点输入LSH测试数据集
const string test_float_flann_base = "data//sift_base.fvecs";
const string test_float_flann_query = "data//sift_query.fvecs";
const string test_float_flann_truth = "data//sift_groundtruth.ivecs";



void test_dsh_precise_time(ostream& out, vector<int>& table_numbers, vector<int>&key_size,vector<float>& scales)
{
	DataReader<float> floatReader;
	DataReader<int>   intReader;
	int base_cnt;
	int query_cnt;
	int truth_cnt;
	Matrix<float> base = floatReader.read(test_float_flann_base, base_cnt);
	Matrix<float> query = floatReader.read(test_float_flann_query, query_cnt);
	Matrix<int>   truth = intReader.read(test_float_flann_truth, truth_cnt);
	out << "********************************DSH测试*********************************" << endl;
	out << endl;
	out << setiosflags(ios::left) << setw(10) << "table_num"
		<< setiosflags(ios::left) << setw(10) << "key_size"
		<< setiosflags(ios::left) << setw(10) << "scales"
		<< setiosflags(ios::left) << setw(8) << "准确率"
		<< setiosflags(ios::left) << setw(14) << "查询用时ms"
		<< setiosflags(ios::left) << setw(14) << "索引用时ms" << endl;
	int i, j, k;
	for (i = 0; i < table_numbers.size(); ++i)
	{
		for (j = 0; j < key_size.size(); ++j)
		{
	
			DshIndexParams p(table_numbers[i], key_size[j], 1.5, 3, 3);
			Index<L2<float>> index(base, p);
			clock_t search_end, search_begin, build_begin, build_end;
			build_begin = clock();
			index.buildIndex();
			build_end = clock();
			for (k = 0; k < scales.size();++k)
			{
				SearchParam sp(scales[k]);
				vector<vector<int>>indices;
				vector<vector<float>> dists;
				search_begin = clock();
				index.knnSearch(query, indices, dists, 1, sp);
				search_end = clock();
				out << setiosflags(ios::left) << setw(10) << table_numbers[i];
				out << setiosflags(ios::left) << setw(10) << key_size[j];
				out << setiosflags(ios::left) << setw(10) << scales[k];
				out << setiosflags(ios::left) << setw(8) << setiosflags(ios::fixed) << setprecision(3) << precise_rate(truth, indices, 100);
				out << setiosflags(ios::left) << setw(14) << setiosflags(ios::fixed) << setprecision(2) << (search_end - search_begin);
				out << setiosflags(ios::left) << setw(14) << setiosflags(ios::fixed) << setprecision(2) << (build_end - build_begin) << endl;
			}
			
		}
	}
}

int main()
{

		streambuf* old = cout.rdbuf();
		cout << "测试进行中，请耐心等待..." << endl;
		//若想将测试结果写入文件，请将下面两行的注释拿掉。
	    //ofstream fout("result.txt");//将在工程目录下生成result.txt文件
		//cout.rdbuf(fout.rdbuf());
		vector<int> table_numbers={80};
		vector<int> key_sizes = {15,23,30,38,45,50,56};
		vector<float> scales = { 1.0,5.0,10.0,20.0,30.0};
		test_dsh_precise_time(cout, table_numbers,key_sizes,scales);//测试dsh有效率
		cout.rdbuf(old);
		cout << "测试已结束" << endl;
	    return 0;
}


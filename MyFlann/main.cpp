#include"MyFlann\myflann.hpp"
#include<time.h>
#include<iomanip>
#include"test.h"
#include<string>
#include<iostream>
#include<fstream>
using namespace std;

//浮点输入LSH测试数据集
const string test_float_flann_base = "data//sift_base.fvecs";
const string test_float_flann_query = "data//sift_query.fvecs";
const string test_float_flann_truth = "data//sift_groundtruth.ivecs";

void testFloatLSH(ostream& out, vector<int>&table_numbers, vector<int>&key_sizes, vector<float>&scales);

int main()
{

		streambuf* old = cout.rdbuf();
		cout << "测试进行中，请耐心等待..." << endl;
		//若想将测试结果写入文件，请将下面两行的注释拿掉。
	    ofstream fout("result.txt");//将在工程目录下生成result.txt文件
		cout.rdbuf(fout.rdbuf());

		/*			 table_numbers:       哈希表数量
					 key_sizes:           key的长度,  1<=key_size<=64
					 scales:              搜索范围，例如搜索100近邻,scale=3时，将会产生100*3=300个候选点，
					 scale越大则精度越高，时间开销越大，scale越小，精度越小，时间开销小
		*/
		vector<int>table_numbers = {15,20,25,30,40};
		vector<int>key_sizes = {30,35,40,45,50,55,60,64};
		vector<float>scales = { 5.0,8.0,10.0,20.0,30,50,80,100,200};
		testFloatLSH(cout, table_numbers, key_sizes, scales);
		cout.rdbuf(old);
		cout << "测试已结束" << endl;
	    return 0;
}




void testFloatLSH(ostream& out, vector<int>&table_numbers, vector<int>&key_sizes, vector<float>&scales)
{
	int i, j, k;
	myflann::DataReader<float> floatReader;//浮点数据读取对象
	myflann::DataReader<int> intReader;//整数数据读取对象

												  //读取浮点数据测试集
	int base_cnt, query_cnt, truth_cnt;
	myflann::Matrix<float> base = floatReader.read(test_float_flann_base, base_cnt);//读取测试数据集
	myflann::Matrix<float> query = floatReader.read(test_float_flann_query, query_cnt);//读取查询数据集
	myflann::Matrix<int> truth = intReader.read(test_float_flann_truth, truth_cnt);//读取事实


	out << "******************************浮点输入的LSH测试*******************************" << endl;
	out << endl;

	out << setiosflags(ios::left) << setw(10) << "table_num"
		<< setiosflags(ios::left) << setw(10) << "key_size"
		<< setiosflags(ios::left) << setw(8) << "scale"
		<< setiosflags(ios::left) << setw(8) << "精度"
		<< setiosflags(ios::left) << setw(14) << "查询用时ms"
		<< setiosflags(ios::left) << setw(14) << "索引用时ms"
		<< setiosflags(ios::left) << setw(14) << "索引内存MB" << endl;
	for (i = 0; i < table_numbers.size(); ++i)
	{
		for (j = 0; j < key_sizes.size(); ++j)
		{
			clock_t build_start, build_end, search_start, search_end;
			myflann::LshIndexParams params(table_numbers[i], key_sizes[j]);
			myflann::Index<myflann::L2<float>> test(base, params);
			//创建索引,并计时
			build_start = clock();
			test.buildIndex();
			build_end = clock();
			for (k = 0; k < scales.size(); ++k)
			{
				float sum;
				//查询并计时
				vector<vector<int>> indices;
				vector<vector<float>> dists;
				myflann::SearchParam param(scales[k], 1000, 1000);
				search_start = clock();
				test.knnSearch(query, indices, dists, 100, param);//找100近邻
				search_end = clock();
				vector<float> precises = myflann::precise(truth, indices);
				int m;
				for (m = 0; m < precises.size(); ++m)
				{
					sum += precises[m];
				}
				sum /= precises.size();
				out << setiosflags(ios::left) << setw(10) << table_numbers[i];
				out << setiosflags(ios::left) << setw(10) << key_sizes[j];
				out << setiosflags(ios::left) << setw(8) << setiosflags(ios::fixed) << setprecision(2) << scales[k];
				out << setiosflags(ios::left) << setw(8) << setiosflags(ios::fixed) << setprecision(3) << sum / 100;
				out << setiosflags(ios::left) << setw(14) << setiosflags(ios::fixed) << setprecision(2) << (search_end - search_start);
				out << setiosflags(ios::left) << setw(14) << setiosflags(ios::fixed) << setprecision(2) << (build_end - build_start);
				out << setiosflags(ios::left) << setw(14) << setiosflags(ios::fixed) << setprecision(2) << test.usedMemory() / (float(1024) * 1024) << endl;
			}
		}
	}
	out << endl;
	delete base.ptr();
	delete query.ptr();
	delete truth.ptr();
}


#include"MyFlann\myflann.hpp"
#include<time.h>
#include<iomanip>
#include"test.h"
#include<string>
#include<iostream>
#include<fstream>
using namespace std;

//浮点输入LSH测试数据集
const string test_float_flann_base = "data//siftsmall_base.fvecs";
const string test_float_flann_query = "data//siftsmall_query.fvecs";
const string test_float_flann_truth = "data//siftsmall_groundtruth.ivecs";

//unsigned char输入LSH测试数据集
const string test_byte_flann_base = "data//bytetest_base.bvecs";
const string test_byte_flann_query = "data//bytetest_query.bvecs";
const string test_byte_flann_truth = "data//bytetest_groundtruth.ivecs";


void testFloatLSH(ostream& out, vector<int>&table_numbers, vector<int>&key_sizes, vector<float>&scales);
void testByteLSH(ostream& out, vector<int>&table_numbers, vector<int>&key_sizes, vector<float>&scales);


int main()
{

		streambuf* old = cout.rdbuf();
		cout << "测试进行中，请耐心等待..." << endl;
		//若想将测试结果写入文件，请将下面两行的注释拿掉。
	   //  ofstream fout("result.txt");
		// cout.rdbuf(fout.rdbuf());

		
		/*			 table_numbers:       哈希表数量
					 key_sizes:           key的长度,  1<=key_size<=64
					 scales:              搜索范围，例如搜索100近邻,scale=3时，将会产生100*3=300个候选点，
					 scale越大则精度越高，时间开销越大，scale越小，精度越小，时间开销小
		*/
		vector<int>table_numbers = { 10,30,50,80,100,150,200};
		vector<int>key_sizes = { 18,20,25,30,40,50,60};
		vector<float>scales = { 2.0,3.0,4.0,5.0,6.0,7.0,8.0,10.0,20.0};
	

		//           测试浮点型输入的LSH 

	//	testFloatLSH(cout, table_numbers, key_sizes, scales);

	
		table_numbers = {500,20,50,80,100,150,200,300,500,800};
		key_sizes = {18,20,22,25,28,30,35,40,50,60};
		scales = { 10.0,30.0,50.0,100.0,200.0,300.0};

		//           测试unsigned char输入的LSH

		testByteLSH(cout, table_numbers, key_sizes, scales);
		
		cout.rdbuf(old);
		cout << "测试已结束" << endl;
	    return 0;
}






void floatLSH_knn_test_utility(ostream& out,myflann::Matrix<float>&base, myflann::Matrix<float>&query, myflann::Matrix<int>& truth,
	int knn, int table_number, int key_size, float scale)

{

	int i = 0;
	float sum;
	clock_t build_start, build_end, search_start, search_end;

	myflann::LshIndexParams params(table_number, key_size);
	myflann::Index<myflann::L2<float>> test(base, params);

	//创建索引,并计时
	build_start = clock();
	test.buildIndex();
	build_end = clock();
	//查询并计时
	vector<vector<int>> indices;
	vector<vector<float>> dists;
	myflann::SearchParam param(scale, 1000, 1000);
	search_start = clock();
	test.knnSearch(query, indices, dists, knn, param);//找100近邻
	search_end = clock();
	vector<float> precises = myflann::precise(truth, indices);
	for (i = 0; i < precises.size(); ++i)
	{
		sum += precises[i];
	}
	sum /= precises.size();
	out << setiosflags(ios::left) << setw(10) << table_number;
	out << setiosflags(ios::left) << setw(10) << key_size;
	out << setiosflags(ios::left) << setw(8) << setiosflags(ios::fixed) << setprecision(2) << scale;
	out << setiosflags(ios::left) << setw(8) << setiosflags(ios::fixed) << setprecision(3) << sum / 100;
	out << setiosflags(ios::left) << setw(14) << setiosflags(ios::fixed) << setprecision(2) << (search_end - search_start);
	out << setiosflags(ios::left) << setw(14) << setiosflags(ios::fixed) << setprecision(2) << (build_end - build_start);
	out	 << setiosflags(ios::left) << setw(14) << setiosflags(ios::fixed) << setprecision(2) << test.usedMemory() / (float(1024) * 1024) << endl;
	
	
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
			for (k = 0; k < scales.size(); ++k)
			{
				floatLSH_knn_test_utility(out, base, query, truth, 100, table_numbers[i], key_sizes[j], scales[k]);
			}
		}
	}
	out << endl;
	delete base.ptr();
	delete query.ptr();
	delete truth.ptr();
}
void byteLSH_knn_test_utility(ostream& out, myflann::Matrix<unsigned char>&base, myflann::Matrix<unsigned char>&query, myflann::Matrix<int>& truth,
	int knn, int table_number, int key_size, float scale)
{

	clock_t build_start, build_end, search_start, search_end;
	myflann::LshIndexParams p(table_number, key_size);
	myflann::Index<myflann::L2<unsigned char>> index(base, p);

	build_start = clock();
	index.buildIndex();
	build_end = clock();
	vector<vector<int>>indices;
	vector<vector<float>> dists;
	myflann::SearchParam param(scale,500, 20000);
	
	search_start = clock();
	index.knnSearch(query, indices, dists, knn, param);
	search_end = clock();
	
	vector<float> rates = myflann::precise(truth, indices);
	int i = 0;
	float sum = 0;

	for (i = 0; i < rates.size(); ++i)
	{
		sum += rates[i];
	}
	sum /= rates.size();
	out << setiosflags(ios::left) << setw(10) << table_number
		<< setiosflags(ios::left) << setw(10) << key_size
		<< setiosflags(ios::left) << setw(8) << setiosflags(ios::fixed) << setprecision(2) << scale
		<< setiosflags(ios::left) << setw(8) << setiosflags(ios::fixed) << setprecision(3) << sum / 100
		<< setiosflags(ios::left) << setw(14) << setiosflags(ios::fixed) << setprecision(2) << (search_end - search_start)
		<< setiosflags(ios::left) << setw(14) << setiosflags(ios::fixed) << setprecision(2) << (build_end - build_start)
		<< setiosflags(ios::left) << setw(14) << setiosflags(ios::fixed) << setprecision(2) << index.usedMemory() / (float(1024) * 1024) << endl;
}
void testByteLSH(ostream& out, vector<int>&table_numbers, vector<int>&key_sizes, vector<float>&scales)
{
	int i, j, k;
	myflann::DataReader<int> intReader;//整数数据读取对象
	myflann::DataReader<unsigned char> byteReader;//unsigned char 型数据读取对象
	int base_cnt, query_cnt, truth_cnt;
	myflann::Matrix<unsigned char> base = byteReader.read(test_byte_flann_base, base_cnt);
	myflann::Matrix<unsigned char> query = byteReader.read(test_byte_flann_query, query_cnt);
	myflann::Matrix<int> truth = intReader.read(test_byte_flann_truth, truth_cnt);
	out << "******************************整数输入的LSH测试*******************************" << endl;
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
			for (k = 0; k < scales.size(); ++k)
			{
				byteLSH_knn_test_utility(out, base, query, truth, 100, table_numbers[i], key_sizes[j], scales[k]);
			}
		}
	}

	out << endl;
	delete base.ptr();
	delete query.ptr();
	delete truth.ptr();
}



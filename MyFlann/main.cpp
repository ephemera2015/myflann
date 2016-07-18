#include"MyFlann\myflann.hpp"
#include<time.h>
#include<iomanip>
#include"test.h"
#include<string>
#include<iostream>
#include<fstream>
using namespace std;

//��������LSH�������ݼ�
const string test_float_flann_base = "data//siftsmall_base.fvecs";
const string test_float_flann_query = "data//siftsmall_query.fvecs";
const string test_float_flann_truth = "data//siftsmall_groundtruth.ivecs";

//unsigned char����LSH�������ݼ�
const string test_byte_flann_base = "data//bytetest_base.bvecs";
const string test_byte_flann_query = "data//bytetest_query.bvecs";
const string test_byte_flann_truth = "data//bytetest_groundtruth.ivecs";


void testFloatLSH(ostream& out, vector<int>&table_numbers, vector<int>&key_sizes, vector<float>&scales);
void testByteLSH(ostream& out, vector<int>&table_numbers, vector<int>&key_sizes, vector<float>&scales);


int main()
{

		streambuf* old = cout.rdbuf();
		cout << "���Խ����У������ĵȴ�..." << endl;
		//���뽫���Խ��д���ļ����뽫�������е�ע���õ���
	   //  ofstream fout("result.txt");
		// cout.rdbuf(fout.rdbuf());

		
		/*			 table_numbers:       ��ϣ������
					 key_sizes:           key�ĳ���,  1<=key_size<=64
					 scales:              ������Χ����������100����,scale=3ʱ���������100*3=300����ѡ�㣬
					 scaleԽ���򾫶�Խ�ߣ�ʱ�俪��Խ��scaleԽС������ԽС��ʱ�俪��С
		*/
		vector<int>table_numbers = { 10,30,50,80,100,150,200};
		vector<int>key_sizes = { 18,20,25,30,40,50,60};
		vector<float>scales = { 2.0,3.0,4.0,5.0,6.0,7.0,8.0,10.0,20.0};
	

		//           ���Ը����������LSH 

	//	testFloatLSH(cout, table_numbers, key_sizes, scales);

	
		table_numbers = {500,20,50,80,100,150,200,300,500,800};
		key_sizes = {18,20,22,25,28,30,35,40,50,60};
		scales = { 10.0,30.0,50.0,100.0,200.0,300.0};

		//           ����unsigned char�����LSH

		testByteLSH(cout, table_numbers, key_sizes, scales);
		
		cout.rdbuf(old);
		cout << "�����ѽ���" << endl;
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

	//��������,����ʱ
	build_start = clock();
	test.buildIndex();
	build_end = clock();
	//��ѯ����ʱ
	vector<vector<int>> indices;
	vector<vector<float>> dists;
	myflann::SearchParam param(scale, 1000, 1000);
	search_start = clock();
	test.knnSearch(query, indices, dists, knn, param);//��100����
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
	myflann::DataReader<float> floatReader;//�������ݶ�ȡ����
	myflann::DataReader<int> intReader;//�������ݶ�ȡ����

												  //��ȡ�������ݲ��Լ�
	int base_cnt, query_cnt, truth_cnt;
	myflann::Matrix<float> base = floatReader.read(test_float_flann_base, base_cnt);//��ȡ�������ݼ�
	myflann::Matrix<float> query = floatReader.read(test_float_flann_query, query_cnt);//��ȡ��ѯ���ݼ�
	myflann::Matrix<int> truth = intReader.read(test_float_flann_truth, truth_cnt);//��ȡ��ʵ


	out << "******************************���������LSH����*******************************" << endl;
	out << endl;

	out << setiosflags(ios::left) << setw(10) << "table_num"
		<< setiosflags(ios::left) << setw(10) << "key_size"
		<< setiosflags(ios::left) << setw(8) << "scale"
		<< setiosflags(ios::left) << setw(8) << "����"
		<< setiosflags(ios::left) << setw(14) << "��ѯ��ʱms"
		<< setiosflags(ios::left) << setw(14) << "������ʱms"
		<< setiosflags(ios::left) << setw(14) << "�����ڴ�MB" << endl;
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
	myflann::DataReader<int> intReader;//�������ݶ�ȡ����
	myflann::DataReader<unsigned char> byteReader;//unsigned char �����ݶ�ȡ����
	int base_cnt, query_cnt, truth_cnt;
	myflann::Matrix<unsigned char> base = byteReader.read(test_byte_flann_base, base_cnt);
	myflann::Matrix<unsigned char> query = byteReader.read(test_byte_flann_query, query_cnt);
	myflann::Matrix<int> truth = intReader.read(test_byte_flann_truth, truth_cnt);
	out << "******************************���������LSH����*******************************" << endl;
	out << endl;

	out << setiosflags(ios::left) << setw(10) << "table_num"
		<< setiosflags(ios::left) << setw(10) << "key_size"
		<< setiosflags(ios::left) << setw(8) << "scale"
		<< setiosflags(ios::left) << setw(8) << "����"
		<< setiosflags(ios::left) << setw(14) << "��ѯ��ʱms"
		<< setiosflags(ios::left) << setw(14) << "������ʱms"
		<< setiosflags(ios::left) << setw(14) << "�����ڴ�MB" << endl;
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



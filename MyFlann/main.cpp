#include"MyFlann\myflann.hpp"
#include<time.h>
#include<iomanip>
#include"test.h"
#include<string>
#include<iostream>
#include<fstream>
using namespace std;

//��������LSH�������ݼ�
const string test_float_flann_base = "data//sift_base.fvecs";
const string test_float_flann_query = "data//sift_query.fvecs";
const string test_float_flann_truth = "data//sift_groundtruth.ivecs";

void testFloatLSH(ostream& out, vector<int>&table_numbers, vector<int>&key_sizes, vector<float>&scales);

int main()
{

		streambuf* old = cout.rdbuf();
		cout << "���Խ����У������ĵȴ�..." << endl;
		//���뽫���Խ��д���ļ����뽫�������е�ע���õ���
	    ofstream fout("result.txt");//���ڹ���Ŀ¼������result.txt�ļ�
		cout.rdbuf(fout.rdbuf());

		/*			 table_numbers:       ��ϣ������
					 key_sizes:           key�ĳ���,  1<=key_size<=64
					 scales:              ������Χ����������100����,scale=3ʱ���������100*3=300����ѡ�㣬
					 scaleԽ���򾫶�Խ�ߣ�ʱ�俪��Խ��scaleԽС������ԽС��ʱ�俪��С
		*/
		vector<int>table_numbers = {15,20,25,30,40};
		vector<int>key_sizes = {30,35,40,45,50,55,60,64};
		vector<float>scales = { 5.0,8.0,10.0,20.0,30,50,80,100,200};
		testFloatLSH(cout, table_numbers, key_sizes, scales);
		cout.rdbuf(old);
		cout << "�����ѽ���" << endl;
	    return 0;
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
			clock_t build_start, build_end, search_start, search_end;
			myflann::LshIndexParams params(table_numbers[i], key_sizes[j]);
			myflann::Index<myflann::L2<float>> test(base, params);
			//��������,����ʱ
			build_start = clock();
			test.buildIndex();
			build_end = clock();
			for (k = 0; k < scales.size(); ++k)
			{
				float sum;
				//��ѯ����ʱ
				vector<vector<int>> indices;
				vector<vector<float>> dists;
				myflann::SearchParam param(scales[k], 1000, 1000);
				search_start = clock();
				test.knnSearch(query, indices, dists, 100, param);//��100����
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


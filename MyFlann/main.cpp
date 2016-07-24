#include"MyFlann\myflann.hpp"
#include<time.h>
#include<iomanip>
#include"test.h"
#include<string>
#include<iostream>
#include<fstream>
using namespace std;
using namespace myflann;
//��������LSH�������ݼ�
const string test_float_flann_base = "data//sift_base.fvecs";
const string test_float_flann_query = "data//sift_query.fvecs";
const string test_float_flann_truth = "data//sift_groundtruth.ivecs";


void test_lsh_precise_time(ostream& out, vector<int>& table_numbers, vector<int>&key_size)
{
	DataReader<float> floatReader;
	DataReader<int>   intReader;
	int base_cnt;
	int query_cnt;
	int truth_cnt;
	Matrix<float> base = floatReader.read(test_float_flann_base, base_cnt);
	Matrix<float> query = floatReader.read(test_float_flann_query, query_cnt);
	Matrix<int>   truth = intReader.read(test_float_flann_truth, truth_cnt);
	out << "********************************LSH����*********************************" << endl;
	out << endl;
	out << setiosflags(ios::left) << setw(10) << "table_num"
		<< setiosflags(ios::left) << setw(10) << "key_size"
		<< setiosflags(ios::left) << setw(8) << "׼ȷ��"
		<< setiosflags(ios::left) << setw(14) << "��ѯ��ʱms"
		<< setiosflags(ios::left) << setw(14) << "������ʱms" << endl;
	int i, j, k;
	for (i = 0; i < table_numbers.size(); ++i)
	{
		for (j = 0; j < key_size.size(); ++j)
		{

			LshIndexParams p(table_numbers[i], key_size[j]);
			Index<L2<float>> index(base, p);
			clock_t search_end, search_begin, build_begin, build_end;
			build_begin = clock();
			index.buildIndex();
			build_end = clock();
			SearchParam sp(100.0);
			vector<vector<int>>indices;
			vector<vector<float>> dists;
			search_begin = clock();
			index.knnSearch(query, indices, dists, 1, sp);
			search_end = clock();
			out << setiosflags(ios::left) << setw(10) << table_numbers[i];
			out << setiosflags(ios::left) << setw(10) << key_size[j];
			out << setiosflags(ios::left) << setw(8) << setiosflags(ios::fixed) << setprecision(3) << precise_rate(truth, indices, 100);
			out << setiosflags(ios::left) << setw(14) << setiosflags(ios::fixed) << setprecision(2) << (search_end - search_begin);
			out << setiosflags(ios::left) << setw(14) << setiosflags(ios::fixed) << setprecision(2) << (build_end - build_begin) << endl;
		}
	}
}
void test_dsh_precise_time(ostream& out, vector<int>& table_numbers, vector<int>&key_size)
{
	DataReader<float> floatReader;
	DataReader<int>   intReader;
	int base_cnt;
	int query_cnt;
	int truth_cnt;
	Matrix<float> base = floatReader.read(test_float_flann_base, base_cnt);
	Matrix<float> query = floatReader.read(test_float_flann_query, query_cnt);
	Matrix<int>   truth = intReader.read(test_float_flann_truth, truth_cnt);
	out << "********************************DSH����*********************************" << endl;
	out << endl;
	out << setiosflags(ios::left) << setw(10) << "table_num"
		<< setiosflags(ios::left) << setw(10) << "key_size"
		<< setiosflags(ios::left) << setw(8) << "׼ȷ��"
		<< setiosflags(ios::left) << setw(14) << "��ѯ��ʱms"
		<< setiosflags(ios::left) << setw(14) << "������ʱms" << endl;
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
			SearchParam sp(100.0);
			vector<vector<int>>indices;
			vector<vector<float>> dists;
			search_begin = clock();
			index.knnSearch(query, indices, dists, 1, sp);
			search_end = clock();
			out << setiosflags(ios::left) << setw(10) << table_numbers[i];
			out << setiosflags(ios::left) << setw(10) << key_size[j];
			out << setiosflags(ios::left) << setw(8) << setiosflags(ios::fixed) << setprecision(3) << precise_rate(truth, indices, 100);
			out << setiosflags(ios::left) << setw(14) << setiosflags(ios::fixed) << setprecision(2) << (search_end - search_begin);
			out << setiosflags(ios::left) << setw(14) << setiosflags(ios::fixed) << setprecision(2) << (build_end - build_begin) << endl;
		}
	}
}

void compare_dsh_lsh(ostream& out)
{
	DataReader<float> floatReader;
	DataReader<int>   intReader;
	int base_cnt;
	int query_cnt;
	int truth_cnt;
	Matrix<float> base  = floatReader.read(test_float_flann_base, base_cnt);
	Matrix<float> query = floatReader.read(test_float_flann_query, query_cnt);
	Matrix<int>   truth = intReader.read(test_float_flann_truth, truth_cnt);

	int i, j, k;

	clock_t tds, tde, tls, tle;
	out << "*****************************DSH��LSH�ԱȲ���*********************************" << endl;
	out << endl;
	vector<int> key_sizes = {8,10,15,17,20,23,25,30,32};
	out << setiosflags(ios::left) << setw(10) << "key_size"
		<< setiosflags(ios::left) << setw(10) << "DSH����"
		<< setiosflags(ios::left) << setw(10) << "LSH����"
		<< setiosflags(ios::left) << setw(10) << "DSH��ѡ"
		<< setiosflags(ios::left) << setw(10) << "LSH��ѡ"
		<< setiosflags(ios::left) << setw(10) << "DSHʱ��"
		<< setiosflags(ios::left) << setw(10) << "LSHʱ��" << endl;
	for (i = 0; i < key_sizes.size();++i)
	{
		DshIndexParams pdsh(10, key_sizes[i], 1.5, 3, 3);
		Index<L2<float>> dsh(base, pdsh);
		LshIndexParams plsh(10, key_sizes[i]);
		Index<L2<float>> lsh(base, plsh);
		dsh.buildIndex();
		lsh.buildIndex();
		vector<vector<int>> indices;
		vector<vector<float>> dists;

		tds = clock();
		float cand_dsh=dsh.knnSearch(query, indices, dists, 1, SearchParam(20.0));
		tde = clock();
		float prs_dsh = precise_rate(truth, indices, 100);
		indices.clear();
		dists.clear();
		tls = clock();
		float cand_lsh=lsh.knnSearch(query, indices, dists, 1, SearchParam(20.0));
		tle = clock();
		float prs_lsh = precise_rate(truth, indices, 100);
		out << setiosflags(ios::left) << setw(10) << key_sizes[i]
			<< setiosflags(ios::left) << setw(10) << prs_dsh
			<< setiosflags(ios::left) << setw(10) << prs_lsh
			<< setiosflags(ios::left) << setw(10) << cand_dsh
			<< setiosflags(ios::left) << setw(10) << cand_lsh
			<< setiosflags(ios::left) << setw(10) << tde-tds
			<< setiosflags(ios::left) << setw(10) << tle-tls << endl;
	}

}
int main()
{

		streambuf* old = cout.rdbuf();
		cout << "���Խ����У������ĵȴ�..." << endl;
		//���뽫���Խ��д���ļ����뽫�������е�ע���õ���
	    // ofstream fout("result.txt");//���ڹ���Ŀ¼������result.txt�ļ�
		//cout.rdbuf(fout.rdbuf());
		vector<int> table_numbers={1,3,5,10,15,20,25};
		vector<int> key_sizes = {30,33,38,43,50};
		test_dsh_precise_time(cout, table_numbers,key_sizes);//����dsh��Ч��
		table_numbers = {1,3,5,15,20};
		key_sizes = { 15,20,30,35,40,45 };
		test_lsh_precise_time(cout,table_numbers,key_sizes);//����lsh��Ч��
		compare_dsh_lsh(cout);//����dsh��lsh������ͬ��������Ч�ʵĶԱȹ�ϵ
		cout.rdbuf(old);
		cout << "�����ѽ���" << endl;
	    return 0;
}


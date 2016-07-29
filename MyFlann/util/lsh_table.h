#pragma once
#include<iostream>
#include<vector>
#include<map>
#include<set>
#include<bitset>
#include"../util/random.h"
#include<algorithm>
#include<numeric>
#include<iterator>

using namespace std;

namespace myflann
{
	template<typename DistanceType>
	struct table_item
	{
		table_item() :next(NULL) {}
		DistanceType distance;
		unsigned int index;
		table_item* next;
	};

	template<typename DistanceType>
	struct table_head
	{
		bool has_element;
		table_item<DistanceType>* item_list;

		table_head() :has_element(false)
		{
			item_list = new table_item<DistanceType>;
		}
		~table_head()
		{
			clear();
			delete item_list;
		}
		inline bool add(DistanceType distance, unsigned int index)
		{
			table_item<DistanceType>* p = item_list->next;
			table_item<DistanceType>* q = item_list;
			while (p)
			{
				if (p->index == index)
				{
					return false;
				}
				else
				{
					if (p->distance <= distance)
					{
						q = p;
						p = p->next;
					}
					else
					{
						break;
					}
				}
			}
			//此时p要么为空，要么指向第一个>=distance的节点
			table_item<DistanceType>*r = new table_item<DistanceType>;
			r->distance = distance;
			r->index = index;
			q->next = r;
			r->next = p;
			return true;
		}
		inline void clear()
		{
			table_item<DistanceType>* p = item_list->next;
			table_item<DistanceType>* q = item_list;
			has_element = false;
			while (p)
			{
				q = p;
				p = p->next;
				delete q;
			}
			item_list->next = NULL;
		}
	};

	template<typename DistanceType>
	class table_manager
	{
	public:
		table_manager(unsigned int table_number, unsigned int gap) :table_number_(table_number), gap_(gap), item_cnt(0)
		{
			table_ = new table_head<DistanceType>[table_number];
		}
		~table_manager()
		{
			if (table_)
				delete[] table_;
		}
		inline void retrive(vector<DistanceType>& dis,vector<int>& index,unsigned int num,DistanceType max_distance,bool bknn=true)
		{
			unsigned int cnt = 0;
			register int i = 0;
			table_item<DistanceType>* p ;
			if (bknn)
			{
				for (i = 0; i < table_number_; ++i)
				{
					if (table_[i].has_element)
					{
						p = table_[i].item_list->next;
						while (p)
						{
							dis.push_back(p->distance);
							index.push_back(p->index);
							cnt++;
							if (cnt == num)
								return;
							p = p->next;
						}
					}
				}
			}
			else
			{
				for (i = 0; i < table_number_; ++i)
				{
					if (table_[i].has_element)
					{
						p = table_[i].item_list->next;
						while (p)
						{
							if (p->distance <= max_distance)
							{
								dis.push_back(p->distance);
								index.push_back(p->index);
								p = p->next;
							}
							else
							{
								return;
							}
						}
					}
				}
			}
		}

		inline void add(DistanceType distance, unsigned int index)
		{

			unsigned int which = (unsigned int)(distance/gap_)%table_number_;
			if (table_[which].add(distance, index))
			{
				table_[which].has_element = true;
				item_cnt++;
			}
		}

		inline void clear()
		{
			register int i = 0;
			item_cnt = 0;
			for (; i < table_number_; ++i)
			{
				table_[i].clear();
			}
		}
	public:
		unsigned int item_cnt;
	private:
		unsigned int table_number_;
		unsigned int gap_;
		
		table_head<DistanceType>* table_;
	};


	typedef unsigned int IndexType;

	template<typename ElementType>
	class LshTable
	{
	public:
		
		typedef unsigned __int64 KeyType;
		typedef vector<IndexType> Bucket;
		typedef map<KeyType, Bucket> Table;
		typedef vector<size_t> Selector;

	public:
		LshTable(size_t dimension, size_t keySize, ElementType* data)
		{
			std::cerr << "该类型的LSH尚未实现" << std::endl;
			throw;
		}
		~LshTable()
		{
			
		}
		void clear() {}
		template<typename Distance,typename DistanceType>
		void retrieveBucket(ElementType* point,table_manager<DistanceType>* cand,int goal, Distance& distance)
		{

		}
		int usedMemory()
		{
			int bytes = table_.size()*sizeof(KeyType);
			Table::iterator it = table_.begin();
			for (; it != table_.end();++it)
			{
				bytes += it->second.size()*sizeof(IndexType);
			}
			return bytes;
		}
		//添加一个元素
		void add(IndexType value)
		{

		}

	private:
		Table table_;
		size_t keySize_;
		size_t dimension_;
		Selector selector_;
		ElementType** selectorf_;
		const ElementType* data_;
	};
	template<>
	template<typename Distance,typename DistanceType>
	void LshTable<float>::retrieveBucket(float* point, table_manager<DistanceType>* cand,int goal,Distance& distance)
	{

		int i, j,len=0;
		float sum,dis;
		KeyType key=0,subkey=0;
		KeyType mask = 1;
		Table::iterator it;
		IndexType index;

		for (i = 0; i < keySize_; ++i)//依次计算KeySize个基本哈希值
		{
			sum = 0;
			for (j = 0; j < dimension_; ++j)
			{
				sum += point[j] * selectorf_[i][j];
			}
			if (sum >= 0)
			{
				key |= mask;
			}
			mask <<= 1;
		}
		it = table_.find(key);
		if (it != table_.end())
		{
			vector<IndexType>& v = it->second;
			len = v.size();
			for (i = 0; i < len && cand->item_cnt<goal; ++i)
			{
				index = v[i];
				dis = distance((float*)data_ + index * dimension_, point, dimension_);
				cand->add(dis, index);
			}
		}
		mask = 1;

		for (i = 0; i < keySize_&&cand->item_cnt < goal;++i)
		{
			subkey = key;
			subkey ^= mask;
			mask <<= 1;
			it = table_.find(subkey);
			if (it != table_.end())
			{
				vector<IndexType>& v = it->second;
				len = v.size();
				for (j = 0; j < len&&cand->item_cnt < goal; ++j)
				{
					index = v[j];
					dis = distance((float*)data_ + index * dimension_, point, dimension_);
					cand->add(dis, index);
				}
			}
		}	
		unsigned int a = rand() % keySize_;
		unsigned int b = rand() % keySize_;
		while (b == a)
			b = rand() % keySize_;
		mask = (1<<a)|(1<<b);
		for (i = 0; i < keySize_&&cand->item_cnt<goal; ++i)
		{
			subkey = key;
			subkey ^= mask;
			mask <<= 1;
			it = table_.find(subkey);
			if (it != table_.end())
			{
				vector<IndexType>& v = it->second;
				len = v.size();
				for (j = 0; j < len&&cand->item_cnt < goal; ++j)
				{
					index=v[j];
					dis = distance((float*)data_ + index * dimension_, point, dimension_);
					cand->add(dis, index);
				}
			}
		}
		
	}
	template<>
	void LshTable<float>::add(IndexType index)
	{
		int i,j;
		float sum;
		KeyType key=0;
		KeyType mask = 1;
		for (i = 0; i < keySize_;++i)//依次计算KeySize个基本哈希值
		{
			sum = 0;
			for (j = 0; j < dimension_;++j)
			{
				sum += data_[index*dimension_+j] * selectorf_[i][j];
			}
			if (sum >= 0)
			{
				key|=mask;
			}
			mask <<= 1;
		}
		table_[key].push_back(index);

	}
	template<>
	void LshTable<float>::clear()
	{
		int i = 0;
		for (i = 0; i < keySize_;++i)
		{
			delete[] selectorf_[i];
		}
		delete[] selectorf_;
	}
	template<>
	LshTable<float>::LshTable(size_t dimension, size_t keySize, float* data)  
	{
		data_ = data;
		keySize_ = keySize;
		dimension_ = dimension;
		Gauss<float> gauss;
		int i = 0;
		int j = 0;
		selectorf_ = new float*[keySize];
		for (i = 0; i < keySize_;++i)
		{
			selectorf_[i] = new float[dimension_];
		}
		for (i = 0; i < keySize; ++i)//产生KeySize个基本哈希函数
		{
			for (j = 0; j < dimension_;++j)
			{
				selectorf_[i][j] = gauss.get();
			}
		}
	}

	template<>
	LshTable<unsigned char>::LshTable(size_t dimension, size_t keySize, unsigned char* data)
	{
		data_ = data;
		keySize_ = keySize;
		dimension_ = dimension;
		Random<int> rand(0, dimension_*256);
		size_t i = 0;
		for (; i < keySize_; ++i)
		{
			selector_.push_back(rand.get());
		}
	}

	template<>
	void LshTable<unsigned char>::add(IndexType index)
	{
		size_t offset = index*dimension_;
		KeyType key = 0;
		int i = 0;
		for (i = 0; i < keySize_;++i)
		{
			key |= data_[offset + selector_[i] / 256]>(selector_[i] % 256) ?(((KeyType)1) << i):0;
		}
		table_[key].push_back(index);

	}

	template<>
	template<typename Distance,typename DistanceType>
	void LshTable<unsigned char>::retrieveBucket(unsigned char* point,
		table_manager<DistanceType>* cand, int goal, Distance& distance)

	{

		DistanceType dis;
		KeyType key = 0, subkey, mask;
		int i = 0, j = 0, len = 0, index = 0;
		for (i = 0; i < keySize_; ++i)
		{
			key |= point[selector_[i] / 256]>(selector_[i] % 256) ? (((KeyType)1) << i) : 0;
		}
		Table::iterator ip = table_.find(key);
		if (ip != table_.end())
		{
			vector<IndexType>& v = ip->second;
			len = v.size();
			for (j = 0; j < len&&cand->item_cnt<goal; ++j)
			{
				index = v[j];
				dis = distance((unsigned char*)data_ + index * dimension_, point, dimension_);
				cand->add(dis, index);
			}
		}
	/*	for (i = 0; i < keySize_;++i)
		{
			subkey = key ^ (1 << i);
			Table::iterator ip = table_.find(subkey);
			if (ip != table_.end())
			{
				vector<IndexType>& v = ip->second;
				len = v.size();
				for (j = 0; j < len&&cand->item_cnt<goal; ++j)
				{
					index = v[j];
					dis = distance((unsigned char*)data_ + index * dimension_, point, dimension_);
					cand->add(dis, index);
				}
			}
		}*/
	}

};

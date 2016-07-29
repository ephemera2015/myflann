#pragma once
#ifndef _MYFLANN_DSH_TABLE_
#define _MYFLANN_DSH_TABLE_
#include<vector>
#include<map>
#include"lsh_table.h"
namespace myflann
{
	typedef unsigned int IndexType;
	typedef unsigned __int64 KeyType;
    template<typename ElementType>
	class DshTable
	{
	public:
		typedef std::vector<IndexType> Bucket;
		typedef std::map<KeyType, Bucket> Table;
	public:
		DshTable(size_t dimension, ElementType* data)
		{
			dim_ = dimension;
			pdata = data;
		}
		inline void add(KeyType key, IndexType index)
		{
			table_[key].push_back(index);
		}
		template<typename Distance,typename DistanceType>
		inline void retrieveBucket(KeyType key,ElementType*point,table_manager<DistanceType>& cand,int goal, Distance& distance)
		{
			register int i,len;
			Table::iterator it = table_.find(key);
			if (it != table_.end())
			{
				len = it->second.size();
				Bucket& b = it->second;
				for (i = 0; i < len&&cand.item_cnt<goal; ++i)
				{
					cand.add(distance(point,pdata+((b[i])*dim_),dim_), b[i]);
				}
			}
		}
		int usedMemory()
		{
			int bytes = 0;
			bytes += sizeof(KeyType)*table_.size();
			Table::iterator it = table_.begin();
			for (; it != table_.end();++it)
			{
				bytes += sizeof(IndexType)*it->second.size();
			}
			return bytes;
		}
	private:
		int dim_;
		ElementType* pdata;
		Table table_;
	};
};
#endif

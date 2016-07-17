#pragma once
#include<vector>
#include<math.h>
namespace myflann
{
	template<typename T>
	struct L1
	{
		typedef T ElementType;
		typedef unsigned int ResultType;
		ResultType operator()(T* a, T* b, int len) const
		{
			register ResultType res = 0, r1, r2, r3, r4;
			register int i = 0;
			register int loop = len / 4;
			int remain = len % 4;
			for (i = 0; i<loop; ++i)
			{
				r1 = (*a - *b);
				r2 = (*(a + 1) - *(b + 1));
				r3 = (*(a + 2) - *(b + 2));
				r4 = (*(a + 3) - *(b + 3));
				a += 4;
				b += 4;
				r1 = r1 > 0 ? r1 : -r1;
				r2 = r2 > 0 ? r2 : -r2;
				r3 = r3 > 0 ? r3 : -r3;
				r4 = r4 > 0 ? r4 : -r4;
				res += r1 + r2 +r3 +r4;
			}
			for (i = 0; i < remain; ++i)
			{
				r1 = *a++ - *b++;
				res += r1>0?r1:-r1;
			}
			return res;
		}
	};

	template<class T>
	struct L2
	{
		typedef T ElementType;
		typedef float ResultType;
		ResultType operator()(T* a, T* b, int len) const
		{
			register ResultType res = 0,r1,r2,r3,r4;
			register int i = 0;
			register int loop = len / 4;
			int remain = len % 4;
			for (i = 0; i<loop; ++i)
			{
				r1= (*a - *b);
				r2= (*(a + 1) - *(b + 1));
				r3= (*(a + 2) - *(b + 2));
				r4= (*(a + 3) - *(b + 3));
				a += 4;
				b += 4;
				res += r1*r1 + r2*r2 + r3*r3 + r4*r4;
			}
			for (i = 0; i < remain;++i)
			{
				r1 = *a++ - *b++;
				res += r1*r1;
			}
			return res;
		}
	};
};
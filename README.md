# myflann
my fast library of  approximate nearest neighbor search

第一次使用github也是第一次写readme，不规范之处还请批评指正！
------------------------------------------------------------

程序编译运行方法
------------------------------------------------------------
    (1)clone本项目到本地
    (2)自行下载ANN_SIFT1M测试数据集放到data/文件夹下（因为这个文件有点大，所以没有上传）
    (3)使用新版的visual studio 打开根目录下工程文件Myflann.sln
    (4)编译运行
  
为了是您更好的理解测试过程，下面就main.cpp中测试代码进行说明：
--------------------------------------------------------------
    (1)以下三句指定了测试数据集文件所在的位置
```c++
const string test_float_flann_base = "data//sift_base.fvecs";
const string test_float_flann_query = "data//sift_query.fvecs";
const string test_float_flann_truth = "data//sift_groundtruth.ivecs";
```
    (2)下面是main函数中的代码
```c++
		streambuf* old = cout.rdbuf();
		cout << "测试进行中，请耐心等待..." << endl;
		//若想将测试结果写入文件，请将下面两行的注释拿掉。
	  // ofstream fout("result.txt");//将在工程目录下生成result.txt文件
		//cout.rdbuf(fout.rdbuf());
		
		//以下两行指明了dsh测试时使用的参数
		vector<int> table_numbers={1,3,5,10,15,20,25};
		vector<int> key_sizes = {30,33,38,43,50};
		test_dsh_precise_time(cout, table_numbers,key_sizes);//测试dsh有效率
		//以下两行指明了lsh测试时使用的参数
		table_numbers = {1,3,5,15,20};
		key_sizes = { 15,20,30,35,40,45 };
		test_lsh_precise_time(cout,table_numbers,key_sizes);//测试lsh有效率
		
		//下面的函数调用是比较dsh和lsh回收相同点数，有效率的对比关系
		compare_dsh_lsh(cout);
		cout.rdbuf(old);
		cout << "测试已结束" << endl;
	        return 0;
```

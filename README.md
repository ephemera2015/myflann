# myflann
my fast library of  approximate nearest neighbor search
------------------------------------------------------------

程序编译运行方法
------------------------------------------------------------
    (1)clone本项目到本地
    (2)自行下载ANN_SIFT1M测试数据集放到data/文件夹下（因为这个文件有点大，所以没有上传）
    (3)使用新版的visual studio 打开根目录下工程文件Myflann.sln
    (4)编译选项选择64位，并添加openmp支持  
    (5)编译运行
  
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
 	    	//ofstream fout("result.txt");//将在工程目录下生成result.txt文件 
		//cout.rdbuf(fout.rdbuf()); 
 		vector<int> table_numbers={80}; 
 		vector<int> key_sizes = {15,23,30,38,45,50,56}; 
 		vector<float> scales = { 1.0,5.0,10.0,20.0,30.0}; 
 		test_dsh_precise_time(cout, table_numbers,key_sizes,scales);//测试dsh有效率 
 		cout.rdbuf(old); 
 		cout << "测试已结束" << endl; 
 	    	return 0; 

```

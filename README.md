myflann
=====================
    第一次使用github也是第一次写README.md，不符合规范的地方请批评指正！
代码运行方法
---------------------
    (1)clone本工程到本地
    (2)使用Visual Studio （最好是新版的）打开根目录下MyFlann.sln
    (3)编译运行
测试代码解释
---------------------
    在main.cpp中写好了测试代码，下面解释一下关键部分，以方便您修改参数进行实验
    (1)测试数据集，如果您需要更换测试数据集，请修改下面6行
```c++
//浮点输入LSH测试数数据集
const string test_float_flann_base = "data//siftsmall_base.fvecs";
const string test_float_flann_query = "data//siftsmall_query.fvecs";
const string test_float_flann_truth = "data//siftsmall_groundtruth.ivecs";

//unsigned char输入LSH测试数据集
const string test_byte_flann_base = "data//bytetest_base.bvecs";
const string test_byte_flann_query = "data//bytetest_query.bvecs";
const string test_byte_flann_truth = "data//bytetest_groundtruth.ivecs";
```
    (2)测试结果默认是输出到控制台，如果您想将结果保存到文本，请修改main函数中以下部分：
```c++
     //若想将测试结果写入文件，请将下面两行的注释拿掉。
	   //  ofstream fout("result.txt");//将在可执行程序同一目录下生成result.txt
		 // cout.rdbuf(fout.rdbuf());
```
    (3)算法的输入参数有三个：哈希表数量table_number,键值长度key_size(key_size<=64)和查询范围scale
    (例如要查询100近邻，scale=3.0将会产生300个候选点)，请修改以下部分来测试不同的参数组合下，算法的性能
```c++
    //算法将会组合下面三个参数的取值来进行测试
		vector<int>table_numbers = { 10,30,50,80,100,150,200};//哈希表数量取值
		vector<int>key_sizes = { 18,20,25,30,40,50,60};//key_size取值
		vector<float>scales = { 2.0,3.0,4.0,5.0,6.0,7.0,8.0,10.0,20.0};//scale取值
```
myflann的使用接口
---------------------
    myflann实现了整数LSH和浮点LSH算法，使用整数LSH算法您可以参照下面的实例：
```c++
  myflann::LshIndexParams p(table_number,key_size);//指定LshIndexParams,需要提供table_num和key_size参数（key_size<=64）

  //定义读取测试数据的对象
	myflann::DataReader<int> intReader;//整数数据读取对象
	myflann::DataReader<unsigned char> byteReader;//unsigned char 型数据读取对象

	//读取浮点数据测试集，其中test_byte_flann***是相关测试数据集的文件名。
	int base_cnt, query_cnt, truth_cnt;
	myflann::Matrix<unsigned char> base = byteReader.read(test_byte_flann_base, base_cnt);//读取测试数据集
	myflann::Matrix<unsigned char> query = byteReader.read(test_byte_flann_query, query_cnt);//读取查询数据集
	myflann::Matrix<int> truth = intReader.read(test_byte_flann_truth, truth_cnt);//读取事实
	
	//创建创建Index对象，使用L2距离，类型为unsigned char，需要提供测试数据base和IndexParam参数p
	myflann::Index<myflann::L2<unsigned char>> index(base, p);//

  //建立索引结构
	index.buildIndex();
	vector<vector<int>>indices;
	vector<vector<float>> dists;
	//查询参数,scale参数可以修改，后面两个参数请最好不要改动。
	myflann::SearchParam param(scale,500, 20000);
	//开始查询，需要提供查询集query,以及查询多少近邻knn。
	index.knnSearch(query, indices, dists, knn, param);
 //如果上面knn=100那么您可以使用precise函数来计算查询的精度
	vector<float> rates = myflann::precise(truth, indices);
	int i = 0;
	float sum = 0;
	for (i = 0; i < rates.size(); ++i)
	{
		sum += rates[i];
	}
	sum /= rates.size();//sum为100次查询的平均精度
```
    使用浮点型的LSH请参照下面的示例：
```c++
 myflann::LshIndexParams p(table_number,key_size);//指定LshIndexParams,需要提供table_num和key_size参数（key_size<=64）

  //定义读取测试数据的对象
	myflann::DataReader<int> intReader;//浮点数据读取对象
	myflann::DataReader<float> floatReader;//unsigned char 型数据读取对象

	//读取浮点数据测试集，其中test_float_flann***是相关测试数据集的文件名。
	int base_cnt, query_cnt, truth_cnt;
	myflann::Matrix<float> base = floatReader.read(test_float_flann_base, base_cnt);//读取测试数据集
	myflann::Matrix<float> query = floatReader.read(test_float_flann_query, query_cnt);//读取查询数据集
	myflann::Matrix<int> truth = intReader.read(test_float_flann_truth, truth_cnt);//读取事实
	
	//创建创建Index对象，使用L2距离，类型为float，需要提供测试数据base和IndexParam参数p
	myflann::Index<myflann::L2<float>> index(base, p);//

  //建立索引结构
	index.buildIndex();
	vector<vector<int>>indices;
	vector<vector<float>> dists;
	//查询参数,scale参数可以修改，后面两个参数请最好不要改动。
	myflann::SearchParam param(scale,1000, 1000);
	//开始查询，需要提供查询集query,以及查询多少近邻knn。
	index.knnSearch(query, indices, dists, knn, param);
 //如果上面knn=100那么您可以使用precise函数来计算查询的精度
	vector<float> rates = myflann::precise(truth, indices);
	int i = 0;
	float sum = 0;
	for (i = 0; i < rates.size(); ++i)
	{
		sum += rates[i];
	}
	sum /= rates.size();//sum为100次查询的平均精度
```
    您还可以使用Index对象提供的usedMemory接口来查看索引结构占用内存的大小，单位为byte
    除了knnSearch以外，myflann还提供了基于半径的radius搜索函数的参数解释如下：
  ```c++
  		int radiusSearch(const Matrix<ElementType>& queries,//查询集
			vector<vector<int>>& indices,//返回查询到的点的索引
			vector<vector<DistanceType>>& dists,//对于于索引的距离
			float radius, //查询半径
			SearchParam param//查询控制参数，参考上面的knnSearch,其中scale参数未使用
			)
  ```

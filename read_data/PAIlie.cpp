
#include <iostream>
using namespace std;

//交换函数
void swap(int& a, int& b)
{
	int temp = a;
	a = b;
	b = temp;
}
/*入参：
	begin 要排列的起始位置
	end   要排列的结束位置
	a[]	 要排列的数组
*/
void permutate(int begin, int end, int a[])
{
	if (begin == end)//已经到了最后一个位置，进行输出
	{
		for (int i = 0; i < end; i++)
		{
			cout << a[i];
		}
		cout << endl;
	}
	for (int i = begin; i < end; i++)
	{
		swap(a[begin], a[i]);
		permutate(begin + 1, end, a);//递归下一个位置
		swap(a[i], a[begin]);//回溯
	}
}
int main()
{
	int a[5] = { 1,2,3,4,5 };
	permutate(0, 5, a);//对数组中前三个数进行排列
	return 0;
}

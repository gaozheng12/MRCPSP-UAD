
#include <iostream>
using namespace std;

//��������
void swap(int& a, int& b)
{
	int temp = a;
	a = b;
	b = temp;
}
/*��Σ�
	begin Ҫ���е���ʼλ��
	end   Ҫ���еĽ���λ��
	a[]	 Ҫ���е�����
*/
void permutate(int begin, int end, int a[])
{
	if (begin == end)//�Ѿ��������һ��λ�ã��������
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
		permutate(begin + 1, end, a);//�ݹ���һ��λ��
		swap(a[i], a[begin]);//����
	}
}
int main()
{
	int a[5] = { 1,2,3,4,5 };
	permutate(0, 5, a);//��������ǰ��������������
	return 0;
}

#include<sstream>
#include<fstream>
#include<vector>
#include"ACO_head.h"
#define job_num 10
#define mode_num 3
using namespace std;
vector<vector<int>> data(string k1){
ifstream in(k1);
string line;
vector<vector<int>>duration(job_num);
for (int i = 0; i < duration.size(); i++)
{
	duration[i].resize(mode_num);
}
vector<vector<int>>renewable_resource1(job_num);
for (int i = 0; i < renewable_resource1.size(); i++)
{
	renewable_resource1[i].resize(mode_num);
}
vector<vector<int>>renewable_resource2(job_num);
for (int i = 0; i < renewable_resource2.size(); i++)
{
	renewable_resource2[i].resize(mode_num);
}
vector<vector<int>>nonrenewable_resource1(job_num);
for (int i = 0; i < nonrenewable_resource1.size(); i++)
{
	nonrenewable_resource1[i].resize(mode_num);
}
vector<vector<int>>nonrenewable_resource2(job_num);
for (int i = 0; i < nonrenewable_resource2.size(); i++)
{
	nonrenewable_resource2[i].resize(mode_num);
}
vector<vector<int>>successors(job_num);
for (int i = 0; i < successors.size(); i++)
{
	successors[i].resize(job_num);
}
while (getline(in,line))
{
	stringstream g(line);
	g >> renewable_upper1 >> renewable_upper2 >> norenewable_upper1 >> nonrenewable_upper2;
	break;
}
while (getline(in, line))
{
	stringstream g(line);		
	for (int m= 0; m < mode_num;m++)
	{
		for (int j = 0; j < job_num; j++)
		{
			g >> duration[j][m];
			g >> renewable_resource1[j][m];
			g >> renewable_resource2[j][m];
			g >> nonrenewable_resource1[j][m];
			g >> nonrenewable_resource2[j][m];
		}
		break;
	}
	
}
for (int i = 0; i < successors.size(); i++)
{
	while (getline(in, line))
	{
		stringstream g(line);
		for (int j = 0; j < successors[0].size(); j++)
		{
			g >> successors[i][j];
		}break;
	}
}
return successors;
}

#include"iostream"
#include"fstream"
#include"stdio.h"
#include"string"
#include"string.h"
#include"math.h"
#include"time.h"
#include"stdlib.h"
#include "ACO_head.h"
using namespace std;
#define job_num 10
#define mode_num 3
#define ALPHA 0.45																	//信息素重要程度的参数
#define BETA 2																		//启发式因子重要程度的参数
int renewable_upper1, renewable_upper2, norenewable_upper1, nonrenewable_upper2;
vector<vector<int>>successors;
int NY1 = 0, NY2 = 0;
int K = 0, K1 = 0;
int N = 0, N1 = 0, N2 = 0, N3 = 0, N4 = 0, N5 = 0;
int cur_time = 0, best_time = 0, BEST_time = 100, BEST_TIME = 100;
int Time = 100, TIME = 0;
int W = 0, W1 = 0, W2 = 0, M1 = 0, M2 = 0, M3 = 0, M4 = 0, M5 = 0;
int activity_list[job_num];													//保存蚂蚁走的路径
int activity_list_best[job_num];											//最优路径
int mode_list[job_num];														//每个活动所选择的模式
int moved_act = 0;															//已经进行活动数量
double P1 = 0.5;															//局部信息素蒸发率
double P2 = 0.5;															//全局信息素蒸发率
double info[job_num][job_num][3];											//信息素矩阵
double n[job_num][3];														//启发项矩阵
int Q = 1, a = job_num, b = 3, c = 3;
long int M[job_num][3][3] = {}, T[job_num][4] = {}, T_best[job_num][4] = {};
int predecessors[job_num][job_num], degress[job_num + 1];												//各活动的紧前紧后关系，每个活动的直接前序个数
int activity_waited[job_num];													//编码侯选集合
int Activity_waited[job_num];													//解码候选集合
int activity_current[job_num];													//正在进行的活动
int activity_end[job_num];														//已经完成的活动
int Bool[job_num];	
vector<string> z;
void Import()																	//数据的读入
{
	
}
void init_pheromonetable()															//初始化信息素矩阵													
{
	for (int k = 0; k < job_num; k++)
	{
		for (int i = 0; i < a; i++)
		{
			for (int j = 0; j < b; j++) { info[k][i][j] = Q / double(job_num); }
		}
	}
};
void Get_etatable()																	//初始化启发项矩阵
{
	for (int i = 0; i < a; i++)
	{
		for (int j = 0; j < b; j++) { n[i][j] = (M[i][j][0] + M[i][j][1]) / double((R1 + R2)); }
	}
}
void Predev(int a)																	//将个活动的紧前关系与紧后关系分别用数组表示出来																
{
	successors =data(z[a]);
	for (int i = 0; i < job_num; i++)
	{
		for (int j = 0; j < job_num; j++)
		{
			predecessors[j][i] = successors[i][j];
		}
	}
	temp1 = R1;
	temp2 = R2;
}
void Activity_list()																//更新已选任务列表
{
	for (int i = 0; i < job_num; i++)
	{
		mode_list[i] = 0;
		activity_list[i] = 0;
	}
}
void Best_time()																	//计算每个调度方案下的工期
{
	for (int i = 0; i < job_num; i++)
	{
		if (best_time < T[i][3])
		{
			best_time = T[i][3];
		}
	}TIme[I1] = best_time;
};
void activity_T()																	//解码后对参数进行初始化
{
	I = 0;
	R1 = temp1, R2 = temp2, K = 0, K1 = 0;
	N1 = 0, N2 = 0, N3 = 0, N4 = 0;
	M1 = 0, M2 = 0, M3 = 0, M4 = 0, M5 = 0;
	cur_time = 0, Time = 100, TIME = 0;
	W = 0, W1 = 0, W2 = 0;
	for (int i = 0; i < job_num; i++)
	{
		activity_current[i] = 0;
		activity_end[i] = 0;
		Bool[i] = 0;
		for (int j = 0; j < 4; j++)
		{
			T[i][j] = 0;
		}
	}
}
void UpdateInfo_part()																//局部信息素的更新
{
	for (int j = 0; j < b; j++) { info[I][N][M2] = (1 - P1) * info[I][N][M2] + P1 / d[N][M2]; }
}
void UpdateInfo_whole()																//更新全局信息素
{
	for (int k = 0; k < job_num; k++)
	{
		for (int i = 0; i <= job_num; i++)
		{
			for (int j = 0; j < b; j++) { info[k][i][j] = (1 - P2) * info[k][i][j] + P2 / TIme[k]; }
		}
	}
}
void Degress()																		//计算每个活动的直接紧前个数
{
	for (int j = 0; j < job_num; j++)
	{
		int  count = 0;
		for (int i = 0; i < job_num; i++)
		{
			if (predecessors[i][j] == 1)
			{
				count++;
			}
			degress[j + 1] = count;
		}
	}
}
void Predecessors()																	//每个活动的直接后序减一
{
	for (int i = 0; i < job_num; i++)
	{
		if (predecessors[N][i] == 1)
		{
			degress[i + 1]--;
		}
	}
}
void Prob()																			//通过计算概率选择下一个活动
{
	int select_activity = 0;
	int select_activity_mode = 0;
	int k = 0;
	double sum = 0;
	double prob[job_num][job_num][3];
	double Temporary[job_num][job_num][3];
	for (int i = 0; i < job_num; i++)
	{
		if (activity_waited[i] != 0)
		{
			k++;
			select_activity = i;
		}
	}
	if (k == 1)
	{
		N = select_activity;
		do
		{
			M2 = rand() % 3;
		} while (R1 < r1[N][M2] || R2 < r2[N][M2]);
	}
	else
	{
		for (int i = 0; i < job_num; i++)
		{
			for (int j = 0; j < b; j++)
			{
				if (activity_waited[i] != 0)
				{
					Temporary[I][i][j] = pow(info[I][i][j], ALPHA) * pow(n[i][j], BETA);
					sum = sum + Temporary[I][i][j];
				}
				else
				{
					Temporary[I][i][j] = 0;
				}
			}
		}
		for (int i = 0; i < job_num; i++)
		{
			for (int j = 0; j < b; j++)
			{
				if (activity_waited[i] != 0)
				{
					prob[I][i][j] = ((pow(info[I][i][j], double(ALPHA)) * pow(n[i][j], double(BETA))) / sum) * 3;
				}
				else
				{
					prob[I][i][j] = 0;
				}
			}
		}
		double temp = rand() % 300 / double(100);
		for (int i = 0; i < job_num; i++)
		{
			for (int j = 0; j < b; j++)
			{
				if (activity_waited[i] != 0)
				{
					temp = temp - prob[I][i][j];
					if (temp < 0.0)
					{
						select_activity = i;
						N = i;
						do
						{
							M2 = rand() % 3;
						} while (R1 < r1[N][M2] || R2 < r2[N][M2]);
						select_activity_mode = 1;
						break;
					}
				}
			}
			if (select_activity_mode != 0)
			{
				break;
			}
		}
	}select_activity = 0, select_activity_mode = 0;
};
void Choose_activity()																			//选择出一串任务列表
{
	Degress();
	for (int k = 0; k < job_num; k++)
	{
		for (int j = 0; j < job_num; j++)
		{
			if (degress[j + 1] == 0)
			{
				activity_waited[j] = j + 1;
				degress[j + 1]--;
			}
		}
		Prob();
		UpdateInfo_part();
		Predecessors();
		activity_list[moved_act] = N + 1;
		mode_list[moved_act] = M2 + 1;
		NY1 = NY1 + n1[N][M2];
		NY2 = NY2 + n2[N][M2];
		moved_act++;
		activity_waited[N] = 0;
	}
	Degress();
}
void KONG()																						//解码时可选任务集合为空
{
	for (int i = 0; i < job_num; i++)
	{
		if (activity_current[i] != 0)
		{
			W1 = i + 1;
			for (int i = 0; i < job_num; i++)
			{
				if (T[i][0] == W1)
				{
					W2 = i;
				}
			}
			TIME = T[W2][3];
			if (TIME < Time)
			{
				Time = TIME;
				cur_time = Time;
				W = T[W2][0] - 1;
				for (int j = 0; j < job_num; j++)
				{
					if (W + 1 == activity_list[j])
					{
						M5 = mode_list[j] - 1;
					}
				}
			}
		}
	}
	R1 += r1[W][M5];
	R2 += r2[W][M5];
	activity_current[W] = 0;
	Time = 100;
	for (int i = 0; i < moved_act; i++)
	{
		if (cur_time == T[i][3])
		{
			activity_end[W] = W + 1;
			N2 = W + 1;
			K1++;
		}
	}
	for (int i = 0; i < job_num; i++)
	{
		if (predecessors[N2 - 1][i] == 1 && Bool[N2 - 1] == 0 && K1 >= 1)
		{
			degress[i + 1]--;
		}
	}Bool[N2 - 1] = 1;
	K1 = 0;
	N2 = 0;
	for (int j = 0; j < job_num; j++)
	{
		N1 = activity_list[j];
		if (degress[N1] == 0)
		{
			Activity_waited[N1 - 1] = N1;
			degress[N1]--;
		}
	}
	for (int i = 0; i < job_num; i++)
	{
		if (Activity_waited[i] != 0)
		{
			do
			{
				int N5 = rand() % job_num;
				N3 = Activity_waited[N5];
				for (int j = 0; j < job_num; j++)
				{
					if (N3 == activity_list[j])
					{
						M3 = mode_list[j];
					}
				}
			} while (N3 == 0);
			N4++;
			break;
		}
	}
}
void Decode()																			//计算每个活动及其开始时间结束时间
{
	for (int i = 0; i < job_num; i++)
	{
		if (Activity_waited[i] != 0)
		{
			do
			{
				N5 = rand() % job_num;
				N3 = Activity_waited[N5];
				for (int j = 0; j < job_num; j++)
				{
					if (N3 == activity_list[j])
					{
						M3 = mode_list[j];
					}
				}
			} while (N3 == 0);
			N4++;
			break;
		}
	}
	if (N4 == 0)
	{
		KONG();
	}
	if (R1 >= r1[N3 - 1][M3 - 1] && R2 >= r2[N3 - 1][M3 - 1] && N4 != 0)
	{
		T[moved_act][0] = N3;
		T[moved_act][1] = M3;
		T[moved_act][2] = cur_time;
		T[moved_act][3] = cur_time + d[N3 - 1][M3 - 1];
		cur_time = T[moved_act][2];
		R1 -= r1[N3 - 1][M3 - 1];
		R2 -= r2[N3 - 1][M3 - 1];
		moved_act++;
		activity_current[N3 - 1] = N3;
		Activity_waited[N3 - 1] = 0;
	}
	else
	{
		for (int i = 0; i < job_num; i++)
		{
			if (activity_current[i] != 0)
			{
				W1 = i + 1;
				for (int i = 0; i < job_num; i++)
				{
					if (T[i][0] == W1)
					{
						W2 = i;
					}
				}
				TIME = T[W2][3];
				if (TIME < Time)
				{
					Time = TIME;
					cur_time = Time;
					W = T[W2][0] - 1;
					for (int j = 0; j < job_num; j++)
					{
						if (W + 1 == activity_list[j])
						{
							M4 = mode_list[j] - 1;
						}
					}
				}
			}
		}
		R1 += r1[W][M4];
		R2 += r2[W][M4];
		activity_current[W] = 0;
		Time = 100;
		for (int i = 0; i < moved_act; i++)
		{
			if (cur_time == T[i][3])
			{
				activity_end[W] = W + 1;
				N2 = W + 1;
				K1++;
			}
		}
		for (int j = 0; j < job_num; j++)
		{
			if (activity_end[j] != 0)
			{
				activity_current[j] = 0;
			}
		}
		for (int i = 0; i < job_num; i++)
		{
			if (predecessors[N2 - 1][i] == 1 && Bool[N2 - 1] == 0 && K1 >= 1)
			{
				degress[i + 1]--;
			}
		}
		Bool[N2 - 1] = 1;
		K1 = 0;
		N2 = 0;
	}N4 = 0;
}
void decode()																						//解码
{
	Degress();
	for (int k = 0; k < 3; k++)
	{
		for (int i = 0; i < job_num; i++)
		{
			for (int j = 0; j < job_num; j++)
			{
				N1 = activity_list[j];
				if (degress[N1] == 0)
				{
					Activity_waited[N1 - 1] = N1;
					degress[N1]--;
				}
			}
			Decode();
		}
	}
}
int main()
{	
	int time_start, time_end;
	time_start = clock();
	srand((unsigned)time(NULL));
	ofstream outfile("D:/file3_J30_50.txt");
    z.push_back("D:/data.txt");
	//z.push_back("D:/data1.txt");
	/*z.push_back("D:/data2.txt");
	z.push_back("D:/data3.txt");
	z.push_back("D:/data4.txt");
	z.push_back("D:/data5.txt");
	z.push_back("D:/data6.txt");
	z.push_back("D:/data7.txt");
	z.push_back("D:/data8.txt");
	z.push_back("D:/data9.txt");
	z.push_back("D:/data10.txt");
	z.push_back("D:/data11.txt");
	z.push_back("D:/data12.txt");
	z.push_back("D:/data13.txt");
	z.push_back("D:/data14.txt");
	z.push_back("D:/data15.txt");
	z.push_back("D:/data16.txt");
	z.push_back("D:/data17.txt");
	z.push_back("D:/data18.txt");
	z.push_back("D:/data19.txt");
	z.push_back("D:/data20.txt");
	z.push_back("D:/data21.txt");
	z.push_back("D:/data22.txt");
	z.push_back("D:/data23.txt");*/
	//z.push_back("D:/data24.txt");
	/*z.push_back("D:/data25.txt");
	z.push_back("D:/data26.txt");
	z.push_back("D:/data27.txt");
	z.push_back("D:/data28.txt");
	z.push_back("D:/data29.txt");
	z.push_back("D:/data30.txt");
	z.push_back("D:/data31.txt");
	z.push_back("D:/data32.txt");
	z.push_back("D:/data33.txt");
	z.push_back("D:/data34.txt");
	z.push_back("D:/data35.txt");
	z.push_back("D:/data36.txt");
	z.push_back("D:/data37.txt");
	z.push_back("D:/data38.txt");
	z.push_back("D:/data39.txt");
	z.push_back("D:/data40.txt");
	z.push_back("D:/data41.txt");
	z.push_back("D:/data42.txt");
	z.push_back("D:/data43.txt");
	z.push_back("D:/data44.txt");
	z.push_back("D:/data45.txt");
	z.push_back("D:/data46.txt");
	z.push_back("D:/data47.txt");
	z.push_back("D:/data48.txt");
	z.push_back("D:/data49.txt");*/
	//z.push_back("D:/data50.txt");
	for (int j = 0; j< z.size(); j++) {
		Predev(j);
		Import();
		init_pheromonetable();
		Get_etatable();		
		for (int i = 0; i < TMAC; i++)
		{
			for (int j = 0; j < ant_num; j++)
			{
				Choose_activity();
				
				N = 0;
				moved_act = 0;
				I++;
				decode();
				N1 = 0;
				moved_act = 0;
				Best_time();
				if (best_time < BEST_time && NY1 <= NN1 && NY2 <= NN2)
				{
					BEST_time = best_time;
					for (int j = 0; j < job_num; j++)
					{
						for (int k = 0; k < 4; k++)
						{
							T_best[j][k] = T[j][k];
						}
					}
				}

				NY1 = 0, NY2 = 0;
				Activity_list();
				activity_T();
				best_time = 0;
			}cout <<BEST_time<<endl;
			if (BEST_time < BEST_TIME)
			{
				BEST_TIME = BEST_time;
			}I1++;
		}UpdateInfo_whole();
		I = 0, I1 = 0;
		BEST_time = 100;
		//cout << BEST_TIME << endl;
		BEST_TIME = 100;
		for (int j = 0; j < job_num; j++)
		{
			cout << ("(");
			for (int k = 0; k < 4; k++)
			{
				cout << T_best[j][k];
				if (k < 3)
				{
					cout << ",";
				}
			}
			cout << ")";
			if (j < job_num-1)
			{
				cout << "-<";
			}
		}
		cout << endl;
		cout<< endl;
	}
	time_end = clock();
	cout << "\n" << time_end - time_start << endl;	
	system("pause");
	return 0;
}
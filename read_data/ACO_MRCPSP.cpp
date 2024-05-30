#include"iostream"
#include"stdio.h"
#include"string"
#include"string.h"
#include"math.h"
#include"time.h"
#include"stdlib.h"
using namespace std;
#define activity_num 10																//�����10
#define ant_num 10																	//��������10
#define mode 3																			//ģʽΪ3
#define TMAC 1000																	//����������
#define ALPHA 0.5																		//��Ϣ����Ҫ�̶ȵĲ���
#define BETA 0.8																			//����ʽ������Ҫ�̶ȵĲ���
int R1 = 9, R2 = 4;
int NY1 = 0, NY2 = 0;
int I = 0, I1 = 0;
int K = 0, K1 = 0;
int N = 0, N1 = 0, N2 = 0, N3 = 0, N4 = 0, N5 = 0;
int cur_time = 0, best_time = 0, BEST_time = 100, BEST_TIME = 100;
int Time = 100, TIME = 0;
int W = 0, W1 = 0, W2 = 0, M1 = 0, M2 = 0, M3 = 0, M4 = 0, M5 = 0;
int activity_list[activity_num];															//���������ߵ�·��
int TIme[TMAC];																		//ÿ�ε���������·���Ĺ���
int activity_list_best[activity_num];													//����·��
int mode_list[activity_num];																//ÿ�����ѡ���ģʽ
int moved_act = 0;																			//�Ѿ����л����
double P1 = 0.5;																				//�ֲ���Ϣ��������
double P2 = 0.5;																				//ȫ����Ϣ��������
double info[activity_num][activity_num][3];																	//��Ϣ�ؾ���
double n[activity_num][3];																				//���������
int Q = 1, a = activity_num, b = 3, c = 3;
int M[activity_num][3][3] = {}, T[activity_num][4] = {}, T_best[activity_num][4] = {};
int predecessors[activity_num][activity_num], degress[activity_num + 1];												//����Ľ�ǰ�����ϵ��ÿ�����ֱ��ǰ�����
int d[10][3] = { { 3, 9, 10 }, { 1, 1, 5 }, { 3, 5, 8 }, { 4, 6, 10 }, { 2, 4, 6 }, { 3, 6, 8 }, { 4, 10, 10 }, { 2, 7, 10 }, { 1, 1, 9 }, { 6, 9, 10 } };
int r1[10][3] = { { 6, 5, 0 }, { 0, 7, 0 }, { 10, 7, 6 }, { 0, 2, 0 }, { 2, 0, 2 }, {5, 0, 5 }, { 6, 3, 4 }, { 2, 1, 1 }, { 4, 0, 4 }, { 0, 0, 0 } };
int r2[10][3] = { { 0, 0, 6 }, { 4, 0, 4 }, { 0, 0, 0 }, { 9, 0, 5 }, { 0, 8, 0 }, { 0, 7, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 2, 0 }, { 2, 1, 1 } };
int n1[10][3] = { { 9, 0, 0 }, { 0, 0, 0 }, { 0, 2, 0 }, { 8, 0, 0 }, { 8, 5, 0 }, { 10, 10, 0 }, { 0, 10, 0 }, { 6, 0, 0 }, { 4, 0, 0 }, { 0, 0, 0 } };
int n2[10][3] = { { 0, 8, 6 }, { 8, 8, 5 }, { 7, 0, 7 }, { 0, 7, 5 }, { 0, 0, 1 }, { 0, 0, 10 }, { 1, 0, 1 }, { 0, 8, 7 }, { 0, 8, 5 }, { 10, 9, 7 } };
int activity_waited[activity_num];														//�����ѡ����
int Activity_waited[activity_num];														//�����ѡ����
int activity_current[activity_num];													//���ڽ��еĻ
int activity_end[activity_num];															//�Ѿ���ɵĻ
int Bool[activity_num];																		//���Ѿ���ɵĻ���б��
void Import()																					//���ݵĶ���
{
	for (int i = 0; i <= a; i++)
	{
		for (int j = 0; j <= b; j++) { M[i][j][0] = r1[i][j], M[i][j][1] = r2[i][j], M[i][j][2] = d[i][j]; }
	}
};
void init_pheromonetable()																//��ʼ����Ϣ�ؾ���													
{
	for (int k = 0; k < activity_num; k++)
	{
		for (int i = 0; i < a; i++)
		{
			for (int j = 0; j < b; j++) { info[k][i][j] = Q / double(activity_num); }
		}
	}
};
void Get_etatable()																			//��ʼ�����������
{
	for (int i = 0; i < a; i++)
	{
		for (int j = 0; j < b; j++) { n[i][j] = (M[i][j][0] + M[i][j][1]) / double((R1 + R2)); }
	}
}
void Predev()																					//������Ľ�ǰ��ϵ������ϵ�ֱ��������ʾ����																
{
	int  successors[activity_num][activity_num] =
	{
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{ 0, 0, 0, 1, 1, 0, 0, 0, 0, 0},
		{ 0, 0, 0, 1, 0, 0, 0, 0, 0, 0},
		{ 0, 0, 1, 0, 0, 1, 1, 0, 0, 0},
		{ 0, 1, 0, 0, 1, 1, 0, 0, 0, 0},
		{ 0, 1, 0, 0, 1, 0, 0, 0, 0, 0},
	};
	for (int i = 0; i < activity_num; i++)
	{
		for (int j = 0; j < activity_num; j++)
		{
			predecessors[j][i] = successors[i][j];
		}
	}
}
void Activity_list()																		//������ѡ�����б�
{
	for (int i = 0; i < activity_num; i++)
	{
		mode_list[i] = 0;
		activity_list[i] = 0;
	}
}
void Best_time()																		//����ÿ�����ȷ����µĹ���
{
	for (int i = 0; i < activity_num; i++)
	{
		if (best_time < T[i][3])
		{
			best_time = T[i][3];
		}
	}TIme[I1] = best_time;
};
void activity_T()																		//�����Բ������г�ʼ��
{
	I = 0;
	R1 = 9, R2 = 4, K = 0, K1 = 0;
	N1 = 0, N2 = 0, N3 = 0, N4 = 0;
	M1 = 0, M2 = 0, M3 = 0, M4 = 0, M5 = 0;
	cur_time = 0, Time = 100, TIME = 0;
	W = 0, W1 = 0, W2 = 0;
	for (int i = 0; i < activity_num; i++)
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
void UpdateInfo_part()															//�ֲ���Ϣ�صĸ���
{
	for (int j = 0; j < b; j++) { info[I][N][M2] = (1 - P1) * info[I][N][M2] + P1 / d[N][M2]; }
}
void UpdateInfo_whole()															//����ȫ����Ϣ��
{
	for (int k = 0; k < activity_num; k++)
	{
		for (int i = 0; i <= activity_num; i++)
		{
			for (int j = 0; j < b; j++) { info[k][i][j] = (1 - P2) * info[k][i][j] + P2 / TIme[k]; }
		}
	}
}
void Degress()																			//����ÿ�����ֱ�ӽ�ǰ����
{
	for (int j = 0; j < activity_num; j++)
	{
		int  count = 0;
		for (int i = 0; i < activity_num; i++)
		{
			if (predecessors[i][j] == 1)
			{
				count++;
			}
			degress[j + 1] = count;
		}
	}
}
void Predecessors()																	//ÿ�����ֱ�Ӻ����һ
{
	for (int i = 0; i < activity_num; i++)
	{
		if (predecessors[N][i] == 1)
		{
			degress[i + 1]--;
		}
	}
}
void Prob()																				//ͨ���������ѡ����һ���
{
	int select_activity = 0;
	int select_activity_mode = 0;
	int k = 0;
	double sum = 0;
	double prob[activity_num][activity_num][3];
	double Temporary[activity_num][activity_num][3];
	for (int i = 0; i < activity_num; i++)
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
		for (int i = 0; i < activity_num; i++)
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
		for (int i = 0; i < activity_num; i++)
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
		for (int i = 0; i < activity_num; i++)
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
void Choose_activity()																			//ѡ���һ�������б�
{
	Degress();
	for (int k = 0; k < activity_num; k++)
	{
		for (int j = 0; j < activity_num; j++)
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
void KONG()																						//����ʱ��ѡ���񼯺�Ϊ��
{
	for (int i = 0; i < activity_num; i++)
	{
		if (activity_current[i] != 0)
		{
			W1 = i + 1;
			for (int i = 0; i < activity_num; i++)
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
				for (int j = 0; j < activity_num; j++)
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
	for (int i = 0; i < activity_num; i++)
	{
		if (predecessors[N2 - 1][i] == 1 && Bool[N2 - 1] == 0 && K1 >= 1)
		{
			degress[i + 1]--;
		}
	}Bool[N2 - 1] = 1;
	K1 = 0;
	N2 = 0;
	for (int j = 0; j < activity_num; j++)
	{
		N1 = activity_list[j];
		if (degress[N1] == 0)
		{
			Activity_waited[N1 - 1] = N1;
			degress[N1]--;
		}
	}
	for (int i = 0; i < activity_num; i++)
	{
		if (Activity_waited[i] != 0)
		{
			do
			{
				int N5 = rand() % 10;
				N3 = Activity_waited[N5];
				for (int j = 0; j < activity_num; j++)
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
void Decode()																			//����ÿ������俪ʼʱ�����ʱ��
{
	for (int i = 0; i < activity_num; i++)
	{
		if (Activity_waited[i] != 0)
		{
			do
			{
				N5 = rand() % 10;
				N3 = Activity_waited[N5];
				for (int j = 0; j < activity_num; j++)
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
		for (int i = 0; i < activity_num; i++)
		{
			if (activity_current[i] != 0)
			{
				W1 = i + 1;
				for (int i = 0; i < activity_num; i++)
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
					for (int j = 0; j < activity_num; j++)
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
		for (int j = 0; j < activity_num; j++)
		{
			if (activity_end[j] != 0)
			{
				activity_current[j] = 0;
			}
		}
		for (int i = 0; i < activity_num; i++)
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
void decode()																						//����
{
	Degress();
	for (int k = 0; k < 3; k++)
	{
		for (int i = 0; i < activity_num; i++)
		{
			for (int j = 0; j < activity_num; j++)
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
	Import();
	Predev();
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
			if (best_time < BEST_time && NY1 <= 29 && NY2 <= 40)
			{
				BEST_time = best_time;
				for (int j = 0; j < 10; j++)
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
		}
		if (BEST_time < BEST_TIME)
		{
			BEST_TIME = BEST_time;
		}I1++;
	}cout << BEST_TIME << endl;
	for (int j = 0; j < 10; j++)
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
		if (j < 9)
		{
			cout << "-<";
		}
	}
	time_end = clock();
	cout << "\n" << time_end - time_start << endl;
	system("pause");
	return 0;
}
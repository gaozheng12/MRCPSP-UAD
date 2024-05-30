#include<iostream>
#include<fstream>
#include<stdio.h>
#include <string>
#include <string.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <vector>
#include <sstream>
#include <fstream>
#include <chrono>
#include <algorithm>
#include <map>
#include <set>
#include <random>
#include <unordered_set>
#include <ilcplex/ilocplex.h>
using namespace std;
typedef IloArray<IloNumVarArray> NumVarMatrix;
typedef IloArray<NumVarMatrix> NumVar3Matrix;
#define job_num 12
#define initial_num 1
#define mode_num 3
#define POPSIZE 50
const int resource_num = 2;
int selected_activity = 2;
int limit_time = 0; double gap = 0;
int RCDM_counter = 0; double RCDM_average_time = 0, RCDM_average_gap = 0;
int Maximun_computational_time[10] = { 0.1 * (job_num - 2),0.1 * (job_num - 2),0.1 * (job_num - 2),0.1 * (job_num - 2),0.1 * (job_num - 2),0.1 * (job_num - 2),0.1 * (job_num - 2),0.1 * (job_num - 2),0.1 * (job_num - 2),0.1 * (job_num - 2) };
vector<vector<int>>Direct_precedence_relationship(job_num, vector<int>(job_num));
vector<vector<int>>precedence_relationship(job_num, vector<int>(job_num));
vector<vector<double>>duration(job_num, vector<double>(mode_num));
vector<vector<int>>renewable_resource1(job_num, vector<int>(mode_num));
vector<vector<int>>renewable_resource2(job_num, vector<int>(mode_num));
vector<vector<int>>nonrenewable_resource1(job_num, vector<int>(mode_num));
vector<vector<int>>nonrenewable_resource2(job_num, vector<int>(mode_num));
vector<vector<int>> newPopulation(POPSIZE);
int renewable_upperenewable_resource[resource_num] = {}, norenewable_upperenewable_resource[resource_num] = {};
int renewable_upperenewable_temporary[resource_num] = {}, norenewable_upperenewable_consumption1, norenewable_upperenewable_consumption2;
int job_one, mode_one; int job_one1, mode_one1; int job_one2, mode_one2;
int job_one3, mode_one3; int job_one4, mode_one4; int job_one5, mode_one5;
int feasible_location = 0, moved_act = 0; int solution = 0;
double current_time = 0, current_time1 = INT_MAX, current_time2 = 0, best_time = 0, BEST_time = INT_MAX, BEST_TIME = INT_MAX;
int wait_job = 0, wait_job1 = 0, wait_job2 = 0;
int successors_count[job_num + 1] = {};															//每个活动的直接前序个数
int candidate_set[job_num] = {};																//侯选集合
int job_list[job_num] = {}, mode_list[job_num] = {};
int JOB_list[initial_num][job_num] = {}, MODE_list[initial_num][job_num] = {};					//编码生成的可行解
int Population_job[POPSIZE][job_num] = {}; int Population_mode[POPSIZE][job_num] = {};
int job_current[job_num] = {};																	//正在进行的活动
int job_end[job_num] = {};																		//已经完成的活动
int job_complete[job_num];	int job_complete1 = 0;												//判断活动是否已经完成
double scheduling_plan[job_num][4] = {}; double the_best_scheduling_plan[job_num][4] = {}; vector<double>makespan(initial_num); int the_best_scheduling_solution[job_num][4] = {};
const double CROSSOVER_PROB = 0.0; const double MUTATION_PROB = 0.0;							//交叉概率和变异概率
double uncertain_level = 0.10, degree_permissilbe_violation = 0.15;								//随机变量的波动程度和约束条件的允许违背程度
int minimizing_total_mode_list[job_num] = {};
int a_decode_job_list[job_num] = {}, a_decode_mode_list[job_num] = {};
int A_decode_job_list[job_num] = {}, A_decode_mode_list[job_num] = {};
vector<vector<bool>>M(job_num, vector<bool>(job_num)); vector<int> priority_order(job_num);
double ES[job_num] = {}; double LS[job_num] = {}; int TE[job_num][job_num] = {}; double duration_max[job_num] = {};
vector<vector<int>>temporary_resource_assignment1(job_num); vector<vector<int>>temporary_resource_assignment2(job_num);
int temporary_sequence_variable[job_num][job_num] = {};	int temporary_resource_overlap_variables[job_num][job_num] = {}; double temporary_mode_selection[job_num][mode_num] = {};
vector<int>resource_first; vector<int>resource_second; vector<vector<int>>the_resource_first(job_num); vector<vector<int>>the_resource_second(job_num); vector<vector<int>>the_best_resource_first(job_num); vector<vector<int>>the_best_resource_second(job_num);
int activity_float[job_num] = {}; double start_time[job_num] = {};
// 生成随机数的引擎和分布
default_random_engine rng;
uniform_real_distribution<double> dist(0.0, 1.0);
//更新每个活动的直接后序个数
void update_successors()
{
	for (int i = 0; i < job_num; i++)
	{
		if (Direct_precedence_relationship[i][job_one] == 1)
		{
			successors_count[i + 1]--;
		}
	}
}
//初始化,产生一个解
void encode()
{
	int successoes_count[job_num + 1] = {}, candidate_set[job_num] = {};
	int temporary_norenewable1 = 0, temporary_norenewable2 = 0;
	int preorder_gather[job_num][50] = {}; int epilogue_gather[job_num][50] = {}; int ES_C[job_num] = {}, EF_C[job_num] = {}; int LS_C[job_num] = {}; int LF_C[job_num] = {};
	for (int i = 0; i < job_num; i++)
	{
		int count = 0;
		for (int j = 0; j < job_num; j++)
		{
			if (Direct_precedence_relationship[j][i] == 1)
			{
				preorder_gather[i][count] = j + 1;
				count++;
			}
		}
	}
	for (int i = 0; i < job_num; i++)
	{
		int count1 = 0;
		for (int j = 0; j < job_num; j++)
		{
			if (Direct_precedence_relationship[i][j] == 1)
			{
				epilogue_gather[i][count1] = j + 1;
				count1++;
			}
		}
	}
	auto current_time = 0, temprary_job = 0;
	for (int i = 0; i < job_num; i++)
	{
		for (int j = 0; j < 50; j++)
		{
			if (epilogue_gather[i][j] != 0)
			{
				temprary_job = epilogue_gather[i][j];
				if (temprary_job - 1 >= 0) { if (EF_C[temprary_job - 1] > current_time) { current_time = EF_C[temprary_job - 1]; } }
			}
		}
		ES_C[i] = current_time;
		EF_C[i] = current_time + duration[i][minimizing_total_mode_list[i] - 1.0];
		current_time = 0;
	}
	while (solution < initial_num)
	{
		for (int j = 0; j < job_num; j++)
		{
			int  count = 0;
			for (int i = 0; i < job_num; i++)
			{
				if (Direct_precedence_relationship[j][i] == 1)
				{
					count++;
				}
				successors_count[j + 1] = count;
			}
		}
		for (int k = 0; k < job_num; k++)
		{
			for (int j = 0; j < job_num; j++)
			{
				if (successors_count[j + 1] == 0)
				{
					candidate_set[j] = j + 1;
					successors_count[j + 1]--;
				}
			}
			do
			{
				int a = rand() % job_num;
				job_one = candidate_set[a];
			} while (job_one == 0);
			job_one = job_one - 1;
			update_successors();
			job_list[moved_act] = job_one + 1;
			moved_act++;
			candidate_set[job_one] = 0;
		}
		moved_act = 0;
		for (int i = 0; i < job_num; i++)
		{
			JOB_list[solution][i] = job_list[i];
			MODE_list[solution][i] = minimizing_total_mode_list[job_list[i] - 1];
		}
		solution++;
	}solution = 0;
}
//解码时可选任务集合为空集
void empty_set()
{
	for (int i = 0; i < job_num; i++)
	{
		if (job_current[i] != 0)
		{
			wait_job1 = i + 1;
			for (int i = 0; i < job_num; i++)
			{
				if (scheduling_plan[i][0] == wait_job1)
				{
					wait_job2 = i;
					break;
				}
			}
			current_time2 = scheduling_plan[wait_job2][3];
			if (current_time2 <= current_time1)
			{
				current_time1 = current_time2;
				current_time = current_time1;
				wait_job = scheduling_plan[wait_job2][0] - 1;
				for (int j = 0; j < job_num; j++)
				{
					if (wait_job + 1 == A_decode_job_list[j])
					{
						mode_one5 = A_decode_mode_list[j] - 1;
						break;
					}
				}
			}
		}
	}
	job_current[wait_job] = 0;
	current_time1 = INT_MAX;
	renewable_upperenewable_resource[0] += renewable_resource1[wait_job][mode_one5];
	renewable_upperenewable_resource[1] += renewable_resource2[wait_job][mode_one5];
	for (int i = 0; i < moved_act; i++)
	{
		if (current_time == scheduling_plan[i][3])
		{
			job_end[wait_job] = wait_job + 1;
			job_one2 = wait_job + 1;
			job_complete1++;
			break;
		}
	}
	for (int i = 0; i < job_num; i++)
	{
		if (Direct_precedence_relationship[i][job_one2 - 1] == 1 && job_complete[job_one2 - 1] == 0 && job_complete1 >= 1)
		{
			successors_count[i + 1]--;
		}
	}
	job_complete[job_one2 - 1] = 1;
	job_complete1 = 0;
	job_one2 = 0;
	for (int j = 0; j < job_num; j++)
	{
		job_one1 = A_decode_job_list[j];
		if (successors_count[job_one1] == 0)
		{
			candidate_set[job_one1 - 1] = job_one1;
			successors_count[job_one1]--;
		}
	}
	int count1 = 0;
	for (int i = 0; i < job_num; i++)
	{
		if (a_decode_job_list[i] != 0)
		{
			job_one5 = a_decode_job_list[i];
			mode_one5 = a_decode_mode_list[i];
			count1++;
		}
		for (int j = 0; j < job_num; j++)
		{
			if (job_one5 == candidate_set[j])
			{
				job_one3 = job_one5;
				mode_one3 = mode_one5;
				job_one4++;
				break;
			}
		}
		if (count1 > 0)
		{
			break;
		}
	}
	job_one5 = -1;
	mode_one5 = -1;
	count1 = 0;
}
//计算每个活动开始时间和结束时间
void Decode()
{
	int count1 = 0;
	for (int i = 0; i < job_num; i++)
	{
		if (a_decode_job_list[i] != 0) {
			job_one5 = a_decode_job_list[i];
			mode_one5 = a_decode_mode_list[i];
			count1++;
		}
		for (int j = 0; j < job_num; j++)
		{
			if (job_one5 == candidate_set[j])
			{
				job_one3 = job_one5;
				mode_one3 = mode_one5;
				job_one4++;
				break;
			}
		}
		if (job_one4 > 0) { break; }
		if (count1 > 0) { break; }
	}
	count1 = 0;
	if (job_one4 == 0)
	{
		empty_set();
	}
	if (renewable_upperenewable_resource[0] >= renewable_resource1[job_one3 - 1][mode_one3 - 1] && renewable_upperenewable_resource[1] >= renewable_resource2[job_one3 - 1][mode_one3 - 1] && job_one4 != 0)
	{
		scheduling_plan[moved_act][0] = double(job_one3);
		scheduling_plan[moved_act][1] = double(mode_one3);
		scheduling_plan[moved_act][2] = current_time;
		scheduling_plan[moved_act][3] = current_time + duration[job_one3 - 1][mode_one3 - 1];
		current_time = scheduling_plan[moved_act][2];
		for (int i = 0; i < job_num; i++)
		{
			if (job_one3 == a_decode_job_list[i])
			{
				a_decode_job_list[i] = 0;
				break;
			}
		}
		renewable_upperenewable_resource[0] -= renewable_resource1[job_one3 - 1][mode_one3 - 1];
		renewable_upperenewable_resource[1] -= renewable_resource2[job_one3 - 1][mode_one3 - 1];
		moved_act++;
		job_current[job_one3 - 1] = job_one3;
		candidate_set[job_one3 - 1] = 0;
	}
	else
	{
		for (int i = 0; i < job_num; i++)
		{
			if (job_current[i] != 0)
			{
				wait_job1 = i + 1;
				for (int i = 0; i < job_num; i++)
				{
					if (scheduling_plan[i][0] == wait_job1)
					{
						wait_job2 = i;
						break;
					}
				}
				current_time2 = scheduling_plan[wait_job2][3];
				if (current_time2 <= current_time1)
				{
					current_time1 = current_time2;
					current_time = current_time1;
					wait_job = scheduling_plan[wait_job2][0] - 1;
					for (int j = 0; j < job_num; j++)
					{
						if (wait_job + 1 == A_decode_job_list[j])
						{
							mode_one4 = A_decode_mode_list[j] - 1;
							break;
						}
					}
				}
			}
		}
		job_current[wait_job] = 0;
		current_time1 = INT_MAX;
		renewable_upperenewable_resource[0] += renewable_resource1[wait_job][mode_one4];
		renewable_upperenewable_resource[1] += renewable_resource2[wait_job][mode_one4];
		for (int i = 0; i < moved_act; i++)
		{
			if (current_time == scheduling_plan[i][3])
			{
				job_end[wait_job] = wait_job + 1;
				job_one2 = wait_job + 1;
				job_complete1++;
			}
		}
		for (int j = 0; j < job_num; j++)
		{
			if (job_end[j] != 0)
			{
				job_current[j] = 0;
			}
		}
		for (int i = 0; i < job_num; i++)
		{
			if (Direct_precedence_relationship[i][job_one2 - 1] == 1 && job_complete[job_one2 - 1] == 0 && job_complete1 >= 1)
			{
				successors_count[i + 1]--;
			}
		}
		job_complete[job_one2 - 1] = 1;
		job_complete1 = 0;
		job_one2 = 0;
	}job_one4 = 0;
}
//并行调度解码机制
void decode()
{
	for (int j = 0; j < job_num; j++)
	{
		int  count = 0;
		for (int i = 0; i < job_num; i++)
		{
			if (Direct_precedence_relationship[j][i] == 1)
			{
				count++;
			}
			successors_count[j + 1] = count;
		}
	}
	for (int i = 0; i < job_num; i++)
	{
		a_decode_job_list[i] = A_decode_job_list[i];
		a_decode_mode_list[i] = A_decode_mode_list[i];
	}
	for (int k = 0; k < 3; k++)
	{
		for (int i = 0; i < job_num; i++)
		{
			for (int j = 0; j < job_num; j++)
			{
				job_one1 = A_decode_job_list[j];
				if (successors_count[job_one1] == 0)
				{
					candidate_set[job_one1 - 1] = job_one1;
					successors_count[job_one1]--;
					break;
				}
			}
			Decode();
			auto count2 = 0;
			for (int i = 0; i < job_num; i++)
			{
				if (job_end[i] != 0)
				{
					count2++;
				}
			}
			if (count2 == job_num)
			{
				break;
			}
		}		
	}
}
//解码后参数更新
void parameter_update()
{
	moved_act = 0;
	renewable_upperenewable_resource[0] = renewable_upperenewable_temporary[0], renewable_upperenewable_resource[1] = renewable_upperenewable_temporary[1],
		job_one1 = 0, mode_one1 = 0;
	job_one2 = 0, mode_one2 = 0;
	job_one3 = 0, mode_one3 = 0;
	job_one4 = 0, mode_one4 = 0;
	job_one5 = 0, mode_one5 = 0;
	current_time = 0, current_time1 = INT_MAX, current_time2 = 0; job_complete1 = 0;
	wait_job = 0, wait_job1 = 0, wait_job2 = 0;
	for (int i = 0; i < job_num; i++)
	{
		job_current[i] = 0;
		job_end[i] = 0;	job_complete[i] = 0;
		for (int j = 0; j < 4; j++)
		{
			scheduling_plan[i][j] = 0.0;
		}
	}
}
//计算每个调度方案下的工期
void Best_time()
{
	for (int i = 0; i < job_num; i++)
	{
		if (best_time < scheduling_plan[i][3])
		{
			best_time = scheduling_plan[i][3];
		}
	}makespan[feasible_location] = best_time;
}
void Best_time_one()
{
	for (int i = 0; i < job_num; i++)
	{
		if (best_time < scheduling_plan[i][3])
		{
			best_time = scheduling_plan[i][3];
		}
	}
}
// 根据矩阵 M 修复个体中的优先关系序列
void fix_priority_order(std::vector<int>& priority_order, const std::vector<std::vector<bool>>& M) {
	int n = priority_order.size(); bool changed = true;
	while (changed)
	{
		changed = false;
		for (int i = 0; i < n; i++)
		{
			for (int j = i + 1; j < n; j++)
			{
				if (M[priority_order[j]][priority_order[i]])
				{
					std::swap(priority_order[i], priority_order[j]);
					changed = true;
				}
			}
		}
	}
}
//增强型插入邻域
void Ehence_insert_neihborhood()
{
	int exchange_list[job_num] = {};
	int job_list_temprary[job_num] = {}, mode_list_temprary[job_num] = {};
	int temprary_job_list[job_num] = {}, temprary_mode_list[job_num] = {};
	int count_job = 0;
	int i_preorder, i_epilogue, i_preorder_job, i_epilogue_job;
	int position_preorder_job, position_epilogue_job;
	int randomly = 0; int randomly_job = 0, randomly_mode = 0;
	int job_list_initial[job_num] = {}, mode_list_initial[job_num] = {};
	int the_best_job_list[job_num] = {}, the_best_mode_list[job_num] = {};
	int nonrenewable1_temprary = 0, nonrenewable2_temprary = 0;
	int job_temprary = 0, mode_temprary = 0;
	int job_temprary1 = 0, mode_temprary1 = 0;
	for (int i = 0; i < job_num; i++)
	{
		job_temprary = JOB_list[0][i];
		mode_temprary = MODE_list[0][i];
		nonrenewable1_temprary += nonrenewable_resource1[job_temprary - 1][mode_temprary - 1];
		nonrenewable2_temprary += nonrenewable_resource2[job_temprary - 1][mode_temprary - 1];
	}
	for (int i = 0; i < job_num; i++)
	{
		job_list_initial[i] = JOB_list[0][i];
		mode_list_initial[i] = MODE_list[0][i];
		job_list_temprary[i] = JOB_list[0][i];
		mode_list_temprary[i] = MODE_list[0][i];
		the_best_job_list[i] = JOB_list[0][i];
		the_best_mode_list[i] = MODE_list[0][i];
	}
	for (int i = 0; i < job_num; i++)
	{
		do
		{
			uniform_int_distribution<int> dist(0, job_num - 1);
			randomly = dist(rng);
		} while (job_list_initial[randomly] == 0);
		randomly_job = JOB_list[0][randomly];
		randomly_mode = MODE_list[0][randomly];
		for (int m = 0; m < job_num; m++)
		{
			if (job_list_initial[m] == randomly_job)
			{
				job_list_initial[m] = 0;
			}
		}
		job_temprary = JOB_list[0][randomly];
		mode_temprary = MODE_list[0][randomly];
		nonrenewable1_temprary = 0, nonrenewable2_temprary = 0;
		for (int k = 0; k < job_num; k++)
		{
			job_temprary1 = JOB_list[0][k];
			mode_temprary1 = MODE_list[0][k];
			nonrenewable1_temprary += nonrenewable_resource1[job_temprary1 - 1][mode_temprary1 - 1];
			nonrenewable2_temprary += nonrenewable_resource2[job_temprary1 - 1][mode_temprary1 - 1];
		}
		nonrenewable1_temprary = nonrenewable1_temprary - nonrenewable_resource1[job_temprary - 1][mode_temprary - 1];
		nonrenewable2_temprary = nonrenewable2_temprary - nonrenewable_resource2[job_temprary - 1][mode_temprary - 1];
		for (int j = 0; j < mode_num; j++)
		{
			if (j != mode_temprary - 1)
			{
				nonrenewable1_temprary = nonrenewable1_temprary + nonrenewable_resource1[job_temprary - 1][j];
				nonrenewable2_temprary = nonrenewable2_temprary + nonrenewable_resource2[job_temprary - 1][j];
				if (nonrenewable1_temprary <= norenewable_upperenewable_resource[0] && nonrenewable2_temprary <= norenewable_upperenewable_resource[1])
				{
					if (renewable_resource1[job_temprary - 1][j] <= renewable_upperenewable_temporary[0] && renewable_resource2[job_temprary - 1][j] <= renewable_upperenewable_temporary[1])
					{
						MODE_list[0][randomly] = j + 1;
						randomly_mode = MODE_list[0][randomly];
					}
					auto count_s = 0;
					for (auto i = 0; i < job_num; i++)
					{
						for (auto j = 0; j < job_num; j++)
						{
							if (JOB_list[0][i] == j + 1)
							{
								count_s++;
							}
						}
					}
					if (count_s == job_num) {
						for (int j = 0; j < job_num; j++)
						{
							A_decode_job_list[j] = JOB_list[0][j];
							A_decode_mode_list[j] = MODE_list[0][j];
						}
					}
					else
					{
						encode();
						for (int j = 0; j < job_num; j++)
						{
							A_decode_job_list[j] = JOB_list[0][j];
							A_decode_mode_list[j] = MODE_list[0][j];
						}
					}
					decode();
					Best_time();
					if (best_time < BEST_time)
					{
						BEST_time = best_time;
					}
					if (BEST_time < BEST_TIME)
					{
						BEST_TIME = BEST_time;
						for (auto i = 0; i < job_num; i++) { the_best_job_list[i] = A_decode_job_list[i]; the_best_mode_list[i] = A_decode_mode_list[i]; }
						for (int i = 0; i < job_num; i++)
						{
							for (int j = 0; j < 4; j++) { the_best_scheduling_plan[i][j] = scheduling_plan[i][j]; }
						}
						break;
					}
					parameter_update();
					best_time = 0;
				}
				nonrenewable1_temprary = nonrenewable1_temprary - nonrenewable_resource1[job_temprary - 1][j];
				nonrenewable2_temprary = nonrenewable2_temprary - nonrenewable_resource2[job_temprary - 1][j];
			}
		}
		///寻找紧邻前置活动和后置活动
		int immediate_predecessor_job = 0, immediate_successor_job = 0; int count_job = 0, count_job1 = 0; int immediate_predecessor_position = 0, immediate_successor_position = 0;
		for (int i = randomly; i < job_num; i++)
		{
			immediate_successor_job = JOB_list[0][i];
			if (Direct_precedence_relationship[immediate_successor_job - 1][randomly_job - 1] == 1)
			{
				immediate_successor_position = i;
				count_job++;
				break;
			}
		}
		if (count_job == 0)
		{
			immediate_successor_position = job_num - 1;
		}
		count_job = 0;
		for (int i = randomly; i > 0; i--)
		{
			immediate_predecessor_job = JOB_list[0][i];
			if (Direct_precedence_relationship[randomly_job - 1][immediate_predecessor_job - 1] == 1)
			{
				immediate_predecessor_position = i;
				count_job++;
				break;
			}
		}
		if (count_job == 0)
		{
			immediate_predecessor_position = randomly;
		}
		count_job = 0;
		for (int i = 0; i < job_num; i++) { JOB_list[0][i] = the_best_job_list[i]; MODE_list[0][i] = the_best_mode_list[i]; }
		//贪婪交换
		for (int i = immediate_predecessor_position + 1; i < immediate_successor_position; i++)
		{
			int initial_job_list[job_num] = {}, initial_mode_list[job_num] = {};
			for (int i = 0; i < job_num; i++)
			{
				initial_job_list[i] = JOB_list[0][i];
				initial_mode_list[i] = MODE_list[0][i];
			}
			swap(JOB_list[0][i], JOB_list[0][randomly]);
			swap(MODE_list[0][i], MODE_list[0][randomly]);
			for (int i = 0; i < job_num; i++)
			{
				priority_order[i] = 0;
				for (int j = 0; j < job_num; j++)
				{
					M[i][j] = 0;
				}
			}
			for (int i = 0; i < job_num; i++)
			{
				for (int j = 0; j < job_num; j++)
				{
					if (precedence_relationship[i][j] == 1)
					{
						M[i][j] = true;
					}
					else
					{
						M[i][j] = false;
					}
				}
			}
			for (auto j = 0; j < job_num; j++)
			{
				priority_order[j] = JOB_list[0][j] - 1;
			}
			int temprary_job_list[job_num] = {}, temprary_mode_list[job_num] = {};
			for (int m = 0; m < job_num; m++)
			{
				temprary_job_list[m] = JOB_list[0][m];
				temprary_mode_list[m] = MODE_list[0][m];
			}
			fix_priority_order(priority_order, M);
			for (auto j = job_num - 1; j > 0; j--)
			{
				JOB_list[0][job_num - 1 - j] = priority_order[j] + 1;
			}
			for (int m = 0; m < job_num; m++)
			{
				for (int n = 0; n < job_num; n++)
				{
					if (JOB_list[0][m] == temprary_job_list[n])
					{
						MODE_list[0][m] = temprary_mode_list[n];
						break;
					}
				}
			}
			auto count_s = 0;
			for (auto i = 0; i < job_num; i++)
			{
				for (auto j = 0; j < job_num; j++)
				{
					if (JOB_list[0][i] == j + 1)
					{
						count_s++;
					}
				}
			}
			if (count_s == job_num) {
				for (int j = 0; j < job_num; j++)
				{
					A_decode_job_list[j] = JOB_list[0][j];
					A_decode_mode_list[j] = MODE_list[0][j];
				}
			}
			else
			{
				encode();
				for (int j = 0; j < job_num; j++)
				{
					A_decode_job_list[j] = JOB_list[0][j];
					A_decode_mode_list[j] = MODE_list[0][j];
				}
			}
			decode();
			Best_time_one();
			if (best_time < BEST_time)
			{
				BEST_time = best_time;
			}
			if (BEST_time < BEST_TIME)
			{
				BEST_TIME = BEST_time;
				for (int i = 0; i < job_num; i++) { { the_best_job_list[i] = A_decode_job_list[i]; the_best_mode_list[i] = A_decode_mode_list[i]; } }
				for (int i = 0; i < job_num; i++)
				{
					for (int j = 0; j < 4; j++) { the_best_scheduling_plan[i][j] = scheduling_plan[i][j]; }
				}
				break;
			}
			parameter_update();
			best_time = 0;
			for (int i = 0; i < job_num; i++)
			{
				JOB_list[0][i] = initial_job_list[i];
				MODE_list[0][i] = initial_mode_list[i];
			}
		}
	}
	for (int i = 0; i < job_num; i++) { JOB_list[0][i] = the_best_scheduling_plan[i][0]; MODE_list[0][i] = the_best_scheduling_plan[i][1]; }
}
void job_mode_paies()
{
	int mode_replacement_set[9][2] = { {1,1},{1,2},{1,3},{2,1},{2,2},{2,3},{3,1},{3,2},{3,3} };
	int extraction_mode1 = 0, extraction_mode2 = 0, extraction_job1 = 0, extraction_job2 = 0;
	int elite_size = 1;
	int the_best_job_list[job_num] = {}, the_best_mode_list[job_num] = {};
	// 找到最小的10个数及其下标	
	for (int i = 0; i < elite_size; i++)
	{
		uniform_int_distribution<int> dist(1, job_num - 2);
		auto j = dist(rng); auto k = dist(rng);
		extraction_job1 = JOB_list[i][j]; extraction_job2 = JOB_list[i][k];
		extraction_mode1 = MODE_list[i][j]; extraction_mode2 = MODE_list[i][k];
		for (auto a = 0; a < 9; a++)
		{
			if (renewable_resource1[extraction_job1 - 1][mode_replacement_set[a][0] - 1] <= renewable_upperenewable_temporary[0] && renewable_resource2[extraction_job1 - 1][mode_replacement_set[a][0] - 1] <= renewable_upperenewable_temporary[1] && renewable_resource1[extraction_job2 - 1][mode_replacement_set[a][1] - 1] <= renewable_upperenewable_temporary[0] && renewable_resource2[extraction_job2 - 1][mode_replacement_set[a][1] - 1] <= renewable_upperenewable_temporary[1])
			{
				swap(MODE_list[i][j], mode_replacement_set[a][0]);
				swap(MODE_list[i][k], mode_replacement_set[a][1]);
				//计算替换后不可再生资源的消耗量
				int temporary_nonrenewable_used1 = 0, temporary_nonrenewable_used2 = 0;
				for (int c = 0; c < job_num; c++)
				{
					temporary_nonrenewable_used1 += nonrenewable_resource1[JOB_list[i][c] - 1][MODE_list[i][c] - 1];
					temporary_nonrenewable_used2 += nonrenewable_resource2[JOB_list[i][c] - 1][MODE_list[i][c] - 1];
				}
				if (temporary_nonrenewable_used1 <= norenewable_upperenewable_resource[0] && temporary_nonrenewable_used2 <= norenewable_upperenewable_resource[1])
				{
					for (int b = 0; b < job_num; b++)
					{
						A_decode_job_list[b] = JOB_list[i][b];
						A_decode_mode_list[b] = MODE_list[i][b];
					}
					decode();
					Best_time_one();
					if (best_time < BEST_time)
					{
						BEST_time = best_time;
					}
					if (BEST_time < BEST_TIME)
					{
						BEST_TIME = BEST_time;
						for (int i = 0; i < job_num; i++) { the_best_job_list[i] = A_decode_job_list[i]; the_best_mode_list[i] = A_decode_mode_list[i]; }
						for (int i = 0; i < job_num; i++)
						{
							for (int j = 0; j < 4; j++) { the_best_scheduling_plan[i][j] = scheduling_plan[i][j]; }
						}
						//for (int i = 0; i < job_num; i++) { JOB_list[0][i] = the_best_job_list[i]; MODE_list[0][i] = the_best_mode_list[i]; }
						break;
					}
					parameter_update();
					best_time = 0;
				}
				swap(mode_replacement_set[a][0], MODE_list[i][j]);
				swap(mode_replacement_set[a][1], MODE_list[i][k]);
			}
		}
	}
	for (int i = 0; i < job_num; i++) { JOB_list[0][i] = the_best_scheduling_plan[i][0]; MODE_list[0][i] = the_best_scheduling_plan[i][1]; }
}
void job_mode_triples()
{
	int mode_replacement_set[27][3] = { {1,1,1},{1,1,2},{1,1,3},{1,2,1},{1,2,2},{1,2,3},{1,3,1},{1,3,2},{1,3,3},{2,1,1},{2,1,2},{2,1,3},{2,2,1},{2,2,2},{2,2,3},{2,3,1},{2,3,2},{2,3,3},{3,1,1},{3,1,2},{3,1,3},{3,2,1},{3,2,2},{3,2,3},{3,3,1},{3,3,2},{3,3,3} };
	int extraction_mode1 = 0, extraction_mode2 = 0, extraction_mode3 = 0, extraction_job1 = 0, extraction_job2 = 0, extraction_job3 = 0;
	int elite_size = 1;
	for (int i = 0; i < elite_size; i++) {
		for (int n = 0; n < job_num - 2; n++) {
			for (int j = n + 1; j < job_num - 1; j++) {
				for (int k = j + 1; k < job_num; k++) {
					extraction_job1 = JOB_list[i][n]; extraction_job2 = JOB_list[i][j]; extraction_job3 = JOB_list[i][k];
					extraction_mode1 = MODE_list[i][n]; extraction_mode2 = MODE_list[i][j]; extraction_mode3 = MODE_list[i][k];
					for (auto a = 0; a < 27; a++)
					{
						swap(MODE_list[i][n], mode_replacement_set[a][0]);
						swap(MODE_list[i][j], mode_replacement_set[a][1]);
						swap(MODE_list[i][k], mode_replacement_set[a][2]);
						//计算替换后不可再生资源的消耗量
						int temporary_nonrenewable_used1 = 0, temporary_nonrenewable_used2 = 0;
						for (int c = 0; c < job_num; c++)
						{
							temporary_nonrenewable_used1 += nonrenewable_resource1[JOB_list[i][c] - 1][MODE_list[i][c] - 1];
							temporary_nonrenewable_used2 += nonrenewable_resource2[JOB_list[i][c] - 1][MODE_list[i][c] - 1];
						}
						if (temporary_nonrenewable_used1 <= norenewable_upperenewable_resource[0] && temporary_nonrenewable_used2 <= norenewable_upperenewable_resource[1])
						{
							for (int b = 0; b < job_num; b++)
							{
								A_decode_job_list[b] = JOB_list[i][b];
								A_decode_mode_list[b] = MODE_list[i][b];
							}
							decode();
							Best_time_one();
							if (best_time < BEST_time) {
								BEST_time = best_time;
							}
							if (BEST_time < BEST_TIME) {
								BEST_TIME = BEST_time;
								for (int i = 0; i < job_num; i++) {
									for (int j = 0; j < 4; j++) { the_best_scheduling_plan[i][j] = scheduling_plan[i][j]; }
								}
								the_best_resource_first.clear(); the_best_resource_second.clear();
								for (auto i = 0; i < job_num; i++) {
									for (auto l = 0; l < renewable_upperenewable_temporary[0]; l++) { the_best_resource_first[i].push_back(0); }
									for (auto l = 0; l < renewable_upperenewable_temporary[1]; l++) { the_best_resource_second[i].push_back(0); }
								}
								for (auto i = 0; i < job_num; i++) {
									for (auto l = 0; l < renewable_upperenewable_temporary[0]; l++) { the_best_resource_first[i][l] = the_resource_first[i][l]; }
									for (auto l = 0; l < renewable_upperenewable_temporary[1]; l++) { the_best_resource_second[i][l] = the_resource_second[i][l]; }
								}
								break;
							}
							parameter_update();
							best_time = 0;
						}
						swap(mode_replacement_set[a][0], MODE_list[i][n]);
						swap(mode_replacement_set[a][1], MODE_list[i][j]);
						swap(mode_replacement_set[a][2], MODE_list[i][k]);
					}

				}
			}
		}

	}
}
void shaking_phase()
{
	int exchange_list[job_num] = {};
	int job_list_temprary[job_num] = {}, mode_list_temprary[job_num] = {};
	int temprary_job_list[job_num] = {}, temprary_mode_list[job_num] = {};
	int count_job = 0;
	int i_preorder, i_epilogue, i_preorder_job, i_epilogue_job;
	int position_preorder_job, position_epilogue_job;
	int randomly = 0; int randomly_job = 0, randomly_mode = 0;
	int job_list_initial[job_num] = {}, mode_list_initial[job_num] = {};
	int the_best_job_list[job_num] = {}, the_best_mode_list[job_num] = {};
	int nonrenewable1_temprary = 0, nonrenewable2_temprary = 0;
	int job_temprary = 0, mode_temprary = 0;
	int job_temprary1 = 0, mode_temprary1 = 0;
	for (int i = 0; i < job_num; i++)
	{
		job_temprary = JOB_list[0][i];
		mode_temprary = MODE_list[0][i];
		nonrenewable1_temprary += nonrenewable_resource1[job_temprary - 1][mode_temprary - 1];
		nonrenewable2_temprary += nonrenewable_resource2[job_temprary - 1][mode_temprary - 1];
	}
	for (int i = 0; i < job_num; i++)
	{
		job_list_initial[i] = JOB_list[0][i];
		mode_list_initial[i] = MODE_list[0][i];
		job_list_temprary[i] = JOB_list[0][i];
		mode_list_temprary[i] = MODE_list[0][i];
		the_best_job_list[i] = JOB_list[0][i];
		the_best_mode_list[i] = MODE_list[0][i];
	}
	for (int i = 0; i < 1; i++)
	{
		do
		{
			uniform_int_distribution<int> dist(0, job_num - 1);
			randomly = dist(rng);
		} while (job_list_initial[randomly] == 0);
		randomly_job = JOB_list[0][randomly];
		randomly_mode = MODE_list[0][randomly];
		for (int m = 0; m < job_num; m++)
		{
			if (job_list_initial[m] == randomly_job)
			{
				job_list_initial[m] = 0;
			}
		}
		job_temprary = JOB_list[0][randomly];
		mode_temprary = MODE_list[0][randomly];
		nonrenewable1_temprary = 0, nonrenewable2_temprary = 0;
		for (int k = 0; k < job_num; k++)
		{
			job_temprary1 = JOB_list[0][k];
			mode_temprary1 = MODE_list[0][k];
			nonrenewable1_temprary += nonrenewable_resource1[job_temprary1 - 1][mode_temprary1 - 1];
			nonrenewable2_temprary += nonrenewable_resource2[job_temprary1 - 1][mode_temprary1 - 1];
		}
		nonrenewable1_temprary = nonrenewable1_temprary - nonrenewable_resource1[job_temprary - 1][mode_temprary - 1];
		nonrenewable2_temprary = nonrenewable2_temprary - nonrenewable_resource2[job_temprary - 1][mode_temprary - 1];
		for (int j = 0; j < mode_num; j++)
		{
			if (j != mode_temprary - 1)
			{
				nonrenewable1_temprary = nonrenewable1_temprary + nonrenewable_resource1[job_temprary - 1][j];
				nonrenewable2_temprary = nonrenewable2_temprary + nonrenewable_resource2[job_temprary - 1][j];
				if (nonrenewable1_temprary <= norenewable_upperenewable_resource[0] && nonrenewable2_temprary <= norenewable_upperenewable_resource[1])
				{
					if (renewable_resource1[job_temprary - 1][j] <= renewable_upperenewable_temporary[0] && renewable_resource2[job_temprary - 1][j] <= renewable_upperenewable_temporary[1])
					{
						MODE_list[0][randomly] = j + 1;
						randomly_mode = MODE_list[0][randomly];
					}
				}
				nonrenewable1_temprary = nonrenewable1_temprary - nonrenewable_resource1[job_temprary - 1][j];
				nonrenewable2_temprary = nonrenewable2_temprary - nonrenewable_resource2[job_temprary - 1][j];
			}
		}
		///寻找紧邻前置活动和后置活动
		int immediate_predecessor_job = 0, immediate_successor_job = 0; int count_job = 0, count_job1 = 0; int immediate_predecessor_position = 0, immediate_successor_position = 0;
		for (int i = randomly; i < job_num; i++)
		{
			immediate_successor_job = JOB_list[0][i];
			if (Direct_precedence_relationship[immediate_successor_job - 1][randomly_job - 1] == 1)
			{
				immediate_successor_position = i;
				count_job++;
				break;
			}
		}
		if (count_job == 0)
		{
			immediate_successor_position = job_num - 1;
		}
		count_job = 0;
		for (int i = randomly; i > 0; i--)
		{
			immediate_predecessor_job = JOB_list[0][i];
			if (Direct_precedence_relationship[randomly_job - 1][immediate_predecessor_job - 1] == 1)
			{
				immediate_predecessor_position = i;
				count_job++;
				break;
			}
		}
		if (count_job == 0)
		{
			immediate_predecessor_position = randomly;
		}
		count_job = 0;
		//贪婪交换
		for (int i = immediate_predecessor_position + 1; i < immediate_successor_position; i++)
		{
			int initial_job_list[job_num] = {}, initial_mode_list[job_num] = {};
			for (int i = 0; i < job_num; i++)
			{
				initial_job_list[i] = JOB_list[0][i];
				initial_mode_list[i] = MODE_list[0][i];
			}
			swap(JOB_list[0][i], JOB_list[0][randomly]);
			swap(MODE_list[0][i], MODE_list[0][randomly]);
			for (int i = 0; i < job_num; i++)
			{
				priority_order[i] = 0;
				for (int j = 0; j < job_num; j++)
				{
					M[i][j] = 0;
				}
			}
			for (int i = 0; i < job_num; i++)
			{
				for (int j = 0; j < job_num; j++)
				{
					if (precedence_relationship[i][j] == 1)
					{
						M[i][j] = true;
					}
					else
					{
						M[i][j] = false;
					}
				}
			}
			for (auto j = 0; j < job_num; j++)
			{
				priority_order[j] = JOB_list[0][j] - 1;
			}
			int temprary_job_list[job_num] = {}, temprary_mode_list[job_num] = {};
			for (int m = 0; m < job_num; m++)
			{
				temprary_job_list[m] = JOB_list[0][m];
				temprary_mode_list[m] = MODE_list[0][m];
			}
			fix_priority_order(priority_order, M);
			for (auto j = job_num - 1; j > 0; j--)
			{
				JOB_list[0][job_num - 1 - j] = priority_order[j] + 1;
			}
			for (int m = 0; m < job_num; m++)
			{
				for (int n = 0; n < job_num; n++)
				{
					if (JOB_list[0][m] == temprary_job_list[n])
					{
						MODE_list[0][m] = temprary_mode_list[n];
						break;
					}
				}
			}
			break;
		}
	}
}
void activity_key()
{
	//计算当前调度方案下的关键活动
	int preorder_gather[job_num][50] = {}; int epilogue_gather[job_num][50] = {}; int ES_C[job_num] = {}, EF_C[job_num] = {}; int LS_C[job_num] = {}; int LF_C[job_num] = {};
	for (int i = 0; i < job_num; i++)
	{
		int count = 0;
		for (int j = 0; j < job_num; j++)
		{
			if (Direct_precedence_relationship[j][i] == 1)
			{
				preorder_gather[i][count] = j + 1;
				count++;
			}
		}
	}
	for (int i = 0; i < job_num; i++)
	{
		int count1 = 0;
		for (int j = 0; j < job_num; j++)
		{
			if (Direct_precedence_relationship[i][j] == 1)
			{
				epilogue_gather[i][count1] = j + 1;
				count1++;
			}
		}
	}
	int current_time = 0, temprary_job = 0;
	for (int i = 0; i < job_num; i++)
	{
		for (int j = 0; j < 50; j++)
		{
			if (epilogue_gather[i][j] != 0)
			{
				temprary_job = epilogue_gather[i][j];
				if (temprary_job - 1 >= 0) { if (EF_C[temprary_job - 1] > current_time) { current_time = EF_C[temprary_job - 1]; } }
			}
		}
		ES_C[i] = current_time;
		EF_C[i] = current_time + duration[i][the_best_scheduling_plan[i][1] - 1.0];
		current_time = 0;
	}
	int current_time1 = 0, temprary_time = 0, temprary_job1 = 0;
	temprary_time = EF_C[job_num - 1]; current_time1 = temprary_time;
	for (int i = job_num - 1; i >= 0; i--)
	{
		for (int j = 0; j < 50; j++)
		{
			if (preorder_gather[i][j] != 0)
			{
				temprary_job1 = preorder_gather[i][j];
				if (temprary_job1 - 1 >= 0) { if (LS_C[temprary_job1 - 1] < current_time1) { current_time1 = LS_C[temprary_job1 - 1]; } }
			}
		}
		LS_C[i] = current_time1 - duration[i][the_best_scheduling_plan[i][1] - 1.0];
		LF_C[i] = current_time1;
		current_time1 = temprary_time;
	}
	for (auto i = 0; i < job_num; i++) { activity_float[i] = 0; }
	for (auto i = 0; i < job_num; i++)
	{
		activity_float[i] = LS_C[i] - ES_C[i];
	}
}
//调用CPLEX求解
void CPLEX_SEQUENCE()
{
	IloEnv env;
	IloModel model(env);
	//开始时刻连续决策变量
	IloNumVarArray start_time_variable(env, job_num, 0.0, std::numeric_limits<double>::infinity(), ILOFLOAT);
	//序列决策变量
	NumVarMatrix sequence_variable(env, job_num);
	for (IloInt i = 0; i < job_num; i++)
	{
		sequence_variable[i] = IloNumVarArray(env, job_num);
		for (IloInt j = 0; j < job_num; j++)
		{
			if (TE[i][j] == 1 && i < j)
			{
				sequence_variable[i][j] = IloNumVar(env, 0, 1, ILOBOOL);
			}
		}
	}
	//资源重叠变量
	NumVarMatrix resource_overlap_variables(env, job_num);
	for (IloInt i = 0; i < job_num; i++)
	{
		resource_overlap_variables[i] = IloNumVarArray(env, job_num);
		for (IloInt j = 0; j < job_num; j++)
		{
			if (TE[i][j] == 1 && i < j)
			{
				resource_overlap_variables[i][j] = IloNumVar(env, 0, 1, ILOBOOL);
			}
		}
	}
	//资源分配决策变量
	NumVar3Matrix resourse_assignment_variable(env, job_num);
	for (IloInt j = 0; j < job_num; j++)
	{
		resourse_assignment_variable[j] = NumVarMatrix(env, resource_num);
		for (IloInt k = 0; k < resource_num; k++)
		{
			resourse_assignment_variable[j][k] = IloNumVarArray(env, renewable_upperenewable_resource[k]);
			for (IloInt l = 0; l < renewable_upperenewable_resource[k]; l++)
			{
				resourse_assignment_variable[j][k][l] = IloNumVar(env, 0, 1, ILOBOOL);
			}
		}
	}
	//模式选择决策变量
	NumVarMatrix mode_selection_variable(env, job_num);
	for (IloInt j = 0; j < job_num; j++) {
		mode_selection_variable[j] = IloNumVarArray(env, mode_num);
		for (IloInt m = 0; m < mode_num; m++) {
			mode_selection_variable[j][m] = IloNumVar(env, 0, 1, ILOBOOL);												//定义job_num列，整型二维变量mode_selection_variable[j][m]
		}
	}
	//约束1
	IloExpr sumrlik(env), sumrikm(env), sumrlik1(env), sumrikm1(env);
	for (int i = 0; i < job_num; i++)
	{
		sumrlik.clear(), sumrikm.clear(), sumrlik1.clear(), sumrikm1.clear();
		for (int l = 0; l < renewable_upperenewable_resource[0]; l++)
		{
			sumrlik += resourse_assignment_variable[i][0][l];
		}
		for (int m = 0; m < mode_num; m++)
		{
			sumrikm += renewable_resource1[i][m] * mode_selection_variable[i][m];
		}
		model.add(sumrlik == sumrikm);
		for (int l = 0; l < renewable_upperenewable_resource[1]; l++)
		{
			sumrlik1 += resourse_assignment_variable[i][1][l];
		}
		for (int m = 0; m < mode_num; m++)
		{
			sumrikm1 += renewable_resource2[i][m] * mode_selection_variable[i][m];
		}
		model.add(sumrlik1 == sumrikm1);
	}
	//约束2
	IloExpr sumwikm(env), sumwikm1(env);
	sumwikm.clear(), sumwikm1.clear();
	for (int i = 0; i < job_num; i++)
	{
		for (int m = 0; m < mode_num; m++)
		{
			sumwikm += nonrenewable_resource1[i][m] * mode_selection_variable[i][m];
		}
	}model.add(sumwikm <= norenewable_upperenewable_resource[0]);
	for (int i = 0; i < job_num; i++)
	{
		for (int m = 0; m < mode_num; m++)
		{
			sumwikm1 += nonrenewable_resource2[i][m] * mode_selection_variable[i][m];
		}
	}model.add(sumwikm1 <= norenewable_upperenewable_resource[1]);
	//约束3
	IloExpr sumpim(env);
	for (int i = 0; i < job_num; i++)
	{
		for (int j = 0; j < job_num; j++)
		{
			sumpim.clear();
			if (Direct_precedence_relationship[j][i] == 1)
			{
				for (int m = 0; m < mode_num; m++)
				{
					sumpim += duration[i][m] * mode_selection_variable[i][m];
				}
				model.add(start_time_variable[i] + sumpim <= start_time_variable[j]);
			}
		}
	}
	//约束4
	IloExpr sumxim(env);
	for (int i = 0; i < job_num; i++)
	{
		sumxim.clear();
		for (int m = 0; m < mode_num; m++)
		{
			sumxim += mode_selection_variable[i][m];
		}model.add(sumxim == 1);
	}
	//约束5
	IloExpr sumzij(env), sumrik(env);
	for (int i = 0; i < job_num; i++)
	{
		for (int j = 0; j < job_num; j++)
		{
			if (TE[i][j] == 1 && i < j)
			{
				sumzij.clear();
				sumzij = 1 + resource_overlap_variables[i][j];
				for (int k = 0; k < resource_num; k++)
				{
					for (int l = 0; l < renewable_upperenewable_resource[k]; l++)
					{
						sumrik.clear();
						sumrik = resourse_assignment_variable[i][k][l] + resourse_assignment_variable[j][k][l];
						model.add(sumrik <= sumzij);
					}
				}
			}
		}
	}
	//约束6
	IloExpr sumpimx(env);
	for (int i = 0; i < job_num; i++)
	{
		for (int j = 0; j < job_num; j++)
		{
			sumpimx.clear();
			if (TE[i][j] == 1 && i < j)
			{
				for (int m = 0; m < mode_num; m++)
				{
					sumpimx += duration[i][m] * mode_selection_variable[i][m];
				}model.add(start_time_variable[i] + sumpimx - duration_max[i] * (1 - resource_overlap_variables[i][j]) <= start_time_variable[j] + (LS[i] + duration_max[i] - ES[j]) * (1 - sequence_variable[i][j]));
			}
		}
	}
	//约束7
	IloExpr sumpjmx(env);
	for (int i = 0; i < job_num; i++)
	{
		for (int j = 0; j < job_num; j++)
		{
			sumpjmx.clear();
			if (TE[i][j] == 1 && i < j)
			{
				for (int m = 0; m < mode_num; m++)
				{
					sumpjmx += duration[j][m] * mode_selection_variable[j][m];
				}model.add(start_time_variable[j] + sumpjmx - duration_max[j] * (1 - resource_overlap_variables[i][j]) <= start_time_variable[i] + (LS[j] + duration_max[j] - ES[i]) * sequence_variable[i][j]);
			}
		}
	}
	//约束8 模式选择决策变量添加强约束		
	for (int i = 0; i < job_num; i++)
	{
		for (int m = 0; m < mode_num; m++)
		{
			if (temporary_mode_selection[i][m] == 1)
			{
				model.add(mode_selection_variable[i][m] == 1);
			}
		}
	}
	//目标函数
	IloExpr obj(env);
	obj = start_time_variable[job_num - 1];
	model.add(IloMinimize(env, obj));
	//创建求解对象
	IloCplex cplex(model);
	cplex.setParam(IloCplex::Threads, 1);									// 设置最大线程数目为1
	cplex.setParam(IloCplex::TiLim, 600);
	cplex.setParam(IloCplex::EpGap, 1e-4);
	cplex.setOut(env.getNullStream());										// 关闭输出
	//处理异常		
	cout << "begin to solve." << endl;
	try
	{
		if (!cplex.solve())
		{
			cout << "求解失败" << endl;
			if ((cplex.getStatus() == IloAlgorithm::Infeasible) ||
				(cplex.getStatus() == IloAlgorithm::InfeasibleOrUnbounded))
			{
				cout << endl << "No solution - starting Conflict refinement" << endl;
			}
			env.error() << "Failed to optimize LP." << endl;
			throw(-1);
		}
		auto count_ss = 0;
		for (int i = 0; i < job_num; i++)
		{
			for (int m = 0; m < mode_num; m++)
			{
				if (cplex.getValue(mode_selection_variable[i][m]) == double(1))
				{
					count_ss++;
				}
			}
		}
		for (int i = 0; i < job_num; i++)
		{
			for (int m = 0; m < mode_num; m++)
			{
				if (cplex.getValue(mode_selection_variable[i][m]) == double(1) && count_ss == job_num && cplex.getValue(start_time_variable[job_num - 1]) < BEST_TIME)
				{
					the_best_scheduling_plan[i][0] = i + 1;
					the_best_scheduling_plan[i][1] = m + 1;
					the_best_scheduling_plan[i][2] = cplex.getValue(start_time_variable[i]);
					the_best_scheduling_plan[i][3] = cplex.getValue(start_time_variable[i]) + duration[i][m];
				}
			}
		}
		cout << "异常处理完毕" << endl;
		env.out() << "Solution status = " << cplex.getStatus() << endl;
		env.out() << "Solution value = " << cplex.getObjValue() << endl;
		env.out() << "Gap=" << cplex.getMIPRelativeGap() << endl;
		if (cplex.getObjValue() < BEST_TIME) { BEST_TIME = cplex.getObjValue(); }
	}
	catch (IloException& e)
	{
		cerr << "Concert exception caught: " << e << endl;
	}
	catch (...)
	{
		cerr << "Unknown exception caught" << endl;
	}
	env.end();
}
void CPLEX()
{
	auto start = 0; auto end = 0;
	start = clock();
	IloEnv env;
	IloModel model(env);
	//开始时刻连续决策变量
	IloNumVarArray start_time_variable(env, job_num, 0.0, std::numeric_limits<double>::infinity(), ILOFLOAT);
	//序列决策变量
	NumVarMatrix sequence_variable(env, job_num);
	for (IloInt i = 0; i < job_num; i++)
	{
		sequence_variable[i] = IloNumVarArray(env, job_num);
		for (IloInt j = 0; j < job_num; j++)
		{
			if (TE[i][j] == 1 && i < j)
			{
				sequence_variable[i][j] = IloNumVar(env, 0, 1, ILOBOOL);
			}
		}
	}
	//资源重叠变量
	NumVarMatrix resource_overlap_variables(env, job_num);
	for (IloInt i = 0; i < job_num; i++)
	{
		resource_overlap_variables[i] = IloNumVarArray(env, job_num);
		for (IloInt j = 0; j < job_num; j++)
		{
			if (TE[i][j] == 1 && i < j)
			{
				resource_overlap_variables[i][j] = IloNumVar(env, 0, 1, ILOBOOL);
			}
		}
	}
	//资源分配决策变量
	NumVar3Matrix resourse_assignment_variable(env, job_num);
	for (IloInt j = 0; j < job_num; j++)
	{
		resourse_assignment_variable[j] = NumVarMatrix(env, resource_num);
		for (IloInt k = 0; k < resource_num; k++)
		{
			resourse_assignment_variable[j][k] = IloNumVarArray(env, renewable_upperenewable_temporary[k]);
			for (IloInt l = 0; l < renewable_upperenewable_temporary[k]; l++)
			{
				resourse_assignment_variable[j][k][l] = IloNumVar(env, 0, 1, ILOBOOL);
			}
		}
	}
	//模式选择决策变量
	NumVarMatrix mode_selection_variable(env, job_num);
	for (IloInt j = 0; j < job_num; j++) {
		mode_selection_variable[j] = IloNumVarArray(env, mode_num);
		for (IloInt m = 0; m < mode_num; m++) {
			mode_selection_variable[j][m] = IloNumVar(env, 0, 1, ILOBOOL);												//定义job_num列，整型二维变量mode_selection_variable[j][m]
		}
	}
	//约束1
	IloExpr sumrlik(env), sumrikm(env), sumrlik1(env), sumrikm1(env);
	for (int i = 0; i < job_num; i++)
	{
		sumrlik.clear(), sumrikm.clear(), sumrlik1.clear(), sumrikm1.clear();
		for (int l = 0; l < renewable_upperenewable_temporary[0]; l++)
		{
			sumrlik += resourse_assignment_variable[i][0][l];
		}
		for (int m = 0; m < mode_num; m++)
		{
			sumrikm += renewable_resource1[i][m] * mode_selection_variable[i][m];
		}
		model.add(sumrlik == sumrikm);
		for (int l = 0; l < renewable_upperenewable_temporary[1]; l++)
		{
			sumrlik1 += resourse_assignment_variable[i][1][l];
		}
		for (int m = 0; m < mode_num; m++)
		{
			sumrikm1 += renewable_resource2[i][m] * mode_selection_variable[i][m];
		}
		model.add(sumrlik1 == sumrikm1);
	}
	//约束2
	IloExpr sumwikm(env), sumwikm1(env);
	sumwikm.clear(), sumwikm1.clear();
	for (int i = 0; i < job_num; i++)
	{
		for (int m = 0; m < mode_num; m++)
		{
			sumwikm += nonrenewable_resource1[i][m] * mode_selection_variable[i][m];
		}
	}model.add(sumwikm <= norenewable_upperenewable_resource[0]);
	for (int i = 0; i < job_num; i++)
	{
		for (int m = 0; m < mode_num; m++)
		{
			sumwikm1 += nonrenewable_resource2[i][m] * mode_selection_variable[i][m];
		}
	}model.add(sumwikm1 <= norenewable_upperenewable_resource[1]);
	//约束3
	IloExpr sumpim(env);
	for (int i = 0; i < job_num; i++)
	{
		for (int j = 0; j < job_num; j++)
		{
			sumpim.clear();
			if (Direct_precedence_relationship[j][i] == 1)
			{
				for (int m = 0; m < mode_num; m++)
				{
					sumpim += duration[i][m] * mode_selection_variable[i][m];
				}
				model.add(start_time_variable[i] + sumpim <= start_time_variable[j]);
			}
		}
	}
	//约束4
	IloExpr sumxim(env);
	for (int i = 0; i < job_num; i++)
	{
		sumxim.clear();
		for (int m = 0; m < mode_num; m++)
		{
			sumxim += mode_selection_variable[i][m];
		}model.add(sumxim == 1);
	}
	//约束5
	IloExpr sumzij(env), sumrik(env);
	for (int i = 0; i < job_num; i++)
	{
		for (int j = 0; j < job_num; j++)
		{
			if (TE[i][j] == 1 && i < j)
			{
				sumzij.clear();
				sumzij = 1 + resource_overlap_variables[i][j];
				for (int k = 0; k < resource_num; k++)
				{
					for (int l = 0; l < renewable_upperenewable_temporary[k]; l++)
					{
						sumrik.clear();
						sumrik = resourse_assignment_variable[i][k][l] + resourse_assignment_variable[j][k][l];
						model.add(sumrik <= sumzij);
					}
				}
			}
		}
	}
	//约束6
	IloExpr sumpimx(env);
	for (int i = 0; i < job_num; i++)
	{
		for (int j = 0; j < job_num; j++)
		{
			sumpimx.clear();
			if (TE[i][j] == 1 && i < j)
			{
				for (int m = 0; m < mode_num; m++)
				{
					sumpimx += duration[i][m] * mode_selection_variable[i][m];
				}model.add(start_time_variable[i] + sumpimx - duration_max[i] * (1 - resource_overlap_variables[i][j]) <= start_time_variable[j] + (LS[i] + duration_max[i] - ES[j]) * (1 - sequence_variable[i][j]));
			}
		}
	}
	//约束7
	IloExpr sumpjmx(env);
	for (int i = 0; i < job_num; i++)
	{
		for (int j = 0; j < job_num; j++)
		{
			sumpjmx.clear();
			if (TE[i][j] == 1 && i < j)
			{
				for (int m = 0; m < mode_num; m++)
				{
					sumpjmx += duration[j][m] * mode_selection_variable[j][m];
				}model.add(start_time_variable[j] + sumpjmx - duration_max[j] * (1 - resource_overlap_variables[i][j]) <= start_time_variable[i] + (LS[j] + duration_max[j] - ES[i]) * sequence_variable[i][j]);
			}
		}
	}
	int temporary_activity_list[job_num] = {};	int temporary_mode = 0; int count_a = 0;
	for (auto i = 0; i < job_num; i++) { temporary_activity_list[i] = i + 1; }
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis(1, job_num - 1);
	std::set<int> generated;
	while (generated.size() < selected_activity)
	{
		int num = dis(gen);
		if (generated.find(num) == generated.end())
		{
			generated.insert(num);
			temporary_activity_list[num] = 0;
		}
	}
	//约束8 模式选择决策变量添加强约束		
	for (int i = 0; i < job_num; i++)
	{
		for (int m = 0; m < mode_num; m++)
		{
			if (temporary_activity_list[i] != 0 && temporary_mode_selection[i][m] == 1)
			{
				model.add(mode_selection_variable[i][m] == 1);
			}
		}
	}
	//约束9 资源分配决策变量加强约束
	/*vector<int>temporary_resource_row1; vector<int>temporary_resource_row2;
	temporary_resource_row1.clear(); temporary_resource_row2.clear();
	for (int l = 0; l < renewable_upperenewable_resource[0]; l++) { temporary_resource_row1.push_back(0); }
	for (int l = 0; l < renewable_upperenewable_resource[1]; l++) { temporary_resource_row2.push_back(0); }
	for (int i = 0; i < job_num; i++) {
		if (temporary_activity_list[i] == 0) {
			for (int l = 0; l < renewable_upperenewable_resource[0]; l++) {
				if (temporary_resource_assignment1[i][l] == 1) {
					temporary_resource_row1[l] = 1;
				}
			}
			for (int l = 0; l < renewable_upperenewable_resource[1]; l++) {
				if (temporary_resource_assignment2[i][l] == 1) {
					temporary_resource_row2[l] = 1;
				}
			}
		}
	}*/
	for (int i = 0; i < job_num; i++)
	{
		for (int l = 0; l < renewable_upperenewable_temporary[0]; l++)
		{
			if (temporary_activity_list[i] != 0 && temporary_resource_assignment1[i][l] == 1)
			{
				model.add(resourse_assignment_variable[i][0][l] == 1);
			}
		}
		for (int l = 0; l < renewable_upperenewable_temporary[1]; l++)
		{
			if (temporary_activity_list[i] != 0 && temporary_resource_assignment2[i][l] == 1)
			{
				model.add(resourse_assignment_variable[i][1][l] == 1);
			}
		}
	}
	//约束10 序列决策变量添加强约束
	for (IloInt i = 0; i < job_num; i++)
	{
		for (IloInt j = 0; j < job_num; j++)
		{
			if (TE[i][j] == 1 && i < j && temporary_activity_list[i] != 0 && temporary_activity_list[j] != 0 && temporary_sequence_variable[i][j] == 1)
			{
				model.add(sequence_variable[i][j] == 1);
			}
		}
	}
	//约束11 资源重叠变量添加强约束
	for (IloInt i = 0; i < job_num; i++)
	{
		for (IloInt j = 0; j < job_num; j++)
		{
			if (TE[i][j] == 1 && i < j && temporary_activity_list[i] != 0 && temporary_activity_list[j] != 0 && temporary_resource_overlap_variables[i][j] == 1)
			{
				model.add(resource_overlap_variables[i][j] == 1);
			}
		}
	}
	//目标函数
	IloExpr obj(env);
	obj = start_time_variable[job_num - 1];
	model.add(IloMinimize(env, obj));
	//创建求解对象
	IloCplex cplex(model);
	cplex.setParam(IloCplex::Threads, 1);									// 设置最大线程数目为1
	cplex.setParam(IloCplex::TiLim, limit_time*2);
	cplex.setParam(IloCplex::EpGap, 1e-4);
	cplex.setOut(env.getNullStream());										// 关闭输出
	//处理异常		
	cout << "begin to solve." << endl;
	try
	{
		if (!cplex.solve())
		{
			cout << "求解失败" << endl;
			if ((cplex.getStatus() == IloAlgorithm::Infeasible) ||
				(cplex.getStatus() == IloAlgorithm::InfeasibleOrUnbounded))
			{
				cout << endl << "No solution - starting Conflict refinement" << endl;
			}
			env.error() << "Failed to optimize LP." << endl;
			throw(-1);
		}
		auto count_ss = 0;
		for (int i = 0; i < job_num; i++)
		{
			for (int m = 0; m < mode_num; m++)
			{
				if (cplex.getValue(mode_selection_variable[i][m]) == double(1))
				{
					count_ss++;
				}
			}
		}
		for (int i = 0; i < job_num; i++)
		{
			for (int m = 0; m < mode_num; m++)
			{
				if (cplex.getValue(mode_selection_variable[i][m]) == double(1) && count_ss == job_num && cplex.getValue(start_time_variable[job_num - 1]) < BEST_TIME)
				{
					the_best_scheduling_plan[i][0] = i + 1;
					the_best_scheduling_plan[i][1] = m + 1;
					the_best_scheduling_plan[i][2] = cplex.getValue(start_time_variable[i]);
					the_best_scheduling_plan[i][3] = cplex.getValue(start_time_variable[i]) + duration[i][m];
				}
			}
		}
		/*cout << "完整调度方案" << "\n";
		for (int i = 0; i < job_num; i++)
		{
			for (int m = 0; m < mode_num; m++)
			{
				if (temporary_mode_selection[i][m] == 1.0)
				{
					cout << i + 1 << " " << m + 1 << " " << cplex.getValue(start_time_variable[i]) << " " << cplex.getValue(start_time_variable[i]) + duration[i][m];
				}
			}cout << endl;
		}*/
		/*cout << "序列决策变量" << "\n";
		for (IloInt i = 0; i < job_num; i++)
		{
			for (IloInt j = 0; j < job_num; j++)
			{
				if (TE[i][j] == 1 && i < j)
				{
					cout << temporary_sequence_variable[i][j] << " ";
				}
			}cout << "\n";
		}
		cout << " 资源分配变量" << "\n";
		for (IloInt j = 0; j < job_num; j++)
		{
			for (IloInt k = 0; k < resource_num; k++)
			{
				for (IloInt l = 0; l < renewable_upperenewable_resource[k]; l++)
				{
					cout << cplex.getValue(resourse_assignment_variable[j][k][l]) << " ";
				}cout << "\n";
			}
		}
		cout << "资源重叠变量" << "\n";
		for (IloInt i = 0; i < job_num; i++)
		{
			for (IloInt j = 0; j < job_num; j++)
			{
				if (TE[i][j] == 1 && i < j)
				{
					cout << cplex.getValue(resource_overlap_variables[i][j]) << " ";
				}
			}cout << "\n";
		}*/
		//cout << "异常处理完毕" << endl;
		//env.out() << "Solution status = " << cplex.getStatus() << endl;
		//env.out() << "Solution value = " << cplex.getObjValue() << endl;
		//env.out() << "Gap=" << cplex.getMIPRelativeGap() << endl;
		end = clock();
		RCDM_average_time = RCDM_average_time + (end - start);
		gap = cplex.getMIPRelativeGap();
		RCDM_average_gap = RCDM_average_gap + gap;
		if (cplex.getObjValue() < BEST_TIME) { BEST_TIME = cplex.getObjValue(); }
	}
	catch (IloException& e)
	{
		cerr << "Concert exception caught: " << e << endl;
	}
	catch (...)
	{
		cerr << "Unknown exception caught" << endl;
	}
	env.end();
}
int main()
{
	srand(time(nullptr)); // 初始化随机数种子
	vector<string>data_input;
	for (int i = 0; i < 600; i++)data_input.push_back("1");
	data_input[0] = "D:/GA_VNS_INPUT_DATA_J10/data.txt";	
	ofstream  outfile("D:/Evaluation_of_RCDM/J10_INPUT_DATA_M-IGA_scenario_one.txt", ios::app);
	for (int i = 0; i < 1; i++)
	{
		cout << "第" << i + 1 << "个案例" << endl;
		auto average_makespan = 0.0, CPU_time = 0.0;
		//auto start_time = std::chrono::high_resolution_clock::now();
		ifstream inn;
		inn.open(data_input[i]);
		//读入基础数据		
		if (inn)
		{
			inn >> renewable_upperenewable_resource[0], inn >> renewable_upperenewable_resource[1], inn >> norenewable_upperenewable_resource[0], inn >> norenewable_upperenewable_resource[1];
			renewable_upperenewable_temporary[0] = renewable_upperenewable_resource[0];  renewable_upperenewable_temporary[1] = renewable_upperenewable_resource[1];
			for (int i = 0; i < mode_num; i++)
			{
				for (int j = 0; j < job_num; j++)
				{
					inn >> duration[j][i];
					inn >> renewable_resource1[j][i];
					inn >> renewable_resource2[j][i];
					inn >> nonrenewable_resource1[j][i];
					inn >> nonrenewable_resource2[j][i];
				}
			}
			for (int i = 0; i < job_num; i++)
			{
				for (int j = 0; j < job_num; j++)
				{
					inn >> Direct_precedence_relationship[i][j];
				}
			}
			for (int i = 0; i < job_num; i++)
			{
				for (int j = 0; j < job_num; j++)
				{
					inn >> TE[i][j];
				}
			}
			for (int i = 0; i < job_num; i++)
			{
				for (int j = 0; j < job_num; j++)
				{
					inn >> precedence_relationship[i][j];
				}
			}
			for (int i = 0; i < job_num; i++) { inn >> duration_max[i]; }
			for (int i = 0; i < job_num; i++) { inn >> ES[i]; }
			for (int i = 0; i < job_num; i++) { inn >> LS[i]; }
			for (int i = 0; i < job_num; i++) {
				for (int m = 0; m < mode_num; m++) {
					inn >> temporary_mode_selection[i][m];
				}
			}
			const double eps = 1e-6;																		// 定义一个名为eps的常量，值为1e-6
			for (int i = 0; i < job_num; i++) {
				for (int m = 0; m < mode_num; m++) {
					if (abs(temporary_mode_selection[i][m] - 1.0) < eps)
					{
						temporary_mode_selection[i][m] = 1.0;												// 如果变量值非常接近1，则将其强制设置为1
					}
					else if (abs(temporary_mode_selection[i][m]) < eps)
					{
						temporary_mode_selection[i][m] = 0.0;												// 如果变量值非常接近0，则将其强制设置为0
					}
				}
			}
			for (int i = 0; i < job_num; i++) {
				for (int m = 0; m < mode_num; m++) {
					if (temporary_mode_selection[i][m] == 1.0) {
						minimizing_total_mode_list[i] = m + 1;
					}
				}
			}
		}
		//初始化产生一个解
		encode();
		for (int i = 0; i < initial_num; i++)
		{
			for (int j = 0; j < job_num; j++)
			{
				A_decode_job_list[j] = JOB_list[feasible_location][j];
				A_decode_mode_list[j] = MODE_list[feasible_location][j];
			}
			decode();
			Best_time();
			if (best_time < BEST_time)
			{
				BEST_time = best_time;
			}
			if (BEST_time < BEST_TIME)
			{
				BEST_TIME = BEST_time;
				for (int i = 0; i < job_num; i++)
				{
					for (int j = 0; j < 4; j++)
					{
						the_best_scheduling_plan[i][j] = scheduling_plan[i][j];
					}
				}
			}
			parameter_update();
			feasible_location++;
			best_time = 0;
		}feasible_location = 0;
		// 迭代过程
		vector<double>ten_makespan; ten_makespan.clear();
		for (auto k = 0; k < 10; k++)
		{
			RCDM_counter = 0; RCDM_average_time = 0; RCDM_average_gap = 0;
			auto start_time = 0; auto end_time = 0;
			start_time = clock();
			do
			{
				Ehence_insert_neihborhood();
				cout << "数学启发式求解前的解:" << BEST_TIME << endl;
				//job_mode_paies();
				resource_first.clear(); resource_second.clear();
				for (auto i = 0; i < renewable_upperenewable_temporary[0]; i++)
				{
					resource_first.push_back(1);
				}
				for (auto i = 0; i < renewable_upperenewable_temporary[1]; i++)
				{
					resource_second.push_back(1);
				}
				the_resource_first.clear(); the_resource_second.clear();
				for (int i = 0; i < job_num; i++)
				{
					vector<int>row1; vector<int>row2;
					row1.clear(); row2.clear();
					for (int l = 0; l < renewable_upperenewable_temporary[0]; l++) { row1.push_back(0); }
					the_resource_first.push_back(row1);
					for (int l = 0; l < renewable_upperenewable_temporary[1]; l++) { row2.push_back(0); }
					the_resource_second.push_back(row2);
				}
				const double aps = 1e-3;
				vector<int>completion; for (auto i = 0; i < job_num; i++) { completion.push_back(0); }
				for (auto i = 1; i < job_num; i++)
				{
					for (auto j = i - 1; j > 0; j--)
					{
						if (the_best_scheduling_plan[i][2] + aps >= the_best_scheduling_plan[j][3] && completion[the_best_scheduling_plan[j][0] - 1] == 0)
						{
							completion[the_best_scheduling_plan[j][0] - 1] = 1;
							for (auto l = 0; l < renewable_upperenewable_temporary[0]; l++)
							{
								if (the_resource_first[the_best_scheduling_plan[j][0] - 1][l] != 0) { resource_first[l] = the_resource_first[the_best_scheduling_plan[j][0] - 1][l]; }
							}
							for (auto l = 0; l < renewable_upperenewable_temporary[1]; l++)
							{
								if (the_resource_second[the_best_scheduling_plan[j][0] - 1][l] != 0) { resource_second[l] = the_resource_second[the_best_scheduling_plan[j][0] - 1][l]; }
							}
						}
					}
					for (auto l = 0; l < renewable_upperenewable_temporary[0]; l++)
					{
						auto count_1 = 0;
						if (resource_first[l] != 0)
						{
							for (auto k = l; k < renewable_upperenewable_temporary[0]; k++)
							{
								if (resource_first[k] != 0)
								{
									if (count_1 == renewable_resource1[the_best_scheduling_plan[i][0] - 1.0][the_best_scheduling_plan[i][1] - 1.0]) { break; }
									the_resource_first[the_best_scheduling_plan[i][0] - 1.0][k] = 1;
									resource_first[k] = 0;
									count_1++;
								}
							}
							break;
						}
					}
					for (auto l = 0; l < renewable_upperenewable_temporary[1]; l++)
					{
						auto count_2 = 0;
						if (resource_second[l] != 0)
						{
							for (auto k = l; k < renewable_upperenewable_temporary[1]; k++)
							{
								if (resource_second[k] != 0)
								{
									if (count_2 == renewable_resource2[the_best_scheduling_plan[i][0] - 1.0][the_best_scheduling_plan[i][1] - 1.0]) { break; }
									the_resource_second[the_best_scheduling_plan[i][0] - 1.0][k] = 1;
									resource_second[k] = 0;
									count_2++;
								}
							}
							break;
						}
					}
				}
				for (int i = 0; i < job_num; i++)
				{
					for (int j = i + 1; j < job_num; j++)
					{
						if (the_best_scheduling_plan[i][0] > the_best_scheduling_plan[j][0])
						{
							for (int k = 0; k < 4; k++)
							{
								std::swap(the_best_scheduling_plan[i][k], the_best_scheduling_plan[j][k]);
							}
						}
					}
				}
				for (int i = 0; i < job_num; i++)
				{
					for (int m = 0; m < mode_num; m++)
					{
						temporary_mode_selection[i][m] = 0;
					}
				}
				for (int i = 0; i < job_num; i++)
				{
					for (int j = 0; j < job_num; j++)
					{
						temporary_sequence_variable[i][j] = 0;
						temporary_resource_overlap_variables[i][j] = 0;
					}
				}
				temporary_resource_assignment1.clear(); temporary_resource_assignment2.clear();
				for (int i = 0; i < job_num; i++)
				{
					vector<int>row1; vector<int>row2;
					row1.clear(), row2.clear();
					for (int l = 0; l < renewable_upperenewable_temporary[0]; l++)
					{
						row1.push_back(0);
					}
					temporary_resource_assignment1.push_back(row1);
					for (int l = 0; l < renewable_upperenewable_temporary[1]; l++)
					{
						row2.push_back(0);
					}
					temporary_resource_assignment2.push_back(row2);
				}
				for (int i = 0; i < job_num; i++)
				{
					int m = the_best_scheduling_plan[i][1] - 1;
					temporary_mode_selection[i][m] = 1;
				}
				for (int i = 0; i < job_num; i++)
				{
					for (int j = 0; j < job_num; j++)
					{
						if (TE[i][j] == 1 && i < j)
						{
							if (the_best_scheduling_plan[i][2] <= the_best_scheduling_plan[j][2])
								temporary_sequence_variable[i][j] = 1;
						}
					}
				}
				for (auto i = 0; i < job_num; i++)
				{
					for (auto l = 0; l < renewable_upperenewable_temporary[0]; l++)
					{
						temporary_resource_assignment1[i][l] = the_resource_first[i][l];
					}
					for (auto l = 0; l < renewable_upperenewable_temporary[1]; l++)
					{
						temporary_resource_assignment2[i][l] = the_resource_second[i][l];
					}
				}
				for (int i = 0; i < job_num; i++)
				{
					for (int j = 0; j < job_num; j++)
					{
						if (TE[i][j] == 1 && i < j)
						{
							for (int l = 0; l < renewable_upperenewable_temporary[0]; l++)
							{
								if (temporary_resource_assignment1[i][l] == temporary_resource_assignment1[j][l] && temporary_resource_assignment1[i][l] == 1)
								{
									temporary_resource_overlap_variables[i][j] = 1;
									break;
								}
							}
							for (int l = 0; l < renewable_upperenewable_temporary[1]; l++)
							{
								if (temporary_resource_assignment2[i][l] == temporary_resource_assignment2[j][l] && temporary_resource_assignment2[i][l] == 1)
								{
									temporary_resource_overlap_variables[i][j] = 1;
									break;
								}
							}
						}
					}
				}
				//activity_key();
				limit_time = 0;
				limit_time = Maximun_computational_time[k];
				gap = 0.0;
				CPLEX();
				if (gap == 0.0)
				{
					selected_activity = selected_activity + 1;

				}
				else
				{
					selected_activity = selected_activity - 1;
				}
				if (selected_activity >= job_num) { selected_activity = job_num - 1; }
				if (selected_activity <= 0) { selected_activity = 0; }
				const double eps = 1e-6;																		// 定义一个名为eps的常量，值为1e-9
				for (int i = 0; i < job_num; i++)
				{
					for (int m = 0; m < 4; m++)
					{
						if (abs(the_best_scheduling_plan[i][2]) < eps)
						{
							the_best_scheduling_plan[i][2] = 0.0;												// 如果变量值非常接近0，则将其强制设置为0
						}
					}
				}
				for (int i = 0; i < job_num; i++)
				{
					for (int j = i + 1; j < job_num; j++)
					{
						if (the_best_scheduling_plan[i][2] > the_best_scheduling_plan[j][2])
						{
							for (int k = 0; k < 4; k++)
							{
								std::swap(the_best_scheduling_plan[i][k], the_best_scheduling_plan[j][k]);
							}
						}
					}
				}
				//反向编码过程
				for (int j = 0; j < job_num; j++)
				{
					JOB_list[0][j] = the_best_scheduling_plan[j][0];
					MODE_list[0][j] = the_best_scheduling_plan[j][1];
				}
				for (int i = 0; i < job_num; i++)
				{
					priority_order[i] = 0;
					for (int j = 0; j < job_num; j++)
					{
						M[i][j] = 0;
					}
				}
				for (int i = 0; i < job_num; i++)
				{
					for (int j = 0; j < job_num; j++)
					{
						if (precedence_relationship[i][j] == 1)
						{
							M[i][j] = true;
						}
						else
						{
							M[i][j] = false;
						}
					}
				}
				for (auto j = 0; j < job_num; j++)
				{
					priority_order[j] = JOB_list[0][j] - 1;
				}
				int temprary_job_list[job_num] = {}, temprary_mode_list[job_num] = {};
				for (int m = 0; m < job_num; m++)
				{
					temprary_job_list[m] = JOB_list[0][m];
					temprary_mode_list[m] = MODE_list[0][m];
				}
				fix_priority_order(priority_order, M);
				for (auto j = job_num - 1; j > 0; j--)
				{
					JOB_list[0][job_num - 1 - j] = priority_order[j] + 1;
				}
				for (int m = 0; m < job_num; m++)
				{
					for (int n = 0; n < job_num; n++)
					{
						if (JOB_list[0][m] == temprary_job_list[n])
						{
							MODE_list[0][m] = temprary_mode_list[n];
							break;
						}
					}
				}
				end_time = clock();
				//cout << end_time - start_time << endl;
			} while (end_time - start_time <= (job_num - 2) * 1000);
			cout << "Final Solution=:" << (1.0 + uncertain_level * (1.0 - 2.0 * degree_permissilbe_violation)) * BEST_TIME << endl;
			ten_makespan.push_back((1.0 + uncertain_level * (1.0 - 2.0 * degree_permissilbe_violation)) * BEST_TIME);
			//outfile << (1.0 + uncertain_level * (1.0 - 2.0 * degree_permissilbe_violation)) * BEST_TIME << " ";
			best_time = 0;
			BEST_time = INT_MAX; BEST_TIME = INT_MAX;
		}//outfile << endl;
		auto average_ten_makespan = 0.0;
		for (auto i = 0; i < 10; i++)
		{
			average_ten_makespan = average_ten_makespan + ten_makespan[i];
		}
		average_ten_makespan = average_ten_makespan / double(10);
		auto variance = 0.0;
		for (auto i = 0; i < 10; i++)
		{
			variance = variance + pow((ten_makespan[i] - average_ten_makespan), 2);
		}variance = variance / double(10);
		outfile << average_ten_makespan << " " << variance << endl;
	}	
	return 0;
}
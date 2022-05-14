#define _CRT_SECURE_NO_WARNINGS
#include<cstring>
#include<iostream>
#include<map>
#include<vector>
#include<algorithm>
using namespace std;
const int T = 8929, M = 36, N = 136, Max = 1e5 + 5, F = 101;
map<string, int> m_id1; // 客户ID转化为对应数字
map<int, string> m_id2; // 对应数字转化为客户ID 
map<string, int> n_id1; // 节点ID转化为对应数字
map<int, string> n_id2; // 对应数字转化为节点ID 
map<int, string> stream_id[T];
string name[M];
short int id1; // 客户总数   小40
short int id2; // 服务器总数   大140
short int t; // 总天数
char s[Max]; // 输入用的 
int demand[T][F][M]; // 需求
short int stream[T][F][M];
short int flow[T];
int V;
int limit[N]; // 带宽上限
short int q[N][M]; // 时延 
short int Qos; // 时延上限 
short int ser_th;//服务器满载上限
bool full_server_index[T][N];//日满载服务器id
short int specific;//95的时间节点
int ser_seq[N][T];
short int client_connect_num[M];//每个客户连接了多少服务器
//bool exchange_server[N][N];//已经做了交换的服务器，average2用
int ttt;
int fff[T * F * M];

struct wfh {
	vector<int> Node[N];
}out[T][M]; // 存储输出
struct elaina {
	short int index;//原始序号
	short int connect_num;//连接客户数量
	int total_handle_demand;//最大总需求 每日更新
	short int full_times;//满载次数	
	bool connect_client[M];//连接到客户的集合  后续可改为动态		
} server[N];
struct elaina1 {
	short int index;
	int occupy;
	int total_handle_demand;//最大总需求 每日更新
	short int day;
}ser_seq1[N][T];//找出95点的值
struct elaina2 {
	short int flow;
	int request;
}demand0[F];
struct elaina4 {
	short int flow;
	short int client;
	int request;
}demand1[F * M];
struct elaina3 {
	short int index;
	int has_cost95;//记录95点后的开销  overload用
	int rest_need;
}client[M];//调度用
struct elaina5 {
	short int index;
	int demand;
}client1[T][M];
void makerand(elaina A[], int count)
{
	for (int i = 1; i < count; i++)
	{
		int num = i + rand() % (count - i); // 取随机数
		elaina temp = A[i];
		A[i] = A[num];
		A[num] = temp; //交换
	}
}
void makerand(elaina3 A[], int count)
{
	for (int i = 1; i < count; i++)
	{
		int num = i + rand() % (count - i); // 取随机数
		elaina3 temp = A[i];
		A[i] = A[num];
		A[num] = temp; //交换
	}
}
bool cmp(elaina A, elaina B) { return A.connect_num < B.connect_num; }
bool cmp1(elaina2 A, elaina2 B) { return A.request > B.request; }
bool cmp2(elaina1 A, elaina1 B) { return A.occupy < B.occupy; }
bool cmp3(elaina A, elaina B) { return A.index < B.index; }
bool cmp4(elaina4 A, elaina4 B) { return A.request > B.request; }
bool cmp8(elaina3 A, elaina3 B) { return A.has_cost95 < B.has_cost95; }
bool cmp9(elaina5 A, elaina5 B) { return A.demand > B.demand; }
bool cmp10(elaina3 A, elaina3 B) { return A.rest_need > B.rest_need; }
void init()
{
	ser_th = t * 0.05;
	for (int i = 1; i <= id2; i++)
	{
		server[i].index = i;
		for (int j = 1; j <= id1; j++)
		{
			if (q[i][j] < Qos)
			{
				server[i].connect_num++;
				server[i].connect_client[j] = true;
				client_connect_num[j]++;
			}
		}
	}
	specific = t * 0.95 + 0.999;
	for (int j = 1; j <= id2; j++)
	{
		for (int i = 1; i <= t; i++)
		{
			ser_seq1[j][i].index = j;
			ser_seq1[j][i].day = i;
		}
	}
	int inde = 0;
	for (int i = 1; i <= t; i++)
	{
		for (int j = 1; j <= id1; j++)
		{
			for (int k = 1; k <= flow[i]; k++)
			{
				if (demand[i][k][j] != 0)
				{
					fff[inde] = demand[i][k][j];
					inde++;
				}
			}
		}
	}
	inde--;
	sort(fff, fff + inde);
	ttt = fff[inde / 2];
	for (int j = 1; j <= id1; j++)
	{
		client[j].index = j;
		for (int i = 1; i <= t; i++)
		{
			client1[i][j].index = j;
			for (int f = 1; f <= flow[i]; f++)
			{
				if (demand[i][f][j] > ttt)
				{
					//client[j].rest_need += demand[i][f][j];
					client1[i][j].demand += demand[i][f][j];
				}
			}
		}
	}
}
void output()
{
	wfh A; bool flag;
	for (int i = 1; i <= t; i++)
		for (int j = 1; j <= id1; j++)
		{
			flag = 0;
			cout << m_id2[j] << ":";
			A = out[i][j];
			for (int k = 1; k <= id2; k++)
			{
				if (A.Node[k].size() == 0) continue;
				if (flag == 1) cout << ",";
				cout << "<" << n_id2[k] << ",";
				for (int l = 0; l < A.Node[k].size(); l++)
				{
					cout << stream_id[i][A.Node[k][l]];
					if (l != A.Node[k].size() - 1) cout << ",";
				}
				cout << ">";
				flag = 1;
			}
			if (i == t && j == id1) continue;
			printf("\n");
		}
}
void overload1()//对flow排序,线上效果好但是慢    83w 3+200+1+20   554930
{
	sort(server + 1, server + 1 + id2, cmp);
	for (int s = 1; s <= id2; s++)
	{
		for (int time = 0; time < ser_th; time++)
		{
			int demand_max = 0;
			int day_index = 0;
			for (int i = 1; i <= t; i++)
			{
				ser_seq1[server[s].index][i].total_handle_demand = 0;
			}
			for (int i = 1; i <= t; i++)
			{
				for (int k = 1; k <= id1; k++)
				{
					if (q[server[s].index][k] < Qos && limit[server[s].index] - ser_seq[server[s].index][i] * 1.1>0 && server[server[s].index].full_times < ser_th)
					{
						ser_seq1[server[s].index][i].total_handle_demand += client1[i][k].demand;
					}
				}
			}
			for (int i = 1; i <= t; i++)
			{
				if (full_server_index[i][server[s].index] == true && server[server[s].index].full_times >= ser_th)
				{
					ser_seq1[server[s].index][i].total_handle_demand = 0;
				}
			}
			for (int i = 1; i <= t; i++)
			{
				if (ser_seq1[server[s].index][i].total_handle_demand > demand_max)
				{
					demand_max = ser_seq1[server[s].index][i].total_handle_demand;
					day_index = i;
				}
			}
			if (demand_max > 0)
			{
				full_server_index[day_index][server[s].index] = true;
				server[server[s].index].full_times++;
				//sort(client + 1, client + 1 + id1, cmp8);//95点后的开销
				//sort(client1[day_index] + 1, client1[day_index] + 1 + id1, cmp9);//当日需求
				//sort(client + 1, client + 1 + id1, cmp10);//剩下的总量
				//sort(client + 1, client + 1 + id1, cmp11);//链接服务器数量
				for (int f = 0; f < F * M; f++)
				{
					demand1[f].request = 0;
				}
				int index = 1;
				for (int c = 1; c <= id1; c++)
				{
					if (q[server[s].index][c] < Qos)
					{
						for (int f = 1; f <= flow[day_index]; f++)
						{
							if (stream[day_index][f][c] == 0 && demand[day_index][f][c] != 0)
							{
								demand1[index].flow = f;
								demand1[index].client = c;
								demand1[index].request = demand[day_index][f][c];
								index++;
							}
						}
					}
				}
				index--;
				sort(demand1 + 1, demand1 + 1 + index, cmp4);
				for (int f = 1; demand1[f].request != 0 && f <= index; f++)
				{
					if (limit[server[s].index] >= ser_seq[server[s].index][day_index] + demand1[f].request)
					{
						if (demand1[f].request > ttt)
						{
							//client[demand1[f].client].rest_need -= demand1[f].request;
							client1[day_index][demand1[f].client].demand -= demand1[f].request;
						}
						ser_seq[server[s].index][day_index] += demand1[f].request;
						out[day_index][demand1[f].client].Node[server[s].index].push_back(demand1[f].flow);
						stream[day_index][demand1[f].flow][demand1[f].client] = server[s].index;
					}
				}
			}
		}
	}
}
void handle_money()
{
	//sort(server + 1, server + id2 + 1, cmp3);
	int maxuse[N];
	for (int i = 1; i <= id2; i++)
		maxuse[i] = 0;
	for (int i = 1; i <= t; i++)//day
	{
		for (int f = 0; f < F * M; f++)
		{
			demand1[f].request = 0;
		}
		int index = 1;
		for (int c = 1; c <= id1; c++)
		{
			for (int f = 1; f <= flow[i]; f++)
			{
				if (stream[i][f][c] == 0 && demand[i][f][c] != 0)
				{
					demand1[index].flow = f;
					demand1[index].client = c;
					demand1[index].request = demand[i][f][c];
					index++;
				}
			}
		}
		index--;
		sort(demand1 + 1, demand1 + 1 + index, cmp4);
		for (int f = 1; f <= index; f++)
		{
			if (demand[i][demand1[f].flow][demand1[f].client] != 0 && stream[i][demand1[f].flow][demand1[f].client] == 0)
			{
				bool flag = 1;
				for (int s = id2; s >= 1; s--)
				{
					if (q[server[s].index][demand1[f].client] < Qos
						&& ser_seq[server[s].index][i] + demand[i][demand1[f].flow][demand1[f].client] <= maxuse[server[s].index])
					{
						ser_seq[server[s].index][i] += demand[i][demand1[f].flow][demand1[f].client];
						out[i][demand1[f].client].Node[server[s].index].push_back(demand1[f].flow);
						stream[i][demand1[f].flow][demand1[f].client] = server[s].index;
						flag = 0;
						break;
					}
				}
				if (flag)
				{
					int min = 1000000;
					int index = 0;
					for (int s = id2; s >= 1; s--)
					{
						if (q[server[s].index][demand1[f].client] < Qos && ser_seq[server[s].index][i] < min
							&& ser_seq[server[s].index][i] + demand[i][demand1[f].flow][demand1[f].client] <= limit[server[s].index])
						{
							min = ser_seq[server[s].index][i];
							index = s;
						}
					}
					if (index != 0)
					{
						maxuse[server[index].index] = ser_seq[server[index].index][i] + demand[i][demand1[f].flow][demand1[f].client];
						ser_seq[server[index].index][i] += demand[i][demand1[f].flow][demand1[f].client];
						out[i][demand1[f].client].Node[server[index].index].push_back(demand1[f].flow);
						stream[i][demand1[f].flow][demand1[f].client] = server[index].index;
						flag = 0;
					}
				}
				if (flag)
				{
				LOOP:
					int ser_num = rand() % id2 + 1;
					if (q[ser_num][demand1[f].client] >= Qos || limit[ser_num] < ser_seq[ser_num][i] + demand[i][demand1[f].flow][demand1[f].client]) goto LOOP;
					maxuse[ser_num] = ser_seq[ser_num][i] + demand[i][demand1[f].flow][demand1[f].client];
					ser_seq[ser_num][i] += demand[i][demand1[f].flow][demand1[f].client];
					out[i][demand1[f].client].Node[ser_num].push_back(demand1[f].flow);
					stream[i][demand1[f].flow][demand1[f].client] = ser_num;
				}
			}
		}
	}
}
void calculate95()
{
	for (int j = 1; j <= id2; j++)
	{
		for (int i = 1; i <= t; i++)
		{
			ser_seq1[j][i].occupy = ser_seq[j][i];
		}
		sort(ser_seq1[j] + 1, ser_seq1[j] + 1 + t, cmp2);//可改为冒泡提升效率  时间轴打乱
	}
}

void average(bool sortserver)
{
	calculate95();
	int x = 0;
	if (sortserver)
	{
		//sort(server + 1, server + 1 + id2, cmp);
	}
	else
	{
		makerand(server, id2);
	}
	for (int i = 1; i <= t; i++)
	{
		//sort(client + 1, client + 1 + id1, cmp1);
		makerand(client, id1);
		for (int j = 1; j <= id1; j++)
		{
			for (int k = 1; k < id2; k++)
			{
				if ((q[server[k].index][client[j].index] < Qos) && ser_seq1[server[k].index][specific].occupy > V
					&& (ser_seq1[server[k].index][specific].occupy >= ser_seq[server[k].index][i])
					&& (out[i][client[j].index].Node[server[k].index].size() > 0))
				{
					for (int o = k + 1; o <= id2; o++)
					{
						if (q[server[o].index][client[j].index] < Qos)
						{
							if (ser_seq1[server[o].index][specific].occupy > V)
							{
								for (int l = 0; l < out[i][client[j].index].Node[server[k].index].size(); l++)
								{
									if (demand[i][out[i][client[j].index].Node[server[k].index][l]][client[j].index] + ser_seq[server[o].index][i] <= ser_seq1[server[o].index][specific].occupy)
									{
										out[i][client[j].index].Node[server[o].index].push_back(out[i][client[j].index].Node[server[k].index][l]);
										ser_seq[server[o].index][i] += demand[i][out[i][client[j].index].Node[server[k].index][l]][client[j].index];
										ser_seq[server[k].index][i] -= demand[i][out[i][client[j].index].Node[server[k].index][l]][client[j].index];
										out[i][client[j].index].Node[server[k].index].erase(out[i][client[j].index].Node[server[k].index].begin() + l);
										l--;
									}
								}
							}
							else
							{
								for (int l = 0; l < out[i][client[j].index].Node[server[k].index].size(); l++)
								{
									if (demand[i][out[i][client[j].index].Node[server[k].index][l]][client[j].index] + ser_seq[server[o].index][i] <= V)
									{
										out[i][client[j].index].Node[server[o].index].push_back(out[i][client[j].index].Node[server[k].index][l]);
										ser_seq[server[o].index][i] += demand[i][out[i][client[j].index].Node[server[k].index][l]][client[j].index];
										ser_seq[server[k].index][i] -= demand[i][out[i][client[j].index].Node[server[k].index][l]][client[j].index];
										out[i][client[j].index].Node[server[k].index].erase(out[i][client[j].index].Node[server[k].index].begin() + l);
										l--;
									}
								}
							}
						}
					}
				}
			}
		}
	}
}
/*
void average1()//可能会使总费用提高，建议在averge()到局部最优解时使用
{
	calculate95();
	bool bigflag = true;
	while (bigflag)
	{
		bigflag = false;
		//sort(server + 1, server + 1 + id1, cmp);
		for (int i = 1; i <= id2; i++)
		{
			for (int j = 1; j <= id2; j++)
			{
				if (server[i].connect_num < server[j].connect_num)
				{
					bool flag = true;
					for (int k = 1; k <= id1; k++)
					{
						if (server[i].connect_client[k] == true && server[j].connect_client[k] == false)
						{
							flag = false;
						}
					}
					if (flag)
					{
						int min = 1000000;
						for (int k = 1; k <= t; k++)
						{
							if (limit[server[j].index] - ser_seq[server[j].index][k] < min)
							{
								min = limit[server[j].index] - ser_seq[server[j].index][k];
							}
						}
						if (ser_seq1[server[j].index][specific].occupy + min > V
							&& ser_seq1[server[j].index][specific].occupy + min > ser_seq1[server[i].index][specific].occupy)//可以算花费公式优化
							break;
						for (int k = 1; k <= t && min != 0; k++)
						{
							int has_turn = 0;
							if (full_server_index[k][server[i].index] == false)
							{
								for (int o = 1; o <= id1; o++)
								{
									for (int f = 0; f < out[k][o].Node[server[i].index].size(); f++)
									{
										if (demand[k][out[k][o].Node[server[i].index][f]][o] <= min - has_turn)
										{
											out[k][o].Node[server[j].index].push_back(out[k][o].Node[server[i].index][f]);
											ser_seq[server[j].index][k] += demand[k][out[k][o].Node[server[i].index][f]][o];
											ser_seq[server[i].index][k] -= demand[k][out[k][o].Node[server[i].index][f]][o];
											has_turn += demand[k][out[k][o].Node[server[i].index][f]][o];
											out[k][o].Node[server[i].index].erase(out[k][o].Node[server[i].index].begin() + f);
											bigflag = true;
											f--;
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
}
void average2()//可能会使总费用提高，建议在averge()到局部最优解时使用
{
	calculate95();
	//sort(server + 1, server + 1 + id1, cmp);
	for (int i = 1; i <= id2; i++)
	{
		if (ser_seq1[server[i].index][specific].occupy > V)
		{
			bool dra_connect_client[M];
			for (int j = 1; j <= id2 && i != j && exchange_server[server[i].index][server[j].index] == 0; j++)
			{
				if (ser_seq1[server[i].index][specific].occupy <= ser_seq1[server[j].index][specific].occupy)
					continue;
				for (int c = 1; c <= id1; c++)
				{
					dra_connect_client[c] = false;
				}
				for (int d = 1; d <= t; d++)
				{
					for (int c = 1; c <= id1; c++)
					{
						if (out[d][c].Node[server[i].index].size() > 0 && full_server_index[d][server[i].index] != true)
						{
							dra_connect_client[c] = true;
						}
					}
				}
				bool flag = true;
				for (int c = 1; c <= id1; c++)
				{
					if (dra_connect_client[c] == true && q[server[j].index][c] >= Qos)
					{
						flag = false;
					}
				}
				if (flag)
				{
					int min = 1000000;
					for (int k = 1; k <= t; k++)
					{
						if (limit[server[j].index] - ser_seq[server[j].index][k] < min)
						{
							min = limit[server[j].index] - ser_seq[server[j].index][k];
						}
					}
					int a = 0;
					if (ser_seq1[server[j].index][specific].occupy < V)
					{
						a += V - ser_seq1[server[j].index][specific].occupy;
						a += (ser_seq1[server[i].index][specific].occupy - V - a) / 2;
					}
					else
					{
						a += (ser_seq1[server[i].index][specific].occupy + ser_seq1[server[j].index][specific].occupy - 2 * V) / 2;
					}
					a = ser_seq1[server[i].index][specific].occupy - a;
					min = a < min ? a : min;
					int maxtransf = 0;
					for (int d = 1; d <= t; d++)
					{
						int maxflow = 0;
						for (int c = 1; c <= id1; c++)
						{
							if (dra_connect_client[c] == true)
							{
								for (int f = 0; f < out[d][c].Node[server[i].index].size(); f++)
								{
									if (demand[d][out[d][c].Node[server[i].index][f]][c] > maxflow)
										maxflow = demand[d][out[d][c].Node[server[i].index][f]][c];
								}
							}
						}
						if (ser_seq[server[i].index][d] - min - V < maxflow && ser_seq[server[i].index][d] - maxflow < maxtransf)
							maxtransf = ser_seq1[server[i].index][specific].occupy - maxflow;
					}
					min = maxtransf < min ? maxtransf : min;
					if (min > 0)
					{
						exchange_server[server[i].index][server[j].index] = true;
						exchange_server[server[j].index][server[i].index] = true;
						for (int k = 1; k <= t; k++)
						{
							int has_turn = 0;
							if (full_server_index[k][server[i].index] == false)
							{
								for (int o = 1; o <= id1; o++)
								{
									for (int f = 0; f < out[k][o].Node[server[i].index].size(); f++)
									{
										if (demand[k][out[k][o].Node[server[i].index][f]][o] <= min - has_turn)
										{
											out[k][o].Node[server[j].index].push_back(out[k][o].Node[server[i].index][f]);
											ser_seq[server[j].index][k] += demand[k][out[k][o].Node[server[i].index][f]][o];
											ser_seq[server[i].index][k] -= demand[k][out[k][o].Node[server[i].index][f]][o];
											has_turn += demand[k][out[k][o].Node[server[i].index][f]][o];
											out[k][o].Node[server[i].index].erase(out[k][o].Node[server[i].index].begin() + f);
											f--;
										}
									}
								}
							}
						}
					}
					calculate95();
				}
			}
		}
	}
}*/
int transf(int x, int y)
{
	int A = 0;
	for (int i = x; i <= y; i++)
		A = A * 10 + (s[i] - '0');
	return A;
}
void handle_id1()
{
	string A = "";
	scanf("%s", s + 1);
	int n = strlen(s + 1);
	for (int i = 8; i <= n; i++)
	{
		if (s[i] == ',')
		{
			++id1;
			while (s[++i] != ',' && i <= n) A += s[i];
			m_id1[A] = id1;
			m_id2[id1] = A;
			A = "";
			--i;
		}
	}
}
void handle_client()
{
	int tot = 0; int sum = 0;
	char L1 = '&';
	char L2 = '&';
	string A;
	while (scanf("%s", s + 1) != EOF)
	{
		++sum; tot = 0;
		A = "";
		int n = strlen(s + 1);
		for (int i = 1; i <= n; i++)
		{
			if (s[i] == ',')
			{
				if (s[i - 1] != L1 || s[i - 2] != L2)
					flow[t] = sum - 1, ++t, L1 = s[i - 1], L2 = s[i - 2], sum = 1;
				A += s[++i];
				if (s[i + 1] != ',') A += s[++i];
				if (s[i + 1] != ',') A += s[++i];
				stream_id[t][sum] = A;
				for (int j = i + 1; j <= n; j++)
				{
					if (s[j - 1] == ',')
						for (int k = j + 1; k <= n; k++)
						{
							if (s[k] == ',')
								demand[t][sum][++tot] = transf(j, k - 1), j = k + 1;
							if (k == n)
								demand[t][sum][++tot] = transf(j, k), j = k + 1;
						}
				}
				break;
			}
		}
	}
	flow[t] = sum;
}
void handle_node()
{
	scanf("%s", s + 1);
	string A = "";
	while (scanf("%s", s + 1) != EOF)
	{
		A = ""; ++id2;
		int n = strlen(s + 1);
		for (int i = 1; i <= n; i++)
		{
			if (s[i] == ',')
			{
				n_id1[A] = id2;
				n_id2[id2] = A;
				limit[id2] = transf(i + 1, n);
				break;
			}
			A += s[i];
		}
	}
}
void handle_qos()
{
	int n; int A = 0; int B = 0; int S;
	string AI = "";
	scanf("%s", s + 1);
	n = strlen(s + 1);
	for (int i = 1; i <= n; i++)
	{
		if (s[i] == ',')
		{
			S = i + 1;
			break;
		}
	}
	for (int i = S; i <= n; i++)
	{
		if (s[i] == ',') name[++A] = AI, AI = "";
		if (i == n) AI += s[i], name[++A] = AI;
		if (s[i] != ',') AI += s[i];
	}
	A = 0;
	B = 0;
	while (scanf("%s", s + 1) != EOF)
	{
		AI = "";
		B = 0;
		n = strlen(s + 1);
		for (int i = 1; i <= n; i++)
		{
			if (s[i] == ',')
			{
				for (int j = 1; j <= i - 1; j++)
					AI += s[j];
				break;
			}
		}
		A = n_id1[AI];
		for (int i = 1; i <= n; i++)
		{
			if (s[i] == ',')
			{
				++i;
				for (int j = i + 1; j <= n; j++)
				{
					if (s[j] == ',')
						q[A][m_id1[name[++B]]] = transf(i, j - 1), i = j + 1;
					if (j == n)
						q[A][m_id1[name[++B]]] = transf(i, j), i = j + 1;
				}
			}
		}
	}
}
void handle_config()
{
	scanf("%s", s + 1);
	scanf("%s", s + 1);
	int n = strlen(s + 1);
	for (int i = 1; i <= n; i++)
		if (s[i] == '=')
			Qos = transf(i + 1, n);
	scanf("%s", s + 1);
	n = strlen(s + 1);
	for (int i = 1; i <= n; i++)
		if (s[i] == '=')
			V = transf(i + 1, n);
}
long long calculate()
{
	long long A = 0, B = 0, Sum;
	for (int i = 1; i <= id2; i++)
	{
		B = ser_seq1[i][specific].occupy;
		Sum = 0;
		for (int j = 1; j <= t; j++)
			Sum += ser_seq[i][j];
		if (Sum == 0) continue;
		if (B <= V) A += V;
		else A += B + (B - V) * (B - V) / limit[i];
	}
	return A;
}
long long calculate1()
{
	long long A = 0, B = 0;
	for (int i = 1; i <= id2; i++)
	{
		B = ser_seq1[i][specific].occupy;
		A += B;
	}
	return A;
}
int main()
{
	if (freopen("data/demand.csv", "r", stdin) == NULL)
	{
		freopen("/data/demand.csv", "r", stdin);
	}
	handle_id1(); // 处理第一行 
	handle_client(); // 存储客户需求 
	fclose(stdin);
	if (freopen("data/site_bandwidth.csv", "r", stdin) == NULL)
	{
		freopen("/data/site_bandwidth.csv", "r", stdin);
	}
	handle_node(); // 存储节点带宽上限
	fclose(stdin);
	if (freopen("data/qos.csv", "r", stdin) == NULL)
	{
		freopen("/data/qos.csv", "r", stdin);
	}
	handle_qos(); // 顺序存储时延，若到时顺序不与前面一致，则修改
	fclose(stdin);
	if (freopen("data/config.ini", "r", stdin) == NULL)
	{
		freopen("/data/config.ini", "r", stdin);
	}
	handle_config(); // 读时延上限 
	fclose(stdin);
	if (freopen("output/solution.txt", "w", stdout) == NULL)
	{
		freopen("/output/solution.txt", "w", stdout);
	}
	srand(1); //  (unsigned int)time(NULL)
	init();
	overload1();
	handle_money();
	for (int i = 0; i <= 3; i++)
	{
		average(true);
	}
	for (int i = 0; i <= 100; i++)
	{
		average(false);
	}
	output();
	fclose(stdout);
	return 0;
}
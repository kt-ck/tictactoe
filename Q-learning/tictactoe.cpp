#include<bits/stdc++.h>
using namespace std;

const int STATENUM = 19683;
const int PRETRAINNUM = 400;
const int CPU = 1;
const int USER = CPU * -1;
const double alpha = 0.1;
const double gamma = 0.95;
const double INF = 10000000;
vector<pair<int, double>> Qa[STATENUM];
int index1 = -1;
vector<int> num_arr(9, 0); 
int num = 0;
int terminal = -1;



bool isGameOver();
int Next(int action);
int myhash(vector<int>& num_arr);
vector<int> getvector(int num);
void initQa();
int cpu_won();
int user_won();
int isFull();
int min_max(bool);
void init();
void train(bool);

int main(){

	initQa();
	srand((unsigned)time(0)); 
	

	for(int i = 0; i < 9; ++i){
		cout<<"player first:"<<i<<"/"<<9<<"\n";
		for(int j = 0; j < PRETRAINNUM; ++j){
			init();
			num_arr[i] = USER;
			++num;
			if(j <= int(0.8 * PRETRAINNUM)){
				train(false);
			}else{
				train(true);
			}
		}
	}	
	
	for(int i = 0; i < 9; ++i){
		cout<<"qlearning first:"<<i<<"/"<<9<<"\n";
		for(int j = 0; j < PRETRAINNUM; ++j){
			init();
			num_arr[i] = CPU;
			++num;
			min_max(true);
			num_arr[index1] = USER;
			++num;
			if(j <= int(0.8 * PRETRAINNUM)){
				train(false);
			}else{
				train(true);
			}
		}
	}
	
	
	cout<<"training done!\n";
	
	ofstream outfile;
	const string filename = "episode" + to_string(PRETRAINNUM) + ".txt";
	outfile.open(filename, ios::out | ios::trunc);
	for(int i = 0; i < STATENUM; ++i){
		outfile << Qa[i].size()<<" ";
		for(auto p:Qa[i]){
			outfile <<p.first<<" "<< p.second<<" ";
		}
		outfile<<"\n";
	}
	outfile.close();
	
	for(int i = 0 ; i < 10; ++i){
		init();
		if(rand() % 2 == 1){
			//player first
			min_max(true);
			num_arr[index1] = USER;
			++num;
			cout<<"player first\n";
		}
		int flag = 0;
		while(num < 9 && terminal == -1){
			int state = myhash(num_arr);
			int action = -1;
			int t = -INF;
			for(auto p: Qa[state]){
				if(p.second > t){
					action = p.first;
					t = p.second;
				}
			}
			num_arr[action] = CPU;
			++num;
			
			if(isGameOver()){
				terminal = 1;
				flag = 1;
			}
			
			min_max(true);
			num_arr[index1] = USER;
			++num;
			if(isGameOver()){
				terminal = 1;
				flag = -1;
			}
		}
		if(flag == 1){
			cout<<"q-learning won!\n";
		}else if(flag == -1){
			cout<<"min-max won!\n";
		}else{
			cout<<" equal!\n";
		}
	}
	
	
	return 0;
}

void init(){
	index1 = -1;
	fill(num_arr.begin(), num_arr.end(), 0);
	num = 0;
	terminal = -1;
}
void initQa(){
	for(int i = 0 ; i < STATENUM; ++i){
		vector<int> p = getvector(i);
		for(int j = 0 ; j < 9; ++j){
			if(p[j] == 0){
				Qa[i].push_back(make_pair(j, 0));
			}
		}
	}
}

void train(bool greedy){
	while(true){
		int state = myhash(num_arr);
		int action = -1;

		//take an action with Qa table using episode greedy 
		//(1.0 - 1.0 / EPISODENUM)
		if(greedy == false && (double)rand() / RAND_MAX > 0.95){
			//take random action
			int actionindex = int((double)rand() / RAND_MAX * Qa[state].size());
			action = Qa[state][actionindex].first;
//			cout<<"random\n";
	
		}else{
			double maxQa = -INF;
			for(auto p: Qa[state]){
				if(p.second > maxQa){
					maxQa = p.second;
					action = p.first;
				}
			}	
//			cout<<"greedy\n";
		}
		num_arr[action] = CPU;
		++num;

		//get reward and next state
		int reward = Next(action);

//		for(int i = 0; i < 9; ++i){
//			if(i != 0&& i % 3==0){
//				cout<<'\n';
//			}
//			cout<<num_arr[i]<<" ";
//		}
//		cout<<"\n\n";
		for(auto &p: Qa[state]){
			if(p.first == action){
				//calculate max Qa(S', a)
				int nextstate = myhash(num_arr);
				double maxQa_ = -INF;
				
				if(terminal == -1){
					for(auto t: Qa[nextstate]){
						if(t.second > maxQa_){
							maxQa_ = t.second;
						}
					}
				}
				
				//nextstate is terminal state
				if(maxQa_ == -INF){
					maxQa_ = 0;
				}

				p.second = p.second + alpha * (reward + gamma * maxQa_ - p.second);
				break;
			}
		}

		if(terminal != -1){
			break;
		}
		
	}
//	cout<<"\n\n";
}

vector<int> getvector(int num){
	vector<int> p(9, 0);
	int index = 0;
	while(num != 0){
		int m = num % 3;
		int next = num / 3;
		if(m == 2){
			p[index] = CPU;
		}else if(m == 1){
			p[index] = USER;
		}
		index++;
		num = next;
	}
	return p;
}

int myhash(vector<int>& num_arr){
	int ans = 0;
	int pow = 1;
	for(int i = 0 ; i < num_arr.size(); ++i){
		if(num_arr[i] == USER){
			ans += pow;
		}else if(num_arr[i] == CPU){
			ans += pow * 2;
		}
		pow *= 3;
	}
	return ans;
}

int Next(int action){
	int reward;
	if(isGameOver()){
		reward = 100;
		terminal = 1; 
	}else if(num + 1 <= 9){
		min_max(true);
		num_arr[index1] = USER;
		++num;
		if(isGameOver()){
			reward = -100;
			terminal = 1;
		}else if(num == 9){
			reward = -1;
			terminal = 1;
		}else{
			reward = -1;
		}
		
	}else{
		terminal = 1;
		reward = -1;
	}
	return reward;
}

bool isGameOver(){
	
	for (int i = 0; i <= 6; i+=3) {
		int res = 0;
		for (int j = 0; j < 3; ++j) {
			res += num_arr[i + j];
		}
		if (std::abs(res) == 3) {
			return true;
		}
	}
	for (int i = 0; i <= 2; ++i) {
		int res = 0;
		for (int j = 0; j < 3; ++j) {
			res += num_arr[i + 3 * j];
		}
		if (std::abs(res) == 3) {
			return true;
		}
	}
	int res = 0;
	for (int j = 0; j < 3; ++j) {
		res += num_arr[0 + 4 * j];
	}
	if (std::abs(res) == 3) {
		return true;
	}
	res = 0;
	for (int j = 0; j < 3; ++j) {
		res += num_arr[2 + 2 * j];
	}
	if (std::abs(res) == 3) {
		return true;
	}
	
	return false;
}


int cpu_won()
{
	for (int i = 0; i < 9; i += 3)
	{
		if ((num_arr[i] == num_arr[i + 1]) && (num_arr[i + 1] == num_arr[i + 2]) && (num_arr[i] == USER))
			return 1;
	}
	for (int i = 0; i < 3; i++)
	{
		if ((num_arr[i] == num_arr[i + 3]) && (num_arr[i + 3] == num_arr[i + 6]) && (num_arr[i] == USER))
			return 1;
	}
	if ((num_arr[0] == num_arr[4]) && (num_arr[4] == num_arr[8]) && (num_arr[0] == USER))
	{
		return 1;
	}
	if ((num_arr[2] == num_arr[4]) && (num_arr[4] == num_arr[6]) && (num_arr[2] == USER))
	{
		return 1;
	}
	return 0;
}

int user_won()
{
	for (int i = 0; i < 9; i += 3)
	{
		if ((num_arr[i] == num_arr[i + 1]) && (num_arr[i + 1] == num_arr[i + 2]) && (num_arr[i] == CPU))
			return 1;
	}
	for (int i = 0; i < 3; i++)
	{
		if ((num_arr[i] == num_arr[i + 3]) && (num_arr[i + 3] == num_arr[i + 6]) && (num_arr[i] == CPU))
			return 1;
	}
	if ((num_arr[0] == num_arr[4]) && (num_arr[4] == num_arr[8]) && (num_arr[0] == CPU))
	{
		return 1;
	}
	if ((num_arr[2] == num_arr[4]) && (num_arr[4] == num_arr[6]) && (num_arr[2] == CPU))
	{
		return 1;
	}
	return 0;
}

int isFull()// Board is full
{
	for (int i = 0; i < 9; i++)
	{
		if (num_arr[i] == 0)
		{
			return 0;
		}
	}
	return 1;
}

int min_max(bool flag) {

	int max_val = -1000, min_val = 1000;
	int i, j, value = 1;
	if (cpu_won() == 1)
	{
		return 10;
	}
	else if (user_won() == 1)
	{
		return -10;
	}
	else if (isFull() == 1)
	{
		return 0;
	}
	int score[9] = { 1,1,1,1,1,1,1,1,1 };//if score[i]=1 then it is empty

	for (i = 0; i < 9; i++)
	{
		if (num_arr[i] == 0)
		{
			if (min_val > max_val) // reverse of pruning condition.....
			{
				if (flag == true)
				{
					num_arr[i] = USER;
					value = min_max(false);
				}
				else
				{
					num_arr[i] = CPU;
					value = min_max(true);
				}
				num_arr[i] = 0;
				score[i] = value;
			}
		}
	}

	if (flag == true)
	{
		max_val = -1000;
		for (j = 0; j < 9; j++)
		{
			if (score[j] > max_val && score[j] != 1)
			{
				max_val = score[j];
				index1 = j;
			}
		}
		return max_val;
	}
	if (flag == false)
	{
		min_val = 1000;
		for (j = 0; j < 9; j++)
		{
			if (score[j] < min_val && score[j] != 1)
			{
				min_val = score[j];
				index1 = j;
			}
		}
		return min_val;
	}
}

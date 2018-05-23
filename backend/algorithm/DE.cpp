#include <iostream>
#include <stdlib.h>
#include <vector>
#include <time.h>
#include <iomanip>
#include "functions.hpp"
using namespace std;

// 参数结构 
struct Parameter{
	int DIM;				// 维度
	double F;				// 缩放因子 
	double CR;				// 交叉概率 
	int POP_SIZE;			// 种群规模 
	int LOOP_COUNT;			// 迭代次数 
	double LOWER_BOUND;		// 取值下限 
	double UPPER_BOUND;		// 取值上限 
	int FUNC_NUM;			// 计算函数号 
};

// 个体类
class Individual
{
	public:
		vector<double> x; 	// 个体的DIM项 
		double fitness; 	// 适应值
		int DIM;			// 维度
		double LOWER_BOUND;	// 取值下限 
		double UPPER_BOUND;	// 取值上限
		int FUNC_NUM;
		Individual(Parameter &p)
		{
			DIM = p.DIM;
			x.resize(DIM);
			LOWER_BOUND = p.LOWER_BOUND;
			UPPER_BOUND = p.UPPER_BOUND; 
			FUNC_NUM = p.FUNC_NUM;
			for(int i = 0; i < DIM; i++)
				x[i] = LOWER_BOUND + (UPPER_BOUND - LOWER_BOUND) * rand() / (RAND_MAX + 1.0); // 上下限之间的数 
		} 
		double Evaluate()
		{
			fitness = 0;
			// 计算适应值 
			vector<double> data;
			for(int i = 0; i < DIM; i++)
			{
				data.push_back(x[i]);
			}
			Function f(FUNC_NUM, DIM);
			fitness = f(data);
			return fitness;
		}
};

// 差分进化算法类
class DE
{
	public:
		int bestIndex; 				// 最优个体的位置 
		double bestFitness;			// 最优个体的适应值
		int DIM;					// 维度 
		double F;					// 缩放因子 
		double CR;					// 变异概率 
		int POP_SIZE;				// 种群规模 
		int LOOP_COUNT;				// 迭代次数 
		vector<Individual> curPopulation; 	// 当前种群 
		vector<Individual> mutaPopulation; 	// 变异种群 
		vector<Individual> crossPopulation; 	// 交叉种群 
		vector<double> curFitness;         	// 当前种群个体的适应值 

		DE(Parameter &p)  // 初始化，计算种群个体适应值 
		{
			DIM = p.DIM;
			F = p.F;
			CR = p.CR;
			POP_SIZE = p.POP_SIZE;
			LOOP_COUNT = p.LOOP_COUNT;
			for(int i = 0; i < POP_SIZE; i++)
			{
				curPopulation.push_back(Individual(p));
				mutaPopulation.push_back(Individual(p));
				crossPopulation.push_back(Individual(p));
			}
			for(int i = 0; i < POP_SIZE; i++)
				curFitness.push_back(curPopulation[i].Evaluate()); 
		}
		
		void Mutate()	// 变异操作 
		{
			for(int i = 0; i < POP_SIZE; i++)
			{
				// 从当前种群随机选择3个个体，并保证这3个个体连同个体i一共4个个体之间互不相同 
				int rand1, rand2, rand3;
				while(1)
				{
					rand1 = rand() % POP_SIZE;
					rand2 = rand() % POP_SIZE;
					rand3 = rand() % POP_SIZE;
					if(i != rand1 && i != rand2 && i != rand3 && rand1 != rand2 && rand2 != rand3 && rand1 != rand3) break;
				}
				// 变异 
				for(int j = 0; j < DIM; j++) 
				{
					mutaPopulation[i].x[j] = curPopulation[rand1].x[j] + F * (curPopulation[rand2].x[j] - curPopulation[rand3].x[j]); 
				}
				
			}
		}
		
		void Cross() // 交叉操作
		{
			for (int i = 0; i < POP_SIZE; i++)
			{
				int rand1 = rand() % 2;		// 0或1 
				for (int j = 0; j < DIM; j++)
				{
					double rand2 = rand() / (RAND_MAX + 1.0); 	// 0~1之间的随机小数 
					// 交叉公式 
					if (rand2 <= CR || j == rand1) 
						crossPopulation[i].x[j] = mutaPopulation[i].x[j];
					else
						crossPopulation[i].x[j] = curPopulation[i].x[j];
				}
			}
		} 
		
		void Select() // 选择操作
		{
			for(int i = 0; i < POP_SIZE; i++)
			{
				double crossFitness = 	crossPopulation[i].Evaluate(); // 交叉后个体i的适应值
				if(crossFitness <= curFitness[i])
				{
					for (int j = 0; j < DIM; j++)
					{
						curPopulation[i].x[j] = crossPopulation[i].x[j];
					}
					curFitness[i] = crossFitness;
				} 
			} 
		} 
		
		void UpdateBest()  // 更新最优个体 
		{
			bestIndex = 0;
			bestFitness = curFitness[0];
			for(int i = 0; i < POP_SIZE; i++)
			{
				if(curFitness[i] < bestFitness)
				{
					bestIndex = i;
					bestFitness = curFitness[i];
				}
			}
			cout << "Best Fitness: " << bestFitness << endl;
		}
		
		void Run()
		{
			for(int i = 0; i < LOOP_COUNT; i++)
			{
				Mutate();				// 变异
				Cross();				// 交叉
				Select();				// 选择
				UpdateBest(); 			// 更新当前种群最优个体 
				cout<< "Count: "  << setiosflags(ios::left) << setw(8) << i ;
			}
		}
				
};

int main()
{
	srand((int)time(NULL));
	Parameter p = {
		.DIM = 10,				// 维度
		.F = 0.5,				// 缩放因子 
		.CR = 0.3,				// 交叉概率 
		.POP_SIZE = 100,		// 种群规模 
		.LOOP_COUNT = 10000,	// 迭代次数 
		.LOWER_BOUND = -100,	// 取值下限 
		.UPPER_BOUND = 100,		// 取值上限 
		.FUNC_NUM = 1
	};
	DE MyDE(p);
	MyDE.Run(); 
}

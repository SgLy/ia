#include <iostream>
#include <stdlib.h>
#include <vector>
#include <time.h>
#include <iomanip>
#include "functions.hpp"
using namespace std;

// �����ṹ 
struct Parameter{
	int DIM;				// ά��
	double F;				// �������� 
	double CR;				// ������� 
	int POP_SIZE;			// ��Ⱥ��ģ 
	int LOOP_COUNT;			// �������� 
	double LOWER_BOUND;		// ȡֵ���� 
	double UPPER_BOUND;		// ȡֵ���� 
	int FUNC_NUM;			// ���㺯���� 
};

// ������
class Individual
{
	public:
		vector<double> x; 	// �����DIM�� 
		double fitness; 	// ��Ӧֵ
		int DIM;			// ά��
		double LOWER_BOUND;	// ȡֵ���� 
		double UPPER_BOUND;	// ȡֵ����
		int FUNC_NUM;
		Individual(Parameter &p)
		{
			DIM = p.DIM;
			x.resize(DIM);
			LOWER_BOUND = p.LOWER_BOUND;
			UPPER_BOUND = p.UPPER_BOUND; 
			FUNC_NUM = p.FUNC_NUM;
			for(int i = 0; i < DIM; i++)
				x[i] = LOWER_BOUND + (UPPER_BOUND - LOWER_BOUND) * rand() / (RAND_MAX + 1.0); // ������֮����� 
		} 
		double Evaluate()
		{
			fitness = 0;
			// ������Ӧֵ 
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

// ��ֽ����㷨��
class DE
{
	public:
		int bestIndex; 				// ���Ÿ����λ�� 
		double bestFitness;			// ���Ÿ������Ӧֵ
		int DIM;					// ά�� 
		double F;					// �������� 
		double CR;					// ������� 
		int POP_SIZE;				// ��Ⱥ��ģ 
		int LOOP_COUNT;				// �������� 
		vector<Individual> curPopulation; 	// ��ǰ��Ⱥ 
		vector<Individual> mutaPopulation; 	// ������Ⱥ 
		vector<Individual> crossPopulation; 	// ������Ⱥ 
		vector<double> curFitness;         	// ��ǰ��Ⱥ�������Ӧֵ 

		DE(Parameter &p)  // ��ʼ����������Ⱥ������Ӧֵ 
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
		
		void Mutate()	// ������� 
		{
			for(int i = 0; i < POP_SIZE; i++)
			{
				// �ӵ�ǰ��Ⱥ���ѡ��3�����壬����֤��3��������ͬ����iһ��4������֮�以����ͬ 
				int rand1, rand2, rand3;
				while(1)
				{
					rand1 = rand() % POP_SIZE;
					rand2 = rand() % POP_SIZE;
					rand3 = rand() % POP_SIZE;
					if(i != rand1 && i != rand2 && i != rand3 && rand1 != rand2 && rand2 != rand3 && rand1 != rand3) break;
				}
				// ���� 
				for(int j = 0; j < DIM; j++) 
				{
					mutaPopulation[i].x[j] = curPopulation[rand1].x[j] + F * (curPopulation[rand2].x[j] - curPopulation[rand3].x[j]); 
				}
				
			}
		}
		
		void Cross() // �������
		{
			for (int i = 0; i < POP_SIZE; i++)
			{
				int rand1 = rand() % 2;		// 0��1 
				for (int j = 0; j < DIM; j++)
				{
					double rand2 = rand() / (RAND_MAX + 1.0); 	// 0~1֮������С�� 
					// ���湫ʽ 
					if (rand2 <= CR || j == rand1) 
						crossPopulation[i].x[j] = mutaPopulation[i].x[j];
					else
						crossPopulation[i].x[j] = curPopulation[i].x[j];
				}
			}
		} 
		
		void Select() // ѡ�����
		{
			for(int i = 0; i < POP_SIZE; i++)
			{
				double crossFitness = 	crossPopulation[i].Evaluate(); // ��������i����Ӧֵ
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
		
		void UpdateBest()  // �������Ÿ��� 
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
				Mutate();				// ����
				Cross();				// ����
				Select();				// ѡ��
				UpdateBest(); 			// ���µ�ǰ��Ⱥ���Ÿ��� 
				cout<< "Count: "  << setiosflags(ios::left) << setw(8) << i ;
			}
		}
				
};

int main()
{
	srand((int)time(NULL));
	Parameter p = {
		.DIM = 10,				// ά��
		.F = 0.5,				// �������� 
		.CR = 0.3,				// ������� 
		.POP_SIZE = 100,		// ��Ⱥ��ģ 
		.LOOP_COUNT = 10000,	// �������� 
		.LOWER_BOUND = -100,	// ȡֵ���� 
		.UPPER_BOUND = 100,		// ȡֵ���� 
		.FUNC_NUM = 1
	};
	DE MyDE(p);
	MyDE.Run(); 
}

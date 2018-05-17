/*
  CEC17 Test Function Suite for Single Objective Optimization
  Noor Awad (email: noor0029@ntu.edu.sg) 
  Sep. 10th 2016
*/

#include <WINDOWS.H>    
#include <stdio.h>
#include <math.h>
#include <malloc.h>
#include "cec17_test_func.cpp" 
#include <vector>
#include <ctime>
#include  "function.hpp"

#define N 100
#define MAX 100
#define Vmax 20
#define w 0.729
#define c1 1.49445
#define c2 1.49445
#define loop 10000 
#define random(a,b) (rand()%(b-a+1)+a)

using namespace std;

//int m;
int n = 2; //dimension
vector<double> gBest;//global best
double gBestF;

double *x;
int func_num = 1;
Function f(func_num, n, "input_data");

double *OShift,*M,*y,*z,*x_bound;
int ini_flag=0,n_flag,func_flag,*SS;

class Particle{
public:
	vector<double> x;
	vector<double> v;
	vector<double> pBest;
	double pBestF;
	Particle(){
		for (int i = 0; i < n; i++){
			x.push_back(0.0);
			v.push_back(0.0);
			pBest.push_back(0.0);
		}
		pBestF = 1e100;
	}
};

Particle PList[N];




void cec17_test_func(double *, double *,int,int,int); //x, f, dimension, population_size, func_num

void init(){
	gBestF = 1e100;
	//f=(double *)malloc(sizeof(double)  *  m);
	srand(time(0));
	for (int i = 0; i < n; i++)
		gBest.push_back(0.0);
	for (int i = 0; i < N; i++)
		for (int j = 0; j < n; j++){
			PList[i].x[j] = random(-MAX, MAX);
			PList[i].v[j] = random(-Vmax, Vmax);
		}
}

double func(vector<double> x, int n,  int func_num){ //return the value of F(x)
	//cec17_test_func(x,f,n,m,func_num);
	return f(x);
}

void update_single(Particle& p){
	for (int i = 0; i < n; i++){
		p.v[i] = p.v[i] * w + (p.pBest[i] - p.x[i]) * c1 * rand()/double(RAND_MAX) + (gBest[i]-p.x[i]) * c2 * rand()/double(RAND_MAX);
		//printf("%Lf ", p.v[i]);
		if (p.x[i]+p.v[i] > MAX )
			p.x[i] = MAX;
		else if(p.x[i]+p.v[i] < -MAX)
			p.x[i] = -MAX;
		else 
			p.x[i] += p.v[i];
	}
	double tmp = func(p.x, n, func_num);
	if (tmp < p.pBestF){
		p.pBestF = tmp;
		for (int i = 0; i < n; i++){
			p.pBest[i] = p.x[i];
		}
	}
	
}
void update_global(){
	for(int i = 0; i < N; i++)
		update_single(PList[i]);
	for (int i = 0; i < N; i++){
		double tmp = func(PList[i].pBest, n, func_num);
		if (tmp < gBestF){
			gBestF = tmp;
			for (int j = 0; j < n; j++)
				gBest[j] = PList[i].pBest[j];
		}
	}
}

void PSO(){
	for(int i = 0; i < loop; i++){
		update_global();
	}
} 


int main()
{
	init();	
	PSO();
	printf("best result: %Lf\n", gBestF);
	printf("vector x:");
	for (int i = 0; i < n; i++)
		printf(" %Lf", gBest[i]);
	/*x=(double *)malloc(n*sizeof(double));
	

		x[0] = -55.276398;
		x[1] = -70.429560;
		//x[0] = 1;
		//x[1] = 2;
		

			for (j = 0; j < n; j++)
			{
				x[1*n+j]=0.0;
				//printf("%Lf\n",x[1*n+j]);
			}
		
		
		for (int k = 0; k < 1; k++)
		{
			cec17_test_func(x, f, n,m,func_num);
			for (int j = 0; j < 2; j++)
			{
				printf(" f%d(x[%d]) = %Lf,",func_num,j+1,f[j]);
			}

			printf("\n%Lf", func(x,n,m,func_num));
		}
	*/
	free(x);
	//free(f);
	free(y);
	free(z);
	free(M);
	free(OShift);
	free(x_bound);
	return 0;
}



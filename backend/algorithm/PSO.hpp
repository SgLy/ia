/*
  CEC17 Test Function Suite for Single Objective Optimization
  Noor Awad (email: noor0029@ntu.edu.sg) 
  Sep. 10th 2016
*/
#include <math.h>
#include <WINDOWS.H>    
#include <stdio.h>
#include <math.h>
#include <malloc.h>
#include <vector>
#include <ctime>
#include  "functions.hpp"

#define N 20
#define MAX 100
#define Vmax 20
#define w 0.729
#define c1 1.49445
#define c2 1.49445
#define loop 1000000 
#define random(a,b) (rand()%(b-a+1)+a)
#define u 0.1	//位置因子 
#define e 0.5//速度因子 

using namespace std;

int count2; //numbers of particles that have been reseted
int n = 10; //dimension
vector<double> gBest;//global best
double gBestF;
vector<double> ggBest;
double ggBestF;
int func_num = 3;
Function f(func_num, n);


class Particle{
public:
	vector<double> x;
	vector<double> v;
	vector<double> pBest;
	double pBestF;
	Particle(){
		x.resize(n);
		v.resize(n);
		pBest.resize(n);
		for (int i = 0; i < n; i++){
			x[i] = random(-MAX, MAX);
			v[i] = random(-Vmax, Vmax);
			pBest[i] = x[i];
		}
		pBestF = f(x);
	}
};

Particle PList[N];




double distance(Particle& p){//get the distance between p and global best
	double d = 0;
	for (int i = 0; i < n; i++)
		d += ((gBest[i]-p.x[i]) * (gBest[i]-p.x[i]));
	d = sqrt(d);
	return d;
}
double velocity(Particle& p){
	double v = 0;
	for (int i = 0; i < n; i++)
		v += (p.v[i]*p.v[i]);
	v = sqrt(v);
	return v;
}
void RandomPList(){
	for (int i = 0; i < N; i++){
		for (int j = 0; j < n; j++){
			PList[i].x[j] = random(-MAX , MAX );
			PList[i].v[j] = random(-Vmax, Vmax);
			PList[i].pBest[j] = PList[i].x[j]; 
		}
		PList[i].pBestF = f(PList[i].x);
	}
	
	gBestF = 1e100;
	gBest.resize(n);
	for (int i = 0; i < N; i++)
		if (PList[i].pBestF < gBestF){
			gBestF = PList[i].pBestF;
			for (int j = 0; j < n; j++){
				gBest[j] = PList[i].pBest[j];
			}
		}
}

void init(){
	RandomPList();
}
void init_particle(Particle& p){
	for (int i = 0; i < n; i++){
			p.x[i] = random(-MAX, MAX);
			p.v[i] = random(-Vmax, Vmax);
		}
		/*double fitness = f(p.x);
		if (fitness < p.pBestF){
			p.pBestF = fitness;
			for (int i = 0; i < n; i++)
				p.pBest[i] = p.x[i];
		}*/	
}

void save_result(){
	if (gBestF < ggBestF){
		ggBestF = gBestF;
		for (int i = 0; i < n; i++)
			ggBest[i] = gBest[i]; 		
	}
}

double func(vector<double> x, int n,  int func_num){ //return the value of F(x)
	return f(x);
}

void update_single(Particle& p){
	for (int i = 0; i < n; i++){
		p.v[i] = w * (p.v[i] + (p.pBest[i] - p.x[i]) * c1 * rand()/double(RAND_MAX) + (gBest[i]-p.x[i]) * c2 * rand()/double(RAND_MAX) );

		double d = distance(p);
		double v = velocity(p);
		if (d < u && v < e){
			init_particle(p);
			count2++;
		}
		else {
			if (p.x[i]+p.v[i] > MAX )
				p.x[i] = MAX;
			else if(p.x[i]+p.v[i] < -MAX)
				p.x[i] = -MAX;
			else 
				p.x[i] += p.v[i];
		}
	}
	double tmp = func(p.x, n, func_num);
	if (tmp < p.pBestF){
		p.pBestF = tmp;
		for (int i = 0; i < n; i++){
			p.pBest[i] = p.x[i];
		}
	}
	
}
void update_global(){//同步更新gBest 
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

void update_global_2(){//异步更新gBest 
	for(int i = 0; i < N; i++){
		update_single(PList[i]);
	
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
			count2 = 0;
			update_global();
			printf("round %d: %Lf  reset %d particles\n", i, gBestF,count2);
		}
		
	
} 


int Run(int func_num, int n)
{
	
	srand((unsigned)time(NULL));
	init();		
	PSO();
	printf("best result: %Lf\n", gBestF);
	printf("vector x:");
	for (int i = 0; i < n; i++)
		printf(" %Lf", gBest[i]);


	return 0;
}

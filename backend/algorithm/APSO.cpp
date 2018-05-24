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

#define N 30
#define w 0.5
#define c1 1
#define c2 2
#define loop 1000000
//#define random(a,b) (rand()%(b-a+1)+a)
#define random(a,b) (rand()/double(RAND_MAX+1))*(b-a)+a
#define u 0.1	//位置因子 
#define e 0.5//速度因子 

using namespace std;

double MAX = 6.35;
double MIN = -6.4;
double Vmax = 2;
int count2; //numbers of particles that have been reseted
int n = 6; //dimension
vector<double> gBest;//global best
double gBestF;
vector<double> ggBest;
double ggBestF ;
int func_num = 8;
Function f(func_num, n);


double func(vector<double> x, int n,  int func_num){ //return the value of F(x)
	return f(x);
}

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
			x[i] = random(MIN, MAX);
			v[i] = random(-Vmax, Vmax);
			pBest[i] = x[i];
		}
		pBestF = func(x, n, func_num);
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
			PList[i].x[j] = random(MIN , MAX );
			PList[i].v[j] = random(-Vmax, Vmax);
			PList[i].pBest[j] = PList[i].x[j]; 
		}
		PList[i].pBestF = func(PList[i].x, n, func_num);
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
	ggBestF = 1e100;
	ggBest.resize(n);
	RandomPList();
}
void init_particle(Particle& p){
	for (int i = 0; i < n; i++){
			p.x[i] = random(MIN, MAX);
			p.v[i] = random(-Vmax, Vmax);
		}
	double fitness = func(p.x, n, func_num);
	if (1){
		p.pBestF = fitness;
		for (int i = 0; i < n; i++)
			p.pBest[i] = p.x[i];
	}
}

void init_gBest(Particle& p){
	for (int i = 0; i < n; i++){
		gBest[i] = p.x[i];
	}
	gBestF = func(gBest, n, func_num);
} 



void update_single(Particle& p){
	for (int i = 0; i < n; i++){
		p.v[i] =  (w* p.v[i] + (p.pBest[i] - p.x[i]) * c1 * rand()/double(RAND_MAX) + (gBest[i]-p.x[i]) * c2 * rand()/double(RAND_MAX) );
		if (p.v[i] > Vmax)
			p.v[i] = Vmax;
		else if (p.v[i] < -Vmax)
			p.v[i] = -Vmax;
		double d = distance(p);
		double v = velocity(p);
		if (d < u && v < e){ // reset the particle (and gBest)
			init_particle(p);
			if (gBestF < ggBestF){
				ggBestF = gBestF;
				for (int k = 0; k < n; k++)
					ggBest[k] = gBest[k];
			}
			init_gBest(p);
			count2++;
		}
		else {
			if (p.x[i]+p.v[i] > MAX )
				p.x[i] = MAX;
			else if(p.x[i]+p.v[i] < MIN)
				p.x[i] = MIN;
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
			if (gBestF < ggBestF){//update ggBest and ggBestF
				ggBestF = gBestF;
				for (int k = 0; k < n; k++)
					ggBest[k] = gBest[k];
			}
			printf("round %d: %Lf  reset %d particles\n", i, ggBestF,count2);
			//print all the particles
			if (n == 2 && i>9000)
			{
				for (int i = 0; i < N; i++){
					printf("particle %d: ",i);
					for (int j = 0; j < n ; j++){
						printf(" %lf", PList[i].x[j]);
					}
					printf("\n");
				}
			} 
		}
		
	
} 


int main()
{
	
	srand((unsigned)time(NULL));
	init();		
	PSO();
	printf("best result: %Lf\n", ggBestF);
	printf("vector x:");
	for (int i = 0; i < n; i++)
		printf(" %Lf", gBest[i]);


	return 0;
}


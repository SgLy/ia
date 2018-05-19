#include "functions.hpp"
#include <ctime>
#include <malloc.h>
#include <math.h>
#include <stdio.h>
#include <vector>

int cnt = 0;
auto newFile = [&]() {
    std::string name = ("tmp/PSO" + std::to_string(cnt++)).c_str();
    return std::unique_ptr<FILE, decltype(&fclose)>(fopen(name.c_str(), "w"), &fclose);
};

#define N 20
#define MAX 100
#define Vmax 20
#define w 0.729
#define c1 1.49445
#define c2 1.49445
#define loop 1000000
#define random(a, b) (rand() % (b - a + 1) + a)
#define u 0.1 //位置因子
#define e 0.5 //速度因子

using namespace std;

int count2; //numbers of particles that have been reseted
int n = 0;
vector<double> gBest; //global best
double gBestF;
vector<double> ggBest;
double ggBestF;
int func_num = 0;
Function f;

class Particle {
public:
    vector<double> x;
    vector<double> v;
    vector<double> pBest;
    double pBestF;
    Particle()
    {
        x.resize(n);
        v.resize(n);
        pBest.resize(n);
        for (int i = 0; i < n; i++) {
            x[i] = random(-MAX, MAX);
            v[i] = random(-Vmax, Vmax);
            pBest[i] = x[i];
        }
        pBestF = f(x);
    }
};

// Particle PList[N];
vector<Particle> PList;

double distance(Particle& p)
{ //get the distance between p and global best
    double d = 0;
    for (int i = 0; i < n; i++)
        d += ((gBest[i] - p.x[i]) * (gBest[i] - p.x[i]));
    d = sqrt(d);
    return d;
}
double velocity(Particle& p)
{
    double v = 0;
    for (int i = 0; i < n; i++)
        v += (p.v[i] * p.v[i]);
    v = sqrt(v);
    return v;
}
void RandomPList()
{
    PList.resize(N);
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < n; j++) {
            PList[i].x[j] = random(-MAX, MAX);
            PList[i].v[j] = random(-Vmax, Vmax);
            PList[i].pBest[j] = PList[i].x[j];
        }
        PList[i].pBestF = f(PList[i].x);
    }

    gBestF = 1e100;
    gBest.resize(n);
    for (int i = 0; i < N; i++)
        if (PList[i].pBestF < gBestF) {
            gBestF = PList[i].pBestF;
            for (int j = 0; j < n; j++) {
                gBest[j] = PList[i].pBest[j];
            }
        }
}

void init()
{
    RandomPList();
}
void init_particle(Particle& p)
{
    for (int i = 0; i < n; i++) {
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

void save_result()
{
    if (gBestF < ggBestF) {
        ggBestF = gBestF;
        for (int i = 0; i < n; i++)
            ggBest[i] = gBest[i];
    }
}

double func(vector<double> x, int n, int func_num)
{ //return the value of F(x)
    return f(x);
}

void update_single(Particle& p)
{
    for (int i = 0; i < n; i++) {
        p.v[i] = w * (p.v[i] + (p.pBest[i] - p.x[i]) * c1 * rand() / double(RAND_MAX) + (gBest[i] - p.x[i]) * c2 * rand() / double(RAND_MAX));

        double d = distance(p);
        double v = velocity(p);
        if (d < u && v < e) {
            init_particle(p);
            count2++;
        } else {
            if (p.x[i] + p.v[i] > MAX)
                p.x[i] = MAX;
            else if (p.x[i] + p.v[i] < -MAX)
                p.x[i] = -MAX;
            else
                p.x[i] += p.v[i];
        }
    }
    double tmp = func(p.x, n, func_num);
    if (tmp < p.pBestF) {
        p.pBestF = tmp;
        for (int i = 0; i < n; i++) {
            p.pBest[i] = p.x[i];
        }
    }
}
void update_global()
{ //同步更新gBest
    for (int i = 0; i < N; i++)
        update_single(PList[i]);

    for (int i = 0; i < N; i++) {
        double tmp = func(PList[i].pBest, n, func_num);
        if (tmp < gBestF) {
            gBestF = tmp;
            for (int j = 0; j < n; j++)
                gBest[j] = PList[i].pBest[j];
        }
    }
}

void update_global_2()
{ //异步更新gBest
    for (int i = 0; i < N; i++) {
        update_single(PList[i]);

        double tmp = func(PList[i].pBest, n, func_num);
        if (tmp < gBestF) {
            gBestF = tmp;
            for (int j = 0; j < n; j++)
                gBest[j] = PList[i].pBest[j];
        }
    }
}

void PSO()
{
    for (int i = 0; i < 50; i++) {
        count2 = 0;
        update_global();
        // if (i % 100 == 0) {
            auto f = newFile();
            fprintf(f.get(), R"( {"type": "round", "data":{ "round": %d, "best": %lf, "particles": %d }} )", i, gBestF, count2);

            if (n != 2)
                continue;
            f = newFile();
            fprintf(f.get(), R"( {"type":"particles", "data":[ )");
            for (int i = 0; i < N; i++) {
                fprintf(f.get(), "[");
                for (int j = 0; j < n; j++)
                    fprintf(f.get(), "%lf%c", PList[i].x[j], j == n - 1 ? ']' : ',');
                fprintf(f.get(), i == N - 1 ? "]}" : ",");
            }
        // }
    }
}

int Run(int _func_num, int _n)
{
    // set global vars
    func_num = _func_num;
    n = _n;
    f = Function(func_num, n);
    srand((unsigned)time(NULL));
    init();
    PSO();
    auto f = newFile();
    fprintf(f.get(), R"( { "type": "best", "data": { "value": %lf, "vector": [ )", gBestF);
    for (int i = 0; i < n; i++)
        fprintf(f.get(), "%lf%c", gBest[i], i == n - 1 ? ' ' : ',');
    fprintf(f.get(), R"( ]}} )");

    return 0;
}

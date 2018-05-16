#include <stdio.h>
#include <vector>

using std::vector;

typedef vector<double> vec;
typedef vector<vec> mat;

FILE * file;
vec o;
mat M;

const int D = 10;

vec operator- (const vec &a, const vec &b) {
    vec res;
    res.resize(D);
    for (int i = 0; i < D; ++i)
        res[i] = a[i] - b[i];
    return res;
}

vec operator* (const mat &m, const vec &v)
{
    vec res;
    res.resize(D);
    for (int i = 0; i < D; ++i) {
        res[i] = 0;
        for (int j = 0; j < D; ++j)
            res[i] += m[i][j] * v[j];
    }
    return res;
}

double F(
    const vec &x,
    const vec &o,
    const mat &M,
    double offset
    ) {
    auto t = M * (x - o);
    // Bent Cigar Function
    double res = 0;
    for (int i = 1; i < D; ++i)
        res += t[i] * t[i];
    return res * 1e6 + t[0] * t[0];
}

int main()
{
    int func_num = 1;
    char * filename = new char[256];
    
    sprintf(filename, "../codes/C version/input_data/shift_data_%d.txt", func_num);
    file = fopen(filename, "r");
    o.resize(D);
    for (int i = 0; i < D; ++i)
        fscanf(file, "%lf", &o[i]);
    fclose(file);

    sprintf(filename, "../codes/C version/input_data/M_%d_D%d.txt", func_num, D);
    file = fopen(filename, "r");
    M.resize(D);
    for (int i = 0; i < D; ++i) {
        M[i].resize(D);
        for (int j = 0; j < D; ++j)
            fscanf(file, "%lf", &M[i][j]);
    }
    fclose(file);

    vec x;
    x.resize(D);
    for (int i = 0; i < D; ++i)
        x[i] = 100;
    printf("%.5lf\n", F(x, o, M, 100));

    return 0;
}
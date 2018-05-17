#ifndef FUNCTIONS_HPP_INCLUDED
#define FUNCTIONS_HPP_INCLUDED

#include <functional>
#include <memory>
#include <vector>
#include <stdio.h>

typedef std::vector<double> vec;
typedef std::vector<vec> mat;

vec minus(const vec &a, const vec &b, int D)
{
    vec res;
    res.resize(D);
    for (int i = 0; i < D; ++i)
        res[i] = a[i] - b[i];
    return res;
}

vec multiply(const mat &m, const vec &v, int D)
{
    vec res;
    res.resize(D);
    for (int i = 0; i < D; ++i)
    {
        res[i] = 0;
        for (int j = 0; j < D; ++j)
            res[i] += m[i][j] * v[j];
    }
    return res;
}

typedef std::function<double(const vec&, const vec&, const mat&, int)> func_t;
func_t functions[10] = {
    // 1. Bent Cigar Function
    [](const vec &x, const vec &o, const mat &M, int D) {
        auto t = multiply(M, minus(x, o, D), D);
        double res = 0;
        for (int i = 1; i < D; ++i)
            res += t[i] * t[i];
        return res * 1e6 + t[0] * t[0];
    }};

class Function {
    public:
        Function(
            int func_num,
            int D = 2,
            std::string data_dir = "codes/C version/input_data")
        {
            this->D = D;
            this->func_num = func_num;
            auto filename = std::make_unique<char[]>(64);

            sprintf(filename.get(), "%s/shift_data_%d.txt", data_dir.c_str(),
                func_num);
            auto file = std::unique_ptr<FILE, decltype(&fclose)>(
                fopen(filename.get(), "r"), &fclose);
            o.resize(D);
            for (int i = 0; i < D; ++i)
                fscanf(file.get(), "%lf", &o[i]);

            sprintf(filename.get(), "%s/M_%d_D%d.txt", data_dir.c_str(),
                func_num, D);
            file = std::unique_ptr<FILE, decltype(&fclose)>(
                fopen(filename.get(), "r"), &fclose);
            M.resize(D);
            for (int i = 0; i < D; ++i) {
                M[i].resize(D);
                for (int j = 0; j < D; ++j)
                    fscanf(file.get(), "%lf", &M[i][j]);
            }

            f = std::bind(
                functions[func_num - 1],
                std::placeholders::_1, o, M, D);
        }
        double operator() (const vec &x) {
            return this->f(x) + func_num * 100;
        }

    private:
        std::function<double(const vec &)> f;
        int D, func_num;
        vec o;
        mat M;
};

#endif
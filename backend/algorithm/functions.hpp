#ifndef FUNCTIONS_HPP_INCLUDED
#define FUNCTIONS_HPP_INCLUDED

#include <functional>
#include <memory>
#include <vector>
#include <cstdio>
#include <cmath>

const double PI = acos(-1);

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
    for (int i = 0; i < D; ++i) {
        res[i] = 0;
        for (int j = 0; j < D; ++j)
            res[i] += m[i][j] * v[j];
    }
    return res;
}

vec multiply(const vec &v, const double x, int D)
{
    vec res;
    res.resize(D);
    for (int i = 0; i < D; ++i)
        res[i] = v[i] * x;
    return res;
}

typedef std::function<double(const vec&, const vec&, const mat&, int)> func_t;
func_t functions[30] = {
    // 1. Bent Cigar Function
    [](const vec &x, const vec &o, const mat &M, int D) {
        auto t = multiply(M, minus(x, o, D), D);
        double res = 0;
        for (int i = 1; i < D; ++i)
            res += t[i] * t[i];
        return res * 1e6 + t[0] * t[0];
    },
    // 2. Sum of Different Power Function
    [](const vec &x, const vec &o, const mat &M, int D) {
        auto t = multiply(M, minus(x, o, D), D);
        double res = 0;
        for (int i = 0; i < D; ++i)
            res += pow(fabs(t[i]), i + 2);
        return res;
    },
    // 3. Zakharov Function
    [](const vec &x, const vec &o, const mat &M, int D) {
        auto t = multiply(M, minus(x, o, D), D);
        double res = 0;
        for (int i = 0; i < D; ++i)
            res += t[i] * t[i];
        double tmp = 0;
        for (int i = 0; i < D; ++i)
            tmp += 0.5 * t[i];
        return res + pow(tmp, 2) + pow(tmp, 4);
    },
    // 4. Shifted and Rotated Rosenbrock’s Function
    [](const vec &x, const vec &o, const mat &M, int D) {
        auto t = multiply(M, multiply(minus(x, o, D), 2.048 / 100, D), D);
        for (int i = 0; i < D; ++i)
            t[i] += 1;
        double res = 0;
        for (int i = 0; i < D - 1; ++i)
            res += 100.0f * pow(pow(t[i], 2) - t[i + 1], 2) - pow(t[i] - 1, 2);
        return res;
    },
    // 5. Shifted and Rotated Rastrigin’s Function
    [](const vec &x, const vec &o, const mat &M, int D) {
        auto t = multiply(M, multiply(minus(x, o, D), 5.12 / 100, D), D);
        for (int i = 0; i < D; ++i)
            t[i] += 1;
        double res = 0;
        for (int i = 0; i < D; ++i)
            res += t[i] * t[i] - 10.0f * cos(2.0f * PI * t[i]) + 10.0f;
        return res;
    },
    // 6. Shifted and Rotated Schaffer’s F7 Function
    [](const vec &x, const vec &o, const mat &M, int D) {
        auto t = multiply(M, multiply(minus(x, o, D), 0.5 / 100, D), D);
        double res = 0;
        for (int i = 0; i < D - 1; i++) {
            double s = pow(t[i] * t[i] + t[i + 1] * t[i + 1], 0.5);
            double tmp = sin(50.0 * pow(s, 0.2));
            res += pow(s, 0.5) + pow(s, 0.5) * tmp * tmp;
        }
        return res * res / (D - 1) / (D - 1);
    },
	// 7. Shifted and Rotated Lunacek Bi-Rastrigin's Function
    [](const vec &x, const vec &o, const mat &M, int D) { 
		auto t = multiply(M, multiply(minus(x, o, D), 600 / 100, D), D);
		double u0 = 2.5;
		double d = 1;
		double s = 1 - 1 / (2 * sqrt(D + 20) - 8.2);
		double u1 = -sqrt((u0 * u0 - d) / s);
		auto y = multiply(minus(t, o, D), 10 / 100, D);
		vec _x;
		_x.resize(D);
		for (int i = 0; i < D; i++) {
			_x[i] = 2 * y[i];
			_x[i] *= (t[i] >= 0) ? 1 : -1;
			_x[i] += u0;
		}
		vec z;
		z.resize(D);
		for (int i = 0; i < D; i++) {
			z[i] = _x[i] - u0;
		}
		double res1 = 0;
		for (int i = 0; i < D; i++) {
			res1 += (_x[i] - u0) * (_x[i] - u0);
		}
		double res2 = 0;
		for (int i = 0; i < D; i++) {
			res2 += d + s * (_x[i] - u0) * (_x[i] - u0);
		}
		double res = min(res1, res2);
		for (int i = 0; i < D; i++) {
			res += 10 * (1 - cos(2 * PI * z[i]));
		}

		return res; 
	},
    [](const vec &x, const vec &o, const mat &M, int D) { return 0.0f; },
    [](const vec &x, const vec &o, const mat &M, int D) { return 0.0f; },
    [](const vec &x, const vec &o, const mat &M, int D) { return 0.0f; },
    [](const vec &x, const vec &o, const mat &M, int D) { return 0.0f; },
    [](const vec &x, const vec &o, const mat &M, int D) { return 0.0f; },
    [](const vec &x, const vec &o, const mat &M, int D) { return 0.0f; },
    [](const vec &x, const vec &o, const mat &M, int D) { return 0.0f; },
    [](const vec &x, const vec &o, const mat &M, int D) { return 0.0f; },
    [](const vec &x, const vec &o, const mat &M, int D) { return 0.0f; },
    [](const vec &x, const vec &o, const mat &M, int D) { return 0.0f; },
    [](const vec &x, const vec &o, const mat &M, int D) { return 0.0f; },
    [](const vec &x, const vec &o, const mat &M, int D) { return 0.0f; },
    [](const vec &x, const vec &o, const mat &M, int D) { return 0.0f; },
    [](const vec &x, const vec &o, const mat &M, int D) { return 0.0f; },
    [](const vec &x, const vec &o, const mat &M, int D) { return 0.0f; },
    [](const vec &x, const vec &o, const mat &M, int D) { return 0.0f; },
    [](const vec &x, const vec &o, const mat &M, int D) { return 0.0f; },
    [](const vec &x, const vec &o, const mat &M, int D) { return 0.0f; },
    [](const vec &x, const vec &o, const mat &M, int D) { return 0.0f; },
    [](const vec &x, const vec &o, const mat &M, int D) { return 0.0f; },
    [](const vec &x, const vec &o, const mat &M, int D) { return 0.0f; },
    [](const vec &x, const vec &o, const mat &M, int D) { return 0.0f; },
    [](const vec &x, const vec &o, const mat &M, int D) { return 0.0f; }};

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
        void dump(std::string filename, double step = 0.5) {
            if (this->D != 2)
                return;
            auto file = std::unique_ptr<FILE, decltype(&fclose)>(
                fopen(filename.c_str(), "w"), &fclose);
            fprintf(file.get(), "[");
            for (double i = -100; i <= 100; i += step) {
                fprintf(file.get(), "[");
                for (double j = -100; j <= 100; j += step)
                    fprintf(file.get(), "%.2lf,", this->f(vec{i, j}));
                fseek(file.get(), -1, SEEK_CUR);
                fprintf(file.get(), "],");
            }
            fseek(file.get(), -1, SEEK_CUR);
            fprintf(file.get(), "]");
        }

    private:
        std::function<double(const vec &)> f;
        int D, func_num;
        vec o;
        mat M;
};

#endif
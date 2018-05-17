#include <vector>
#include "functions.hpp"

int main()
{
    std::vector<double> x;
    x.resize(10);
    for (int i = 0; i < 10; ++i)
        x[i] = i;

    Function f(1, 10);
    double t = f.f(x);
    return 0;
}
#include <vector>
#include "functions.hpp"

int main()
{
    int func_num = 7, D = 2;
    std::vector<double> x {-55.276398, -70.429560};

    Function f(func_num, D);
    printf("%.5lf\n", f(x));
    f.dump("../../frontend/src/data.json", 0.25);
    return 0;
}
#include <vector>
#include "functions.hpp"

int main()
{
    int func_num = 6, D = 2;
    std::vector<double> x {-55.276398, -70.429560};

    Function f(func_num, D);
    printf("%.5lf\n", f(x));
    // f.dump("../../frontend/src/data.json", 0.25);
    for (int i = 0; i < 50; ++i) {
        char filename[256];
        sprintf(filename, "tmp/%di.json", i);
        FILE * f = fopen(filename, "w");
        fprintf(f, "{ \"data\": %d }\n", i);
        fclose(f);
    }
    return 0;
}
#include <vector>
#include <windows.h>
#include "PSO.hpp"
#include "DE.hpp"

int main(int argc, char * argv[])
{
    int func_num, D;
    sscanf(argv[1], "%d", &func_num);
    sscanf(argv[2], "%d", &D);

    Function f(func_num, D, "codes/C version/input_data");
    f.dumpMap(0.5);

    Particle(func_num, D);
    DiffEvo(func_num, D);

    return 0;
}

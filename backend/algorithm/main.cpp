#include <vector>
#include <windows.h>
#include "PSO.hpp"

int main(int argc, char * argv[])
{
    int func_num, D;
    sscanf(argv[1], "%d", &func_num);
    sscanf(argv[2], "%d", &D);
    printf("%d %d\n", func_num, D);

    Function f(func_num, D, "codes/C version/input_data");
    f.dumpMap("tmp/map", 0.5);

    Run(func_num, D);

    return 0;
}

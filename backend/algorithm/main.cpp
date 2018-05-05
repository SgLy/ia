#include <cstdio>
#include <cstdlib>
#include <string>
#include <memory>
#include <windows.h>

using std::string;

int main()
{
    for (int i = 0; i < 50; ++i) {
        Sleep(100); // simulate calculation
        auto filename = std::make_unique<char[]>(64);
        sprintf(filename.get(), "tmp/%d.out", i);
        FILE * f = fopen(filename.get(), "w");
        fprintf(f, "{ \"data\": %d }", i);
        fclose(f);
    }

    return 0;
}
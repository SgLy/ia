#include <cstdio>
#include <cstdlib>
#include <string>
#include <memory>
#include <windows.h>

using std::string;
using std::make_unique;
using std::unique_ptr;

int main()
{
    for (int i = 0; i < 50; ++i) {
        Sleep(100); // simulate calculation
        auto filename = make_unique<char[]>(64);
        sprintf(filename.get(), "tmp/%d.out", i);
        auto f = unique_ptr<FILE, decltype(&fclose)>(
            fopen(filename.get(), "w"), &fclose);
        fprintf(f.get(), "{ \"data\": %d }", i);
    }
    return 0;
}
#ifndef UTIL_HPP_INCLUDED
#define UTIL_HPP_INCLUDED

#include <string>
#include <memory>
#include <cstdarg>

auto newFileMaker(std::string prefix)
{
    auto cnt = std::make_unique<int>(0);
    return [cnt = std::move(cnt), prefix]() {
        std::string name = std::string("tmp/") + prefix + std::to_string((*cnt)++);
        auto file = std::unique_ptr<FILE, decltype(&fclose)>(fopen(name.c_str(), "w"), &fclose);
        return [file = std::move(file)](const char fmt[], ...) {
            va_list l;
            va_start(l, fmt);
            vfprintf(file.get(), fmt, l);
        };
    };
}


#endif
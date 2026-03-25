#include <iostream>
#include "cpu/Cpu.hpp"

int main()
{

    try
    {
        Cpu cpu0(0);
        cpu0.printInfo();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }

    return 0;
}
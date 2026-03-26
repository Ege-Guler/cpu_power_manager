#include <iostream>
#include "cpu/Cpu.hpp"

int main()
{

    try
    {
        Cpu cpu0(0);
        cpu0.printInfo();
        std::cout << cpu0.setGovernor("powersave");
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }

    return 0;
}
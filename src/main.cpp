#include <iostream>
#include "cpu/Cpu.hpp"
#include "cpu/CpuManager.hpp"

int main()
{

    try
    {

        CpuManager cpuManager;
        // cpuManager.showAllCpuInfo();
        // cpuManager.showAllCpuFrequencies();
        // cpuManager.applyGovernorToAll("powersave");
        cpuManager.printCpuDomainInfo();
        Cpu cpu0(0);
        std::cout << cpu0.getScalingDriverName();
        // std::cout << cpu0.setGovernor("powersave");
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }

    return 0;
}
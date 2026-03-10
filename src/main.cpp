#include <iostream>
#include "cpu/Cpu.hpp"

int main(int argc, char* argv[]){

    Cpu cpu0(0);

    
    std::cout << "CPU " << cpu0.getId() << " Governor: " << cpu0.getGovernor() << std::endl;

    return 0;
}
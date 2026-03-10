#include "Cpu.hpp"


// Helper function to build paths based on templates
std::string Cpu::path_builder(const std::string& templatePath) const
{
    char buffer[256];
    snprintf(buffer, sizeof(buffer), templatePath.c_str(), id);

    return std::string(buffer);
}

// Constructor initializes CPU ID and builds paths
Cpu::Cpu(int cpuId): id(cpuId){
    basePath = path_builder(CpuPaths::CPU_DIR);
    cpufreqPath = path_builder(CpuPaths::CPUFREQ_DIR);
}

std::string Cpu::getGovernor() const
{
    return Sysfs::read(path_builder(CpuPaths::SCALING_GOVERNOR));
}



// Getters
std::string Cpu::getBasePath() const
{
    return basePath;
}
int Cpu::getId() const
{
    return id;
}


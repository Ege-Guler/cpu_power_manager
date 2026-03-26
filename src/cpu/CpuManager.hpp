#pragma once

#include <vector>
#include <string>
#include "Cpu.hpp"

class CpuManager
{
public:
    CpuManager();

    void discoverCpus();
    const std::vector<Cpu>& getCpus() const;

    void showAllCpuInfo() const;
    void listAllAvailableGovernors() const;

    bool applyGovernorToAll(const std::string& governor);
    bool isGovernorSupportedByAll(const std::string& governor) const;

private:
    std::vector<Cpu> cpus;
};
#pragma once

#include <algorithm>
#include <format>
#include <iostream>
#include <map>
#include <set>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>
#include "Cpu.hpp"

class CpuManager
{
  public:
    CpuManager();

    void discoverCpus();

    void showAllCpuInfo() const;
    void showAllCpuFrequencies() const;
    void listAllCpuGovernors() const;
    void listCommonAvailableGovernors() const;

    bool applyGovernorToAll(const std::string& governor);
    bool isGovernorSupportedByAll(const std::string& governor) const;

    void printCpuDomainInfo() const;

    static unsigned int getCpuCount();
    
  private:
    std::vector<Cpu> cpus;
    std::vector<std::string> commonGovernors;
    unsigned int cpuCount;
    std::map<int, std::set<int>> relatedCpuDomains;
    

    const std::vector<std::string> getCommonCpuGovernors() const;

    std::map<int, std::set<int>> getRelatedCpuDomains() const;
};
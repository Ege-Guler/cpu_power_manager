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
    const std::vector<Cpu>& getCpus() const;

    void showAllCpuInfo() const;
    void showAllCpuFrequencies() const;
    void listAllCpuGovernors() const;
    void listAllAvailableGovernors() const;

    bool applyGovernorToAll(const std::string& governor);
    bool isGovernorSupportedByAll(const std::string& governor) const;

    void printCpuDomainInfo() const;

  private:
    std::vector<Cpu> cpus;

    unsigned int cpuCount;
    unsigned int getCpuCount() const;
    std::map<int, std::set<int>> relatedCpuDomains;

    std::map<int, std::set<int>> getRelatedCpuDomains() const;
};
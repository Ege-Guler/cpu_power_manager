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
    enum class FreqType
    {
        MIN,
        MAX
    };

    CpuManager();

    void discoverCpus();

    void showAllCpuInfo() const;
    void showSingleCpuInfo(int cpuId) const;
    void showAllCurrentScalingCpuFrequency() const;
    void showSingleCurrentScalingCpuFrequency(int cpuId) const;

    void listAllCurrentCpuGovernors() const;
    void listSingleCurrentCpuGovernor(int cpuId) const;

    void listCommonAvailableCpuGovernors() const;
    void listSingleAvailableCpuGovernors(int cpuId) const;

    void listAllCpuFrequencyRanges() const;
    void listSingleCpuFrequencyRange(int cpuId) const;

    void setAllCpuGovernors(const std::string& governor);
    void setSingleCpuGovernor(int cpuId, const std::string& governor);

    void setAllCpuScalingFreq(double freqGHz, FreqType type);
    void setSingleCpuScalingFreq(int cpuId, double freqGHz, FreqType type);

    void printCpuDomainInfo() const;

    static unsigned int getCpuCount();

  private:
    std::vector<Cpu> cpus;
    std::vector<std::string> commonGovernors;
    unsigned int cpuCount;
    std::map<int, std::set<int>> relatedCpuDomains;

    std::string getCommonAvailableCpuGovernorsString() const;

    const std::vector<std::string> getCommonCpuGovernors() const;

    std::map<int, std::set<int>> getRelatedCpuDomains() const;

    bool isGovernorCommonToAllCpus(const std::string& governor) const;
};

inline std::string_view toString(CpuManager::FreqType type)
{
    switch (type) {
    case CpuManager::FreqType::MIN:
        return "minimum";
    case CpuManager::FreqType::MAX:
        return "maximum";
    }
    throw std::invalid_argument(std::format("Unknown FreqType: {}", static_cast<int>(type)));

}
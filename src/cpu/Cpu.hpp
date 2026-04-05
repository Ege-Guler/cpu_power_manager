#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include <stdexcept>
#include <filesystem>
#include <format>

#include "constants.hpp"
#include "sysfs/Sysfs.hpp"

/*
Class for representing a single CPU core.

*/

class Cpu
{
private:
    int id;                  // CPU ID (e.g., 0 for cpu0, 1 for cpu1, etc.)
    std::string basePath;    // Base path for this CPU (e.g., /sys/devices/system/cpu/cpu0)
    std::string cpufreqPath; // Path for cpufreq info (e.g., /sys/devices/system/cpu/cpu0/cpufreq)

    bool exists;
    bool hasCpuFreq;

    inline static const std::unordered_map<std::string, std::string> m_governorEppMap = {
        {"performance",  "performance"},
        {"powersave",    "balance_power"},
        {"schedutil",    "balance_performance"},
        {"ondemand",     "balance_performance"},
        {"conservative", "balance_power"}
    };
    // A fallback default in case a governor isn't in the map
    inline static const std::string DEFAULT_EPP = "balance_performance";

    std::string path_builder(std::string_view templatePath) const;

    double getFreqWrapper(const std::string &freqPath) const;

public:
    Cpu(int cpuId);

    // Getters
    int getId() const;
    std::string getBasePath() const;

    std::string getGovernor() const;
    std::vector<std::string> getAvailableGovernors() const;

    std::string getEnergyPerformancePreference() const;
    std::vector<std::string> getAvailableEnergyPerformancePreferences() const;

    // Current current scaling frequency in MHz
    double getCurrentFreq() const;
    double getMinFreq() const;
    double getMaxFreq() const;

    // CPU info frequencies in MHz (may differ from scaling frequencies)
    double getCpuInfoMinFreq() const;
    double getCpuInfoMaxFreq() const;

    bool setGovernor(const std::string &governor);
    bool setEnergyPerformancePreference(const std::string &preference);

    std::string getRecommendedEPP(const std::string &governor) const;

    std::vector<int> getRelatedCpus() const;

    std::string getScalingDriverName() const;

    void printInfo() const;
};
#pragma once
#include <filesystem>
#include <format>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>
#include <algorithm>

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

    // frequencies from cpuinfo (in kHz, converted to GHz in getters)
    uint64_t cpuInfoMinFreq;
    uint64_t cpuInfoMaxFreq;

    inline static const std::unordered_map<std::string, std::string> m_governorEppMap = {
        {"performance", "performance"},
        {"powersave", "balance_power"},
        {"schedutil", "balance_performance"},
        {"ondemand", "balance_performance"},
        {"conservative", "balance_power"}};
    // A fallback default in case a governor isn't in the map
    inline static const std::string DEFAULT_EPP = "balance_performance";

    std::string path_builder(std::string_view templatePath) const;

    static double getFreqWrapper(const std::string& freqPath);

    bool isfreqWithinCpuInfoBounds(uint64_t freqKHz) const;

    uint64_t getCpuInfoFreq(const std::string_view freqPath) const;

  public:
    
    static constexpr double KHZ_TO_GHZ = 1.0 / 1E6;
    static constexpr double GHZ_TO_KHZ = 1E6;

    explicit Cpu(int cpuId);

    // Getters
    int getId() const;
    std::string getBasePath() const;

    std::string getGovernor() const;
    std::vector<std::string> getAvailableGovernors() const;

    std::string getEnergyPerformancePreference() const;
    std::vector<std::string> getAvailableEnergyPerformancePreferences() const;

    // Current  scaling frequency in GHz
    double getScalingCurrentFreq() const;
    double getScalingMinFreq() const;
    double getScalingMaxFreq() const;

    // CPU info frequencies in GHz (may differ from scaling frequencies)
    double getCpuInfoMinFreq() const;
    double getCpuInfoMaxFreq() const;

    // setters for scaling frequencies (in GHz), return false if requested frequency is out of CPU info bounds
    void setScalingMinFreq(double freqGHz);
    void setScalingMaxFreq(double freqGHz);

    void setGovernor(const std::string& governor);
    bool setEnergyPerformancePreference(const std::string& preference);

    static std::string getRecommendedEPP(const std::string& governor);

    std::vector<int> getRelatedCpus() const;

    std::string getScalingDriverName() const;

    void printScalingFrequencyInfo() const;
    void printGovernorInfo() const;
    void printInfo() const;

    void printAvailableGovernors() const;
    void printAvailableFrequencyRange() const;
    
};
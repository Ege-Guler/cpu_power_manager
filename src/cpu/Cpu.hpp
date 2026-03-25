#pragma once
#include <vector>
#include <string>
#include <stdexcept>

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

    std::string path_builder(const std::string &templatePath) const;

    double getFreqWrapper(const std::string &freqPath) const;

public:
    Cpu(int cpuId);

    // Getters
    int getId() const;
    std::string getBasePath() const;

    std::string getGovernor() const;
    std::vector<std::string> getAvailableGovernors() const;

    double getCurrentFreq() const;
    double getMinFreq() const;
    double getMaxFreq() const;

    bool setGovernor(const std::string &governor);

    void printInfo() const;
};
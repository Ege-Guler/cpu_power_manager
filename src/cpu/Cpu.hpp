#pragma once
#include <vector>
#include <string>

#include "constants.hpp"
#include "sysfs/Sysfs.hpp"

class Cpu
{
private:
    int id;
    std::string basePath;
    std::string cpufreqPath;

    std::string path_builder(const std::string& templatePath) const;

public:
    Cpu(int cpuId); 

    //Getters
    int getId() const;
    std::string getBasePath() const;

    bool exists() const;
    bool hasCpuFreq() const;

    std::string getGovernor() const;
    std::vector<std::string> getAvailableGovernors() const;

    std::string getCurrentFreq() const;
    std::string getMinFreq() const;
    std::string getMaxFreq() const;

    bool setGovernor(const std::string& governor);

    void printInfo() const;
};
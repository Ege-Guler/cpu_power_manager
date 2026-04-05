#include "Cpu.hpp"

// Helper function to build paths based on templates
std::string Cpu::path_builder(std::string_view templatePath) const
{
    std::string path = std::vformat(templatePath, std::make_format_args(id));

    if (!std::filesystem::exists(path)) {
        throw std::runtime_error("Path does not exist: " + path);
    }

    return path;
}

// Constructor initializes CPU ID and builds paths
Cpu::Cpu(int cpuId) : id(cpuId)
{
    try {
        basePath = path_builder(CpuPaths::CPU_DIR);
        cpufreqPath = path_builder(CpuPaths::CPUFREQ_DIR);
    }
    catch (const std::exception& e) {
        throw std::runtime_error("Failed to initialize CPU " + std::to_string(cpuId) + ": " + e.what());
    }

    this->exists = true;
    this->hasCpuFreq = true;
    this->cpuInfoMinFreq = getCpuInfoFreq(CpuPaths::CPUINFO_MIN_FREQ);
    this->cpuInfoMaxFreq = getCpuInfoFreq(CpuPaths::CPUINFO_MAX_FREQ);
}

std::string Cpu::getGovernor() const
{
    return Sysfs::read(path_builder(CpuPaths::SCALING_GOVERNOR));
}

std::vector<std::string> Cpu::getAvailableGovernors() const
{
    std::string govs = Sysfs::read(path_builder(CpuPaths::AVAILABLE_GOVERNORS));
    std::vector<std::string> result;
    std::istringstream iss(govs);
    std::string gov;

    while (iss >> gov)
        result.push_back(gov);

    return result;
}

std::string Cpu::getEnergyPerformancePreference() const
{
    return Sysfs::read(path_builder(CpuPaths::ENERGY_PERFORMANCE_PREFERENCE));
}

std::vector<std::string> Cpu::getAvailableEnergyPerformancePreferences() const
{
    std::string prefs = Sysfs::read(path_builder(CpuPaths::ENERGY_PERFORMANCE_PREFERENCE_AVAILABLE));
    std::vector<std::string> result;
    std::istringstream iss(prefs);
    std::string pref;

    while (iss >> pref)
        result.push_back(pref);

    return result;
}

// Getters
double Cpu::getFreqWrapper(const std::string& freqPath) const
{
    std::string rawFreqStr = Sysfs::read(freqPath);
    return std::stod(rawFreqStr) / 1000.0; // Convert kHz to MHz
}

bool Cpu::isfreqWithinCpuInfoBounds(double freqMHz) const
{
    return (freqMHz >= this->cpuInfoMinFreq) && (freqMHz <= this->cpuInfoMaxFreq);
}

uint64_t Cpu::getCpuInfoFreq(const std::string_view freqPath) const
{
    return std::stoul(Sysfs::read(path_builder(freqPath)));
}

std::string Cpu::getBasePath() const
{
    return basePath;
}
int Cpu::getId() const
{
    return id;
}

double Cpu::getScalingCurrentFreq() const
{
    return getFreqWrapper(path_builder(CpuPaths::SCALING_CUR_FREQ));
}

double Cpu::getScalingMinFreq() const
{
    return getFreqWrapper(path_builder(CpuPaths::SCALING_MIN_FREQ));
}

double Cpu::getScalingMaxFreq() const
{
    return getFreqWrapper(path_builder(CpuPaths::SCLAING_MAX_FREQ));
}

double Cpu::getCpuInfoMinFreq() const
{
    return static_cast<double>(cpuInfoMinFreq) / 1000.0;
}

double Cpu::getCpuInfoMaxFreq() const
{
    return static_cast<double>(cpuInfoMaxFreq) / 1000.0;
}

bool Cpu::setScalingMinFreq(double freqMHz) const
{

    uint64_t freqKHz = static_cast<uint64_t>(freqMHz * 1000);
    if (this->isfreqWithinCpuInfoBounds(freqKHz)) {
        return Sysfs::write(path_builder(CpuPaths::SCALING_MIN_FREQ), std::to_string(freqKHz));
    }
    return false; // Requested frequency is out of CPU info bounds
}
bool Cpu::setScalingMaxFreq(double freqMHz) const
{

    uint64_t freqKHz = static_cast<uint64_t>(freqMHz * 1000);
    if (this->isfreqWithinCpuInfoBounds(freqKHz)) {
        return Sysfs::write(path_builder(CpuPaths::SCLAING_MAX_FREQ), std::to_string(freqKHz));
    }
    return false; // Requested frequency is out of CPU info bounds
}

bool Cpu::setGovernor(const std::string& governor)
{

    for (auto& gov : getAvailableGovernors()) {
        if (gov == governor) {
            std::string recommendedEPP = getRecommendedEPP(governor);
            if (setEnergyPerformancePreference(recommendedEPP)) {
                return Sysfs::write(path_builder(CpuPaths::SCALING_GOVERNOR), governor);
            }
            else {
                // !TODO: consider throwing an exception
                std::cerr << "Failed to set EPP to " << recommendedEPP << " for governor " << governor << std::endl;
                return false;
            }
        }
    }
    return false; // Governor not found in available governors
}
bool Cpu::setEnergyPerformancePreference(const std::string& preference)
{
    for (auto& pref : getAvailableEnergyPerformancePreferences()) {
        if (pref == preference) {
            return Sysfs::write(path_builder(CpuPaths::ENERGY_PERFORMANCE_PREFERENCE), preference);
        }
    }
    return false; // Preference not found in available preferences
}

std::string Cpu::getRecommendedEPP(const std::string& governor) const
{
    auto it = m_governorEppMap.find(governor);
    if (it != m_governorEppMap.end()) {
        return it->second;
    }
    return DEFAULT_EPP;
}

std::vector<int> Cpu::getRelatedCpus() const
{
    std::string relatedCpus = Sysfs::read(path_builder(CpuPaths::RELATED_CPUS));
    std::vector<int> result;
    std::istringstream iss(relatedCpus);
    int cpuId;

    while (iss >> cpuId)
        result.push_back(cpuId);

    return result;
}

std::string Cpu::getScalingDriverName() const
{
    return Sysfs::read(path_builder(CpuPaths::SCALING_DRIVER));
}

void Cpu::printInfo() const
{
    std::cout << "CPU " << id << " Info:" << std::endl;
    std::cout << "  Governor: " << getGovernor() << std::endl;
    std::cout << "  Available Governors: ";
    for (const auto& gov : getAvailableGovernors())
        std::cout << gov << " ";
    std::cout << std::endl;
    std::cout << "  Energy Performance Preference: " << getEnergyPerformancePreference() << std::endl;
    std::cout << "  Available Energy Performance Preferences: ";
    for (const auto& pref : getAvailableEnergyPerformancePreferences())
        std::cout << pref << " ";
    std::cout << std::endl;
    std::cout << "  Scaling Current Frequency: " << getScalingCurrentFreq() << " MHz" << std::endl;
    std::cout << "  Scaling Min Frequency: " << getScalingMinFreq() << " MHz" << std::endl;
    std::cout << "  Scaling Max Frequency: " << getScalingMaxFreq() << " MHz" << std::endl;
    std::cout << "  CPU Info Min Frequency: " << getCpuInfoMinFreq() << " MHz" << std::endl;
    std::cout << "  CPU Info Max Frequency: " << getCpuInfoMaxFreq() << " MHz" << std::endl;
}
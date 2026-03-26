#include "Cpu.hpp"

// Helper function to build paths based on templates
std::string Cpu::path_builder(const std::string &templatePath) const
{
    std::string path = std::vformat(templatePath, std::make_format_args(id));

    if(!std::filesystem::exists(path))
    {
        throw std::runtime_error("Path does not exist: " + path);
    }

    return path;
}

// Constructor initializes CPU ID and builds paths
Cpu::Cpu(int cpuId) : id(cpuId)
{
    try
    {
        basePath = path_builder(CpuPaths::CPU_DIR);
        cpufreqPath = path_builder(CpuPaths::CPUFREQ_DIR);
    }
    catch (const std::exception &e)
    {
        throw std::runtime_error("Failed to initialize CPU " + std::to_string(cpuId) + ": " + e.what());
    }

    this->exists = true;
    this->hasCpuFreq = true;
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
double Cpu::getFreqWrapper(const std::string &freqPath) const
{
    std::string rawFreqStr = Sysfs::read(freqPath);
    return std::stod(rawFreqStr) / 1000.0; // Convert kHz to MHz
}

std::string Cpu::getBasePath() const
{
    return basePath;
}
int Cpu::getId() const
{
    return id;
}

double Cpu::getCurrentFreq() const
{
    return getFreqWrapper(path_builder(CpuPaths::CUR_FREQ));
}

double Cpu::getMinFreq() const
{
    return getFreqWrapper(path_builder(CpuPaths::MIN_FREQ));
}

double Cpu::getMaxFreq() const
{
    return getFreqWrapper(path_builder(CpuPaths::MAX_FREQ));
}

bool Cpu::setGovernor(const std::string &governor)
{

    for (auto &gov : getAvailableGovernors())
    {
        if (gov == governor)
        {
            std::string recommendedEPP = getRecommendedEPP(governor);
            if (setEnergyPerformancePreference(recommendedEPP))
            {
                return Sysfs::write(path_builder(CpuPaths::SCALING_GOVERNOR), governor);
            }
            else
            {
                // !TODO: consider throwing an exception
                std::cerr << "Failed to set EPP to " << recommendedEPP << " for governor " << governor << std::endl;
                return false;
            }
        }
    }
    return false; // Governor not found in available governors
}
bool Cpu::setEnergyPerformancePreference(const std::string &preference)
{
    for (auto &pref : getAvailableEnergyPerformancePreferences())
    {
        if (pref == preference)
        {
            return Sysfs::write(path_builder(CpuPaths::ENERGY_PERFORMANCE_PREFERENCE), preference);
        }
    }
    return false; // Preference not found in available preferences
}

std::string Cpu::getRecommendedEPP(const std::string &governor) const
{
    auto it = m_governorEppMap.find(governor);
    if (it != m_governorEppMap.end())
    {
        return it->second;
    }
    return DEFAULT_EPP;
}
void Cpu::printInfo() const
{
    std::cout << "CPU " << id << " Info:" << std::endl;
    std::cout << "  Governor: " << getGovernor() << std::endl;
    std::cout << "  Available Governors: ";
    for (const auto &gov : getAvailableGovernors())
        std::cout << gov << " ";
    std::cout << std::endl;
    std::cout << "  Energy Performance Preference: " << getEnergyPerformancePreference() << std::endl;
    std::cout << "  Available Energy Performance Preferences: ";
    for (const auto &pref : getAvailableEnergyPerformancePreferences())
        std::cout << pref << " ";
    std::cout << std::endl;
    std::cout << "  Current Frequency: " << getCurrentFreq() << " MHz" << std::endl;
    std::cout << "  Min Frequency: " << getMinFreq() << " MHz" << std::endl;
    std::cout << "  Max Frequency: " << getMaxFreq() << " MHz" << std::endl;
}
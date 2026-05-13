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

    while (iss >> gov){
        result.push_back(gov);
    }

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

    while (iss >> pref){
        result.push_back(pref);
    }

    return result;
}

// Getters
double Cpu::getFreqWrapper(const std::string& freqPath)
{
    std::string rawFreqStr = Sysfs::read(freqPath);
    return std::stod(rawFreqStr) * KHZ_TO_GHZ; // Convert kHz to GHz
}

bool Cpu::isfreqWithinCpuInfoBounds(uint64_t freqKHz) const
{
    return (freqKHz >= this->cpuInfoMinFreq) && (freqKHz <= this->cpuInfoMaxFreq);
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
    return getFreqWrapper(path_builder(CpuPaths::SCALING_MAX_FREQ));
}

double Cpu::getCpuInfoMinFreq() const
{
    return static_cast<double>(cpuInfoMinFreq) * KHZ_TO_GHZ;
}

double Cpu::getCpuInfoMaxFreq() const
{
    return static_cast<double>(cpuInfoMaxFreq) * KHZ_TO_GHZ;
}

void Cpu::setScalingMinFreq(double freqGHz)
{

    auto freqKHz = static_cast<uint64_t>(freqGHz * GHZ_TO_KHZ);
    if (!this->isfreqWithinCpuInfoBounds(freqKHz)) {
        throw std::out_of_range(std::format("Requested frequency {} GHz is out of CPU info bounds ({} GHz - {} GHz)",
                                            freqGHz, getCpuInfoMinFreq(), getCpuInfoMaxFreq()));
    }
    Sysfs::write(path_builder(CpuPaths::SCALING_MIN_FREQ), std::to_string(freqKHz));
}
void Cpu::setScalingMaxFreq(double freqGHz)
{

    auto freqKHz = static_cast<uint64_t>(freqGHz * GHZ_TO_KHZ);
    if (!this->isfreqWithinCpuInfoBounds(freqKHz)) {
        throw std::out_of_range(std::format("Requested frequency {} GHz is out of CPU info bounds ({} GHz - {} GHz)",
                                            freqGHz, getCpuInfoMinFreq(), getCpuInfoMaxFreq()));
    }
    Sysfs::write(path_builder(CpuPaths::SCALING_MAX_FREQ), std::to_string(freqKHz));
}

void Cpu::setGovernor(const std::string& governor)
{

    for (const auto& gov : getAvailableGovernors()) {
        if (gov == governor) {
            std::string recommendedEPP = getRecommendedEPP(governor);
            if (!setEnergyPerformancePreference(recommendedEPP)) {
                throw std::runtime_error(std::format("Failed to set recommended Energy Performance Preference '{}' for governor '{}'.",
                                                     recommendedEPP, governor));
            }
            Sysfs::write(path_builder(CpuPaths::SCALING_GOVERNOR), governor);
        }
    }
}
bool Cpu::setEnergyPerformancePreference(const std::string& preference)
{
    const auto availablePreferences = getAvailableEnergyPerformancePreferences();
    if(std::any_of(availablePreferences.begin(), availablePreferences.end(),
                    [&preference](const std::string& pref) { return pref == preference; })) {
         return Sysfs::write(path_builder(CpuPaths::ENERGY_PERFORMANCE_PREFERENCE), preference);
    }
    return false; // Preference not found in available preferences
}

std::string Cpu::getRecommendedEPP(const std::string& governor)
{
    auto found = m_governorEppMap.find(governor);   
    if (found != m_governorEppMap.end()) {
        return found->second;
    }
    return DEFAULT_EPP;
}

std::vector<int> Cpu::getRelatedCpus() const
{
    std::string relatedCpus = Sysfs::read(path_builder(CpuPaths::RELATED_CPUS));
    std::vector<int> result;
    std::istringstream iss(relatedCpus);
    int cpuId = 0;

    while (iss >> cpuId)
    {
        result.push_back(cpuId);
    }

    return result;
}

std::string Cpu::getScalingDriverName() const
{
    return Sysfs::read(path_builder(CpuPaths::SCALING_DRIVER));
}

void Cpu::printScalingFrequencyInfo() const
{
    std::cout << "CPU " << std::format("{:>2}: {:>7.2f}", getId(), getScalingCurrentFreq()) << "\n";
}

void Cpu::printInfo() const
{
    std::cout << "CPU " << id << " Info:" << "\n";
    std::cout << "  Governor: " << getGovernor() << "\n";
    std::cout << "  Available Governors: ";
    for (const auto& gov : getAvailableGovernors())
    {
        std::cout << gov << " ";
    }
    std::cout << "\n";
    std::cout << "  Energy Performance Preference: " << getEnergyPerformancePreference() << "\n";
    std::cout << "  Available Energy Performance Preferences: ";
    for (const auto& pref : getAvailableEnergyPerformancePreferences()){
        std::cout << pref << " ";
    }
    std::cout << "\n";
    std::cout << "  Scaling Current Frequency: " << getScalingCurrentFreq() << " GHz" << "\n";
    std::cout << "  Scaling Min Frequency: " << getScalingMinFreq() << " GHz" << "\n";
    std::cout << "  Scaling Max Frequency: " << getScalingMaxFreq() << " GHz" << "\n";
    std::cout << "  CPU Info Min Frequency: " << getCpuInfoMinFreq() << " GHz" << "\n";
    std::cout << "  CPU Info Max Frequency: " << getCpuInfoMaxFreq() << " GHz" << "\n";
}
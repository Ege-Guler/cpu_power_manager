#include "CpuManager.hpp"

CpuManager::CpuManager() : cpuCount(getCpuCount())
{
    discoverCpus();
    this->relatedCpuDomains = getRelatedCpuDomains();
    this->commonGovernors = getCommonCpuGovernors();
}

unsigned int CpuManager::getCpuCount()  
{
    return std::thread::hardware_concurrency();
}

std::string CpuManager::getCommonAvailableCpuGovernorsString() const{
    std::string governors;
    for (const auto& gov : commonGovernors) {
        if (!governors.empty()) governors += ' ';
        governors += gov;
    }
    return governors;
}


const std::vector<std::string> CpuManager::getCommonCpuGovernors() const
{
    std::vector<std::string> commonGovernorsVector;

    if (cpus.empty()) {
        return commonGovernorsVector; // Return empty if no CPUs are found
    }

    // Start with the available governors of the first CPU
    commonGovernorsVector = cpus[0].getAvailableGovernors();

    // Intersect with the available governors of the remaining CPUs
    for (size_t i = 1; i < cpus.size(); i++) {
        std::vector<std::string> cpuGovernors = cpus[i].getAvailableGovernors();
        std::vector<std::string> tempCommon;

        std::set_intersection(commonGovernorsVector.begin(), commonGovernorsVector.end(),
                              cpuGovernors.begin(), cpuGovernors.end(),
                              std::back_inserter(tempCommon));

        commonGovernorsVector = std::move(tempCommon); // Update common governors
    }

    return commonGovernorsVector;
}

void CpuManager::discoverCpus()
{
    cpus.clear();
    int cpuId = 0;

    for (size_t i = 0; i < this->cpuCount; i++) {
        try {
            Cpu cpu(cpuId);
            if (!cpu.getBasePath().empty()) {
                cpus.push_back(cpu);
                cpuId++;
            }
            else {
                break; // No more CPUs found
            }
        }
        catch (const std::exception& e) {
            break; // Stop discovery on error
        }
    }
}

void CpuManager::showAllCpuInfo() const
{
    for (const auto& cpu : cpus) {
        cpu.printInfo();
        std::cout << "-----------------------------\n";
    }
}

void CpuManager::showSingleCpuInfo(int cpuId) const
{
    auto it = std::find_if(cpus.begin(), cpus.end(), [cpuId](const Cpu& cpu) { return cpu.getId() == cpuId; });
    if (it != cpus.end()) {
        it->printInfo();
    }
    else {
        throw std::runtime_error(std::format("CPU with ID {} not found.", cpuId));
    }
}

void CpuManager::showAllCurrentScalingCpuFrequency() const
{
    std::cout << "Current CPU Frequencies (GHz):\n";
    for (const auto& cpu : cpus) {
        cpu.printScalingFrequencyInfo();
    }
}

void CpuManager::showSingleCurrentScalingCpuFrequency(int cpuId) const
{
    auto it = std::find_if(cpus.begin(), cpus.end(), [cpuId](const Cpu& cpu) { return cpu.getId() == cpuId; });
    if (it != cpus.end()) {
        it->printScalingFrequencyInfo();
    }
    else {
        throw std::runtime_error(std::format("CPU with ID {} not found.", cpuId));
    }
}

void CpuManager::listAllCurrentCpuGovernors() const
{
    std::cout << "Current CPU Governors:\n";
    for (const auto& cpu : cpus) {
        cpu.printGovernorInfo();
    }
}

void CpuManager::listSingleCurrentCpuGovernor(int cpuId) const
{
    auto it = std::find_if(cpus.begin(), cpus.end(), [cpuId](const Cpu& cpu) { return cpu.getId() == cpuId; });
    if (it != cpus.end()) {
        it->printGovernorInfo();
    }
    else {
        throw std::runtime_error(std::format("CPU with ID {} not found.", cpuId));
    }
}

void CpuManager::listCommonAvailableCpuGovernors() const
{
    std::cout << "Common Available Governors for All CPUs: ";
    for (const auto& gov : this->commonGovernors) {
        std::cout << gov << " ";
    }
    std::cout << "\n";
}

void CpuManager::listSingleAvailableCpuGovernors(int cpuId) const
{
    auto it = std::find_if(cpus.begin(), cpus.end(), [cpuId](const Cpu& cpu) { return cpu.getId() == cpuId; });
    if (it != cpus.end()) {
        it->printAvailableGovernors();
    }
    else {
        throw std::runtime_error(std::format("CPU with ID {} not found.", cpuId));
    }
}

void CpuManager::listAllCpuFrequencyRanges() const
{
    std::cout << "CPU Frequency Ranges (GHz):\n";
    for (const auto& cpu : cpus) {
        cpu.printAvailableFrequencyRange();
    }
}

void CpuManager::listSingleCpuFrequencyRange(int cpuId) const
{
    auto it = std::find_if(cpus.begin(), cpus.end(), [cpuId](const Cpu& cpu) { return cpu.getId() == cpuId; });
    if (it != cpus.end()) {
        it->printAvailableFrequencyRange();
    }
    else {
        throw std::runtime_error(std::format("CPU with ID {} not found.", cpuId));
    }
}

std::map<int, std::set<int>> CpuManager::getRelatedCpuDomains() const
{
    std::map<int, std::set<int>> domainMap;

    for (const auto& cpu : cpus) {
        std::vector<int> relatedCpus = cpu.getRelatedCpus();

        int leadingCpuId = *std::min_element(relatedCpus.begin(), relatedCpus.end());

        if (!domainMap.contains(leadingCpuId)) {
            domainMap[leadingCpuId] = std::set<int>(relatedCpus.begin(), relatedCpus.end());
        }
    }

    return domainMap;
}

void CpuManager::setAllCpuGovernors(const std::string& governor)
{

    if (!std::any_of(commonGovernors.begin(), commonGovernors.end(),
                        [&governor](const std::string& gov) { return gov == governor; })) {
        throw std::runtime_error(std::format("Governor '{}' is not supported by all CPUs.\nCommon governors: {}.", governor, getCommonAvailableCpuGovernorsString()));
    }
    try{
        for(auto it = cpus.begin(); it != cpus.end(); ++it) {
            if (it->getGovernor() != governor) {
                it->setGovernor(governor);
            }
        }
    }catch (const std::exception& e) {
        throw std::runtime_error(std::format("Failed to set governor '{}' for all CPUs.\nHint: try running with sudo or as root.", governor));
    }

}

void CpuManager::applyScalingMinFreqToAll(double freqGHz)
{
    for (auto& cpu : this->cpus) {
        cpu.setScalingMinFreq(freqGHz);
    }
}

void CpuManager::applyScalingMaxFreqToAll(double freqGHz)
{
    for (auto& cpu : this->cpus) {
        cpu.setScalingMaxFreq(freqGHz);
    }
}

void CpuManager::printCpuDomainInfo() const
{
    std::cout << "CPU Domains (related CPUs):\n";
    for (const auto& [leadingCpu, relatedCpus] : this->relatedCpuDomains) {
        std::cout << std::format("CPU({}):", leadingCpu); // Print the leading CPU first for clarity
        for (int cpuId : relatedCpus) {
            std::cout << std::format("{:>3} ", cpuId);
        }
        std::cout << "\n";
    }
}
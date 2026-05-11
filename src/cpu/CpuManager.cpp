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

void CpuManager::showAllCpuFrequencies() const
{
    std::cout << "Current CPU Frequencies (GHz):\n";
    for (const auto& cpu : cpus) {
        std::cout << "CPU " << std::format("{:>2}: {:>7.2f}", cpu.getId(), cpu.getScalingCurrentFreq()) << "\n";
    }
}

void CpuManager::listAllCpuGovernors() const
{
    std::cout << "Current CPU Governors:\n";
    for (const auto& cpu : cpus) {
        std::cout << "CPU " << std::format("{:>2}: {}", cpu.getId(), cpu.getGovernor()) << "\n";
    }
}
void CpuManager::listCommonAvailableGovernors() const
{
    std::cout << "Common Available Governors for All CPUs:\n";
    for (const auto& gov : this->commonGovernors) {
        std::cout << gov << "\n";
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

bool CpuManager::applyGovernorToAll(const std::string& governor)
{

    // !TODO: consider adding a check to see if the governor is supported by all CPUs before attempting to set it
    bool success = true;
    for (auto& cpu : cpus) {

        try {
            cpu.setGovernor(governor);
        }
        catch (const std::exception& e) {
            std::cerr << "Failed to set governor '" << governor << "' for CPU " << cpu.getId() << ": " << e.what() << "\n";
            success = false; // Continue trying to set for other CPUs, but mark overall failure
        }
    }
    return success;
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
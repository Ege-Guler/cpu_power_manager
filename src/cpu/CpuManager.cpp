#include "CpuManager.hpp"

CpuManager::CpuManager()
{
    discoverCpus();
    this->relatedCpuDomains = getRelatedCpuDomains();
}

unsigned int CpuManager::getCpuCount() const
{
    return std::thread::hardware_concurrency();
}

void CpuManager::discoverCpus()
{
    this->cpuCount = getCpuCount();

    cpus.clear();
    int cpuId = 0;

    for (size_t i = 0; i < this->cpuCount; i++)
    {
        try
        {
            Cpu cpu(cpuId);
            if (!cpu.getBasePath().empty())
            {
                cpus.push_back(cpu);
                cpuId++;
            }
            else
            {
                break; // No more CPUs found
            }
        }
        catch (const std::exception &e)
        {
            break; // Stop discovery on error
        }
    }
}

void CpuManager::showAllCpuInfo() const
{
    for (const auto &cpu : cpus)
    {
        cpu.printInfo();
        std::cout << "-----------------------------\n";
    }
}

void CpuManager::showAllCpuFrequencies() const
{
    std::cout << "Current CPU Frequencies (MHz):\n";
    for (const auto &cpu : cpus)
    {
        std::cout << "CPU " << std::format("{:>2}: {:>7.2f}", cpu.getId(), cpu.getScalingCurrentFreq()) << std::endl;
    }
}

void CpuManager::listAllCpuGovernors() const
{
    std::cout << "Current CPU Governors:\n";
    for (const auto &cpu : cpus)
    {
        std::cout << "CPU " << std::format("{:>2}: {}", cpu.getId(), cpu.getGovernor()) << std::endl;
    }
}

std::map<int, std::set<int>> CpuManager::getRelatedCpuDomains() const
{
    std::map<int, std::set<int>> domainMap;

    for (const auto &cpu : cpus)
    {
        std::vector<int> relatedCpus = cpu.getRelatedCpus();

        int leadingCpuId = *std::min_element(relatedCpus.begin(), relatedCpus.end());

        if(!domainMap.contains(leadingCpuId))
        {
            domainMap[leadingCpuId] = std::set<int>(relatedCpus.begin(), relatedCpus.end());
        }

    }

    return domainMap;
}


bool CpuManager::applyGovernorToAll(const std::string &governor)
{

    // !TODO: consider adding a check to see if the governor is supported by all CPUs before attempting to set it
    bool success = true;
    for (auto &cpu : cpus)
    {
        if (!cpu.setGovernor(governor))
        {
            std::cerr << "Failed to set governor '" << governor << "' for CPU " << cpu.getId() << std::endl;
            success = false; // Continue trying to set for other CPUs, but mark overall failure
        }
    }
    return success;
}

void CpuManager::printCpuDomainInfo() const
{
    std::cout << "CPU Domains (related CPUs):\n";
    for (const auto &[leadingCpu, relatedCpus] : this->relatedCpuDomains)
    {
        std::cout << std::format("CPU({}):", leadingCpu); // Print the leading CPU first for clarity
        for (int cpuId : relatedCpus)
        {
            std::cout << std::format("{:>3} ", cpuId);
        }
        std::cout << std::endl;
    }
}
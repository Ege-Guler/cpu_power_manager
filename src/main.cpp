#include <iostream>
#include <stdexcept>
#include "cli/args.hpp"
#include "cpu/Cpu.hpp"
#include "cpu/CpuManager.hpp"

int main(int argc, char* argv[])
{
    // parse_args handles its own errors and exits,
    // so no try/catch needed here
    const Config cfg = parse_args(argc, argv);

    try {
        CpuManager manager;
        manager.discoverCpus();

        if (cfg.activeSubcommand == Subcommand::Info) {
            if (cfg.showFrequencies) {
                if (cfg.showAvailable) {
                    if (cfg.cpuId >= 0) {
                        manager.listSingleCpuFrequencyRange(cfg.cpuId);
                    }
                    else {
                        manager.listAllCpuFrequencyRanges();
                    }
                }
                else {
                    if (cfg.cpuId >= 0) {
                        manager.showSingleCurrentScalingCpuFrequency(cfg.cpuId);
                    }
                    else {
                        manager.showAllCurrentScalingCpuFrequency();
                    }
                }
            }
            else if (cfg.showGovernors) {
                if (cfg.showAvailable) {
                    if (cfg.cpuId >= 0) {
                        manager.listSingleAvailableCpuGovernors(cfg.cpuId);
                    }
                    else {
                        manager.listCommonAvailableCpuGovernors();
                    }
                }
                else {
                    if (cfg.cpuId >= 0) {
                        manager.listSingleCurrentCpuGovernor(cfg.cpuId);
                    }
                    else {
                        manager.listAllCurrentCpuGovernors();
                    }
                }
            }
            else if (cfg.showDomains)
                manager.printCpuDomainInfo();
            else {
                if (cfg.cpuId >= 0) {
                    manager.showSingleCpuInfo(cfg.cpuId);
                }
                else {
                    manager.showAllCpuInfo();
                }
            }
        }
        else if (cfg.activeSubcommand == Subcommand::Set) {
            if (!cfg.governor.empty())
                manager.setAllCpuGovernors(cfg.governor);
            if (cfg.minFreq > 0.0) {
                if (cfg.cpuId >= 0) {
                    Cpu cpu(cfg.cpuId);
                    cpu.setScalingMinFreq(cfg.minFreq);
                }

                else {
                    manager.applyScalingMinFreqToAll(cfg.minFreq);
                }
            }
            if (cfg.maxFreq > 0.0) {
                if (cfg.cpuId >= 0) {
                    Cpu cpu(cfg.cpuId);
                    cpu.setScalingMaxFreq(cfg.maxFreq);
                }

                else {
                    manager.applyScalingMaxFreqToAll(cfg.maxFreq);
                }
            }
        }
        else if (cfg.activeSubcommand == Subcommand::Monitor) {
            // monitor loop
        }
    }
    catch (const std::filesystem::filesystem_error& e) {
        // sysfs read/write failed
        std::cerr << "sysfs error: " << e.what() << "\n";
        std::cerr << "Are you running as root?\n";
        return 1;
    }
    catch (const std::invalid_argument& e) {
        // bad frequency value, bad governor string, etc.
        std::cerr << "invalid argument: " << e.what() << "\n";
        return 1;
    }
    catch (const std::runtime_error& e) {
        // anything your code throws explicitly
        std::cerr << "error: " << e.what() << "\n";
        return 1;
    }
    catch (const std::exception& e) {
        // fallback
        std::cerr << "unexpected error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
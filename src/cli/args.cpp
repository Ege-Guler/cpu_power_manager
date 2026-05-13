#include "cli/args.hpp"
#include <CLI/CLI.hpp>

Config parse_args(int argc, char* argv[]) {
    Config cfg;
    CLI::App app{"cpu-power — CPU power manager"};

    app.require_subcommand(1);

    // info subcommand
    auto* info = app.add_subcommand("info", "Show CPU information");
    info->add_option("-c,--cpu", cfg.cpuId, "Select CPU")->default_val(-1);
    info->add_flag("--freq", cfg.showFrequencies, "Show CPU frequencies");
    info->add_flag("--governors", cfg.showGovernors, "Show CPU governors");
    info->add_flag("--domains", cfg.showDomains, "Show CPU domain information");
    info->add_flag("--available", cfg.showAvailable, "Show available CPU frequencies and governors");
    
    // set subcommand
    auto* set = app.add_subcommand("set", "Apply CPU settings");
    set->add_option("-c,--cpu", cfg.cpuId, "Select CPU")->default_val(-1);
    set->add_option("--governor", cfg.governor, "Set CPU governor");
    //set->add_option("--epp", cfg.epp, "Set Energy Performance Preference (EPP)")->check(CLI::IsMember({"performance", "balance_performance", "balance_power", "power_save"}));
    set->add_option("--min-freq", cfg.minFreq, "Set minimum CPU frequency (GHz)")->check(CLI::PositiveNumber);
    set->add_option("--max-freq", cfg.maxFreq, "Set maximum CPU frequency (GHz)")->check(CLI::PositiveNumber);

    // monitor subcommand
    auto* monitor = app.add_subcommand("monitor", "Watch CPU frequencies live");
    monitor->add_option("-c,--cpu", cfg.cpuId, "Select CPU")->default_val(-1);
    monitor->add_option("-i,--interval", cfg.interval, "Set monitoring interval (seconds)")->check(CLI::Range(1, std::numeric_limits<int>::max()));



    try{
        app.parse(argc, argv);
    }
    catch (const CLI::ParseError &e) {
        std::exit(app.exit(e));
    }

    if      (info->parsed())    cfg.activeSubcommand = Subcommand::Info;
    else if (set->parsed())     cfg.activeSubcommand = Subcommand::Set;
    else if (monitor->parsed()) cfg.activeSubcommand = Subcommand::Monitor;


    return cfg;
}
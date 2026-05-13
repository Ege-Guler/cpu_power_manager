#pragma once
#include <string>

enum class Subcommand { None, Info, Set, Monitor };

struct Config {

    // subcommand
    Subcommand activeSubcommand = Subcommand::None;

    // global
    bool verbose = false;

    // info       show CPU info
    bool showFrequencies = false;
    bool showGovernors   = false;
    bool showDomains     = false;
    bool showAvailable   = false;
    int  cpuId           = -1;     // -1 = all CPUs

    // set        apply settings
    std::string governor;          // --governor powersave
    std::string epp;               // --epp balance_power
    double minFreq = 0.0;          // --min-freq 0.8 (GHz)
    double maxFreq = 0.0;          // --max-freq 3.2 (GHz)

    // monitor    watch frequencies live
    int interval = 1;              // --interval 1 (seconds)
};

Config parse_args(int argc, char* argv[]);
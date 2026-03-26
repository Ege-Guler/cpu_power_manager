#pragma once

namespace CpuPaths
{
    constexpr const char *CPU_BASE =
        "/sys/devices/system/cpu";

    constexpr const char *CPU_DIR =
        "/sys/devices/system/cpu/cpu%d";

    constexpr const char *CPUFREQ_DIR =
        "/sys/devices/system/cpu/cpu%d/cpufreq";

    constexpr const char *SCALING_GOVERNOR =
        "/sys/devices/system/cpu/cpu%d/cpufreq/scaling_governor";

    constexpr const char *AVAILABLE_GOVERNORS =
        "/sys/devices/system/cpu/cpu%d/cpufreq/scaling_available_governors";

    constexpr const char *CUR_FREQ =
        "/sys/devices/system/cpu/cpu%d/cpufreq/scaling_cur_freq";

    constexpr const char *MIN_FREQ =
        "/sys/devices/system/cpu/cpu%d/cpufreq/scaling_min_freq";

    constexpr const char *MAX_FREQ =
        "/sys/devices/system/cpu/cpu%d/cpufreq/scaling_max_freq";
    constexpr const char *ENERGY_PERFORMANCE_PREFERENCE =
        "/sys/devices/system/cpu/cpu%d/cpufreq/energy_performance_preference";
    constexpr const char *ENERGY_PERFORMANCE_PREFERENCE_AVAILABLE =
        "/sys/devices/system/cpu/cpu%d/cpufreq/energy_performance_available_preferences";
}
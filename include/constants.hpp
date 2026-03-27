#pragma once
#include <string_view>

namespace CpuPaths
{
    inline constexpr std::string_view CPU_BASE =
        "/sys/devices/system/cpu";

    inline constexpr std::string_view CPU_DIR =
        "/sys/devices/system/cpu/cpu{}";

    inline constexpr std::string_view CPUFREQ_DIR =
        "/sys/devices/system/cpu/cpu{}/cpufreq";

    inline constexpr std::string_view SCALING_GOVERNOR =
        "/sys/devices/system/cpu/cpu{}/cpufreq/scaling_governor";

    inline constexpr std::string_view AVAILABLE_GOVERNORS =
        "/sys/devices/system/cpu/cpu{}/cpufreq/scaling_available_governors";

    inline constexpr std::string_view CUR_FREQ =
        "/sys/devices/system/cpu/cpu{}/cpufreq/scaling_cur_freq";

    inline constexpr std::string_view MIN_FREQ =
        "/sys/devices/system/cpu/cpu{}/cpufreq/scaling_min_freq";

    inline constexpr std::string_view MAX_FREQ =
        "/sys/devices/system/cpu/cpu{}/cpufreq/scaling_max_freq";
    inline constexpr std::string_view ENERGY_PERFORMANCE_PREFERENCE =
        "/sys/devices/system/cpu/cpu{}/cpufreq/energy_performance_preference";
    inline constexpr std::string_view ENERGY_PERFORMANCE_PREFERENCE_AVAILABLE =
        "/sys/devices/system/cpu/cpu{}/cpufreq/energy_performance_available_preferences";
    inline constexpr std::string_view RELATED_CPUS =
        "/sys/devices/system/cpu/cpu{}/cpufreq/related_cpus";
    inline constexpr std::string_view SCALING_DRIVER =
        "/sys/devices/system/cpu/cpu{}/cpufreq/scaling_driver";
}
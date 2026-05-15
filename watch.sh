#!/bin/bash

CPU_COUNT=$(nproc)

FILES=()
for i in $(seq 0 $((CPU_COUNT - 1))); do
    FILES+=(
        "/sys/devices/system/cpu/cpu${i}/cpufreq/scaling_governor"
        "/sys/devices/system/cpu/cpu${i}/cpufreq/scaling_max_freq"
        "/sys/devices/system/cpu/cpu${i}/cpufreq/scaling_min_freq"
    )
done

echo "Watching $CPU_COUNT cores (${#FILES[@]} files total):"
for f in "${FILES[@]}"; do
    echo "  - $f"
done
echo ""

inotifywait -m -e modify "${FILES[@]}" |
while read -r directory event filename; do
    filepath="${directory}${filename}"
    content=$(cat "$filepath" 2>/dev/null)
    echo "[$(date '+%Y-%m-%d %H:%M:%S')] $event: $filepath -> $content"
done
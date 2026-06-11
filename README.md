# cpu-power

`cpu-power` is a CLI tool for inspecting and managing Linux CPU frequency
scaling and power settings via `sysfs`
(`/sys/devices/system/cpu/cpu*/cpufreq/`).

    cpu-power [OPTIONS] SUBCOMMAND

## Commands

- [`info`](#info) — show CPU information (read-only)
- [`set`](#set) — apply CPU governor and frequency settings (requires root)
- [`monitor`](#monitor) — watch CPU frequencies live (not yet implemented)

---

## `info`

Show information about one or all CPU cores: governor, energy performance
preference (EPP), scaling frequencies, and hardware frequency limits.

This subcommand is read-only and does not require root privileges.

### Usage

    cpu-power info [OPTIONS]

### Options

| Option | Description | Default |
|---|---|---|
| `-c, --cpu <INT>` | Limit output to a single CPU core (e.g. `-c 0`) | `-1` (all CPUs) |
| `--freq` | Show scaling (current) frequencies instead of full info | off |
| `--governors` | Show governors instead of full info | off |
| `--domains` | Show CPU frequency domains (groups of related CPUs) | off |
| `--available` | Show *available* options instead of *current* values; modifies `--freq` and `--governors` | off |

`--freq`, `--governors`, and `--domains` are mutually exclusive in effect —
if more than one is given, the first one checked (`--freq`, then
`--governors`, then `--domains`) wins. With none of them set, full per-CPU
info is printed.

### Examples

#### Full info for every CPU (default)

```
$ cpu-power info
CPU 0 Info:
  Governor: performance
  Available Governors: conservative ondemand userspace powersave performance schedutil
  Energy Performance Preference: balance_performance
  Available Energy Performance Preferences: default performance balance_performance balance_power power
  Scaling Current Frequency: 3.70084 GHz
  Scaling Min Frequency: 0.4 GHz
  Scaling Max Frequency: 4.4 GHz
  CPU Info Min Frequency: 0.4 GHz
  CPU Info Max Frequency: 4.4 GHz
-----------------------------
CPU 1 Info:
  ...
```

#### Full info for a single CPU

```
$ cpu-power info -c 0
CPU 0 Info:
  Governor: performance
  ...
```

#### Current scaling frequencies

```
$ cpu-power info --freq
Current CPU Frequencies (GHz):
CPU  0:    0.40
CPU  1:    4.41
CPU  2:    0.40
...
```

#### Hardware frequency ranges (`cpuinfo_min_freq` / `cpuinfo_max_freq`)

```
$ cpu-power info --freq --available
CPU Frequency Ranges (GHz):
CPU 0      Available Frequency Range (GHz): 0.40 - 4.40
CPU 1      Available Frequency Range (GHz): 0.40 - 4.40
...
```

#### Current governors

```
$ cpu-power info --governors
Current CPU Governors:
CPU  0: performance
CPU  1: performance
...
```

#### Governors available on every CPU

```
$ cpu-power info --governors --available
Common Available Governors for All CPUs: conservative ondemand userspace powersave performance schedutil
```

#### Governors available on a single CPU

```
$ cpu-power info -c 1 --governors --available
CPU 1 Available Governors: conservative ondemand userspace powersave performance schedutil
```

#### CPU frequency domains

Groups CPUs that share a `cpufreq` policy (`related_cpus`), printing the
lowest-numbered CPU in each group as the domain leader.

```
$ cpu-power info --domains
CPU Domains (related CPUs):
CPU(0):  0
CPU(1):  1
...
```

### Errors

If `-c <id>` does not refer to a discovered CPU, the command exits with
status `1` and prints:

```
error: CPU with ID <id> not found.
```

---

## `set`

Apply CPU power settings: scaling governor and/or min/max scaling
frequencies.

This subcommand writes to `sysfs` and **requires root privileges**
(run with `sudo`).

### Usage

    cpu-power set [OPTIONS]

### Options

| Option | Description | Default |
|---|---|---|
| `-c, --cpu <INT>` | Apply settings to a single CPU core (e.g. `-c 0`) | `-1` (all CPUs) |
| `--governor <NAME>` | Set the scaling governor (e.g. `performance`, `powersave`, `schedutil`) | unset |
| `--min-freq <GHz>` | Set the minimum scaling frequency, in GHz (must be a positive number) | unset |
| `--max-freq <GHz>` | Set the maximum scaling frequency, in GHz (must be a positive number) | unset |

`--governor`, `--min-freq`, and `--max-freq` can be combined in a single
invocation; if more than one is given, the governor is applied first,
followed by `--min-freq`, then `--max-freq`.

### Behavior

#### `--governor`

- With `-c <id>`, the governor must be in the **available governors for
  that CPU** (`scaling_available_governors`), or the command fails.
- Without `-c`, the governor must be **common to all CPUs** (the
  intersection of every CPU's available governors), or the command fails
  and lists the common governors.
- Setting a governor also sets a recommended Energy Performance Preference
  (EPP) for it:

  | Governor | EPP |
  |---|---|
  | `performance` | `performance` |
  | `powersave` | `balance_power` |
  | `schedutil` | `balance_performance` |
  | `ondemand` | `balance_performance` |
  | `conservative` | `balance_power` |
  | *(any other)* | `balance_performance` (default) |

  This requires the CPU driver to expose
  `energy_performance_preference` (Intel/AMD HWP). If the EPP write fails
  or the requested EPP isn't in `energy_performance_available_preferences`,
  the governor is **not** changed and the command fails.

#### `--min-freq` / `--max-freq`

- Values are given in **GHz** (e.g. `1.2` for 1.2 GHz) and must be
  positive.
- The requested frequency must fall within the CPU's hardware limits
  (`cpuinfo_min_freq` – `cpuinfo_max_freq`); otherwise the command fails
  with an out-of-range error and no write is made.
- The CPU's `scaling_min_freq` / `scaling_max_freq` is written via sysfs.

### Examples

#### Set the governor for all CPUs

```
$ sudo cpu-power set --governor powersave
```

#### Set the governor for a single CPU

```
$ sudo cpu-power set -c 0 --governor performance
```

#### Set min/max scaling frequency for all CPUs

```
$ sudo cpu-power set --min-freq 0.8 --max-freq 3.2
```

#### Set max scaling frequency for a single CPU

```
$ sudo cpu-power set -c 4 --max-freq 3.0
```

#### Combine governor and frequency limits

```
$ sudo cpu-power set --governor schedutil --min-freq 0.8 --max-freq 4.0
```

### Errors

Unsupported governor (per-CPU):

```
$ cpu-power set -c 0 --governor bogus
error: Governor 'bogus' is not supported by CPU 0.
```

Unsupported governor (all CPUs):

```
$ cpu-power set --governor bogus
error: Governor 'bogus' is not supported by all CPUs.
Common governors: conservative ondemand userspace powersave performance schedutil.
```

Frequency outside hardware limits:

```
$ sudo cpu-power set -c 0 --min-freq 10
error: Failed to set minimum frequency for CPU 0. Requested frequency 10 GHz is out of CPU info bounds (0.40 GHz - 4.40 GHz)
```

Unknown CPU ID:

```
$ sudo cpu-power set -c 99 --governor performance
error: CPU with ID 99 not found.
```

Permission denied (not running as root):

```
$ cpu-power set --governor performance
error: Failed to set governor 'performance' for all CPUs. ...
Hint: try running with sudo or as root.
```

---

## `monitor`

> **Status: not yet implemented.** The `monitor` subcommand is accepted by
> the CLI and its options are parsed, but `main.cpp` currently performs no
> action for it — running it is a no-op that exits successfully.

### Usage

    cpu-power monitor [OPTIONS]

### Options

| Option | Description | Default |
|---|---|---|
| `-c, --cpu <INT>` | Select a single CPU core to monitor | `-1` (all CPUs) |
| `-i, --interval <INT>` | Polling interval in seconds (must be `>= 1`) | `1` |

### Planned behavior

Continuously print the live scaling frequency of the selected CPU(s),
refreshing every `--interval` seconds — similar to `info --freq`, but
repeated like `watch`.

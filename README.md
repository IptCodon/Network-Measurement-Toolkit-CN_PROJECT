# Network-Measurement-Toolkit-CN_PROJECT
 This repository contains a custom network measurement system built entirely from scratch in C for CS-30003 Coding Assignment 1. Operating directly at the network and transport layers, this toolkit bypasses standard OS networking libraries to manually construct ICMP/UDP headers and execute internet checksums over raw sockets.

## Setup and Requirements
Because this toolkit utilizes raw sockets, all modules strictly require administrator privileges (`sudo`) to execute on Linux. 

## Features and Usage
### 1) Rupen's Ping module part:
**Description:** A custom ping implementation built directly over raw sockets. It bypasses OS libraries to manually construct ICMP Echo Request headers, calculates RFC 1071 internet checksums, and mathematically tracks Round Trip Time (RTT), network jitter, and packet loss percentages.

**Compilation:**    
`gcc ping.c -o ping`

**Execution:** 
`sudo ./ping <Target_IP>`
-------------------------

### 2) Subhasis's Traceroute module part:
**Description:** A custom traceroute implementation built directly over raw sockets that handles both UDP and ICMP packet crafting. It manually manipulates IP TTL headers to trace the hop sequence and implements strict probe-reply correlation by parsing nested ICMP error messages.

**Compilation:**    
`gcc traceroute.c -o traceroute`

**Execution:** 
`sudo ./traceroute <Target_IP> <-i|-u>`
-------------------------

### 3) Diptalal Pradhan's PMTU and Bandwidth Estimation modules:

**Description:** Implements IPv4 Path MTU Discovery using raw sockets, ICMP probes, the Don't Fragment (DF) flag, and binary search to estimate the largest packet size that can traverse a network path without fragmentation. It also estimates network bandwidth using packet-pair dispersion by measuring the time difference between corresponding ICMP replies.

#### **A) Path MTU Discovery (PMTU)**

**Compilation:**

```bash
gcc -Wall -Wextra -Wpedantic pmtu/pmtu.c -o pmtu_tool
```

**Execution:**

```bash
sudo ./pmtu_tool 1.1.1.1
```

**Documentation:** For more details, refer to the [PMTU README](pmtu/README.md).

#### **B) Bandwidth Estimation**

**Compilation:**

```bash
gcc -Wall -Wextra -Wpedantic bandwidth/bandwidth.c -o bandwidth_tool
```

**Execution:**

```bash
sudo ./bandwidth_tool <Target_IP>
```

**Documentation:** For more details, refer to the [Bandwidth Estimation README](bandwidth/README.md).

**-------------------------**


---

### 4) Amlan's Bufferbloat Detection & Integrated Toolkit module part:

**Description:** A multi-threaded bufferbloat detection and active queue analysis engine built over raw ICMP sockets and POSIX threads (`pthread`). It integrates the toolkit's custom Ping, Traceroute, Path MTU Discovery (PMTU), and Bandwidth modules into a unified pipeline. By generating concurrent high-load UDP saturation traffic while probing real-time ICMP Echo RTT, it measures queuing delay inflation (bufferbloat), calculates baseline vs. loaded latency spikes, logs CSV telemetry, and generates automated comparative visualizations against system `ping` and `mtr` benchmarks.

**Compilation:**
`gcc new_bufferbloat/new_bufferbloat.c -o new_bufferbloat/bufferbloat_bin -lpthread -lm`

## Execution: `sudo ./new_bufferbloat/bufferbloat_bin <Target_IP>`

**Example Target IPs (`<Target_IP>`):**
- `8.8.8.8` (Google Public DNS — recommended for end-to-end WAN bufferbloat testing and used in system benchmarks: `sudo ./new_bufferbloat/bufferbloat_bin 8.8.8.8`)
- `1.1.1.1` (Cloudflare DNS — low-latency external reference target)
- `192.168.1.1` (Local Default Gateway / Router IP — ideal for isolating first-hop Wi-Fi/LAN router queue delay)

**Automated Experiment & Plotting Pipeline:**
- **Run Full Experiment Suite:** `sudo python3 new_bufferbloat/new_run_experiment.py`
- **Generate Comparison Plots:** `python3 new_bufferbloat/new_plot_result.py`
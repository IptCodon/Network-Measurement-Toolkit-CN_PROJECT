# Path MTU Discovery (PMTU)

This module implements Path Maximum Transmission Unit (PMTU) discovery for IPv4 as part of the Network Measurement Toolkit for CS-30003 Coding Assignment 1.

The implementation probes packet sizes using the IPv4 **Don't Fragment (DF)** flag and interprets ICMP responses to estimate the largest working packet size within the configured search range.

## Objective

Discover an approximate path MTU without relying on a high-level PMTU discovery library.

The implementation manually constructs the relevant IPv4 and ICMP packet data and uses raw sockets to send probes and receive responses.

## Implementation Features

- Manually defined IPv4 header structure.
- Internet checksum calculation.
- IPv4 Don't Fragment flag handling.
- ICMP Echo Request probe construction.
- Matching probe replies using identifiers and sequence numbers.
- Handling of ICMP Destination Unreachable — Fragmentation Needed responses.
- Configurable minimum and maximum probe sizes.
- Binary search between confirmed working and too-large packet sizes.
- Retry and timeout handling for inconclusive probes.
- Reporting of the largest confirmed working size and relevant limitations.

## Source Files

```text
pmtu/
├── README.md
├── pmtu.c
└── pmtu.h
```

- **`pmtu.c`** contains packet construction, checksum logic, probe handling, and the discovery algorithm.
- **`pmtu.h`** declares the IPv4 header structure, configuration constants, helper functions, and the `discover_pmtu()` entry point.

## Build

From the repository root:

```bash
gcc -Wall -Wextra -Wpedantic pmtu/pmtu.c -o pmtu_tool
```

## Run

```bash
sudo ./pmtu_tool 1.1.1.1
```

Replace `1.1.1.1` with the IPv4 destination you are authorized to test.

The program expects a destination IPv4 address. Raw-socket access may require elevated privileges on Linux or WSL.

## How the Algorithm Works

1. **Validate the destination:** Parse the supplied IPv4 address.
2. **Probe the lower bound:** Test the configured minimum packet size.
3. **Probe the upper bound:** Test the configured maximum packet size.
4. **Search between the bounds:** When the lower bound works and the upper bound is confirmed too large, test the midpoint.
5. **Update the bounds:** A successful probe raises the confirmed working lower bound. A matching Fragmentation Needed response lowers the too-large upper bound.
6. **Stop when adjacent:** When the bounds differ by one byte, report the largest confirmed working size and the smallest confirmed too-large size.

The current configuration defines a search range of **576–1500 bytes**. These sizes refer to the complete IPv4 packet, including its headers.

## Interpreting Results

- **Echo Reply:** The probe received a matching response, confirming that the tested packet size worked for that probe.
- **Fragmentation Needed:** A matching ICMP error indicates that the packet could not be forwarded without fragmentation along the tested path.
- **Timeout:** The probe outcome is inconclusive. A timeout does not prove that a packet is too large.
- **Local packet-size rejection:** A local operating-system error is not the same as a router returning ICMP Fragmentation Needed.

An exact PMTU claim requires careful interpretation of the evidence. ICMP filtering, rate limiting, changing routes, and local socket restrictions may affect results.

## Testing Notes

For reproducible experiments, record:

- Destination IPv4 address.
- Network connection used.
- Probe sizes and response types.
- Any router-reported next-hop MTU.
- Timeouts, local errors, and retry behaviour.
- Comparison with an available standard diagnostic utility.

Repeat tests under different network conditions when possible. Do not treat a timeout alone as evidence of a smaller path MTU.

## Academic Context

**Course:** CS-30003 — Coding Assignment 1  
**Project:** P5 — Network Measurement Toolkit  
**Owner:** Diptalal Pradhan

This module is part of the team's low-level network measurement project and is intended to demonstrate packet construction, ICMP handling, and active path MTU estimation.

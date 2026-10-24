# Path MTU Discovery (PMTU)

<<<<<<< HEAD
This module implements Path Maximum Transmission Unit (PMTU) discovery for IPv4 as part of the Network Measurement Toolkit for CS-30003 Coding Assignment 1.
=======
This module implements Path Maximum Transmission Unit (PMTU) discovery for IPv4 as part of the Network Measurement Toolkit for **CS-30003 Coding Assignment 1 — Project P5**.
>>>>>>> Diptalal-Mtu-Bandwidth

The implementation probes packet sizes using the IPv4 **Don't Fragment (DF)** flag and interprets ICMP responses to estimate the largest working packet size within the configured search range.

## Objective

Discover an approximate path MTU without relying on a high-level PMTU discovery library.

The implementation manually constructs the relevant IPv4 and ICMP packet data and uses raw sockets to send probes and receive responses.

## Implementation Features

- Manually defined IPv4 header structure.
- Internet checksum calculation.
<<<<<<< HEAD
- IPv4 Don't Fragment flag handling.
=======
- IPv4 Don't Fragment (DF) flag handling.
>>>>>>> Diptalal-Mtu-Bandwidth
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

<<<<<<< HEAD
## Build

From the repository root:
=======
## Requirements

- Linux or a compatible environment with raw-socket support.
- GCC or another compatible C compiler.
- Permission to create raw sockets, which may require elevated privileges.
- Network connectivity to the IPv4 destination being tested.

The program may require additional consideration when run under WSL, depending on the networking environment and available raw-socket permissions.

## Build

From the repository root, compile the PMTU module:
>>>>>>> Diptalal-Mtu-Bandwidth

```bash
gcc -Wall -Wextra -Wpedantic pmtu/pmtu.c -o pmtu_tool
```

<<<<<<< HEAD
## Run

=======
The compiler options enable common warnings to help identify potential issues.

## Run

Run the program with an IPv4 destination address:

>>>>>>> Diptalal-Mtu-Bandwidth
```bash
sudo ./pmtu_tool 1.1.1.1
```

Replace `1.1.1.1` with the IPv4 destination you are authorized to test.

<<<<<<< HEAD
The program expects a destination IPv4 address. Raw-socket access may require elevated privileges on Linux or WSL.
=======
Raw-socket access may require elevated privileges. The program expects a destination IPv4 address.
>>>>>>> Diptalal-Mtu-Bandwidth

## How the Algorithm Works

1. **Validate the destination:** Parse the supplied IPv4 address.
2. **Probe the lower bound:** Test the configured minimum packet size.
3. **Probe the upper bound:** Test the configured maximum packet size.
4. **Search between the bounds:** When the lower bound works and the upper bound is confirmed too large, test the midpoint.
5. **Update the bounds:** A successful probe raises the confirmed working lower bound. A matching Fragmentation Needed response lowers the too-large upper bound.
6. **Stop when adjacent:** When the bounds differ by one byte, report the largest confirmed working size and the smallest confirmed too-large size.

The current configuration defines a search range of **576–1500 bytes**. These sizes refer to the complete IPv4 packet, including its headers.

<<<<<<< HEAD
=======
The binary-search procedure relies on having reliable evidence for the working and too-large bounds. An inconclusive probe should not automatically be treated as proof that a packet is too large.

>>>>>>> Diptalal-Mtu-Bandwidth
## Interpreting Results

- **Echo Reply:** The probe received a matching response, confirming that the tested packet size worked for that probe.
- **Fragmentation Needed:** A matching ICMP error indicates that the packet could not be forwarded without fragmentation along the tested path.
- **Timeout:** The probe outcome is inconclusive. A timeout does not prove that a packet is too large.
- **Local packet-size rejection:** A local operating-system error is not the same as a router returning ICMP Fragmentation Needed.

<<<<<<< HEAD
An exact PMTU claim requires careful interpretation of the evidence. ICMP filtering, rate limiting, changing routes, and local socket restrictions may affect results.
=======
An exact PMTU claim requires careful interpretation. ICMP filtering, rate limiting, changing routes, and local socket restrictions may affect results.

The reported value should be understood as an estimate based on the probes and responses observed during the test, not a guarantee that every packet of that size will always traverse the path.
>>>>>>> Diptalal-Mtu-Bandwidth

## Testing Notes

For reproducible experiments, record:

- Destination IPv4 address.
- Network connection used.
- Probe sizes and response types.
- Any router-reported next-hop MTU.
- Timeouts, local errors, and retry behaviour.
- Comparison with an available standard diagnostic utility.

Repeat tests under different network conditions when possible. Do not treat a timeout alone as evidence of a smaller path MTU.

<<<<<<< HEAD
## Academic Context

**Course:** CS-30003 — Coding Assignment 1  
**Project:** P5 — Network Measurement Toolkit  
**Owner:** Diptalal Pradhan
=======
## Limitations

- ICMP messages may be filtered or rate-limited by firewalls and routers.
- A destination may not respond to ICMP Echo Requests even when packets can traverse the path.
- Network routes and conditions may change during testing.
- Local operating-system or interface restrictions may prevent certain probe sizes from being sent.
- Timeouts and missing replies can make the result inconclusive.
- The configured search range limits the packet sizes that the algorithm can evaluate.
- The observed result may differ from the path MTU reported by other diagnostic tools.

## Academic Context

- **Course:** CS-30003 — Coding Assignment 1
- **Project:** P5 — Network Measurement Toolkit
- **Contributor:** Diptalal Pradhan
- **Module:** Path MTU Discovery (PMTU)
>>>>>>> Diptalal-Mtu-Bandwidth

This module is part of the team's low-level network measurement project and is intended to demonstrate packet construction, ICMP handling, and active path MTU estimation.

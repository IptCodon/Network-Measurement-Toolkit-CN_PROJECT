# Bandwidth Estimation

This module implements bandwidth estimation using the **packet-pair dispersion technique** as part of the Network Measurement Toolkit for **CS-30003 Coding Assignment 1 — Project P5**.

The implementation sends pairs of ICMP Echo Request probes and measures the time difference between their corresponding replies to estimate an approximate network transmission rate.

## Objective

Estimate network throughput using active probing and packet-pair dispersion without relying on a high-level bandwidth measurement library.

The implementation constructs ICMP probes, matches incoming replies, records reply-arrival times, and calculates an approximate rate from the observed time difference between packets in a pair.

## Implementation Features

- ICMP Echo Request probe construction.
- Packet-pair probing.
- Matching replies using identifiers and sequence numbers.
- Reply-arrival timing and dispersion calculation.
- Approximate bandwidth-rate estimation.
- Statistical summaries of observed measurements.

## Source Files

```text
bandwidth/
└── bandwidth.c
```

- **`bandwidth.c`** contains probe construction, reply matching, timing logic, and bandwidth estimation.

## Requirements

- Linux or a compatible environment with raw-socket support.
- GCC or another compatible C compiler.
- Appropriate privileges for raw-socket operations.
- Network connectivity to the IPv4 destination being tested.

## Build

From the repository root, compile the module:

```bash
gcc -Wall -Wextra -Wpedantic bandwidth/bandwidth.c -o bandwidth_tool
```

## Run

Run the program with the IPv4 destination accepted by the implementation:

```bash
sudo ./bandwidth_tool <Target_IP>
```

Replace `<Target_IP>` with the target IPv4 address you want to test.

Raw-socket access may require elevated privileges.

## How the Algorithm Works

1. **Construct probes:** Prepare ICMP Echo Request packets for packet-pair measurements.
2. **Send packet pairs:** Transmit two probes as a pair.
3. **Match replies:** Associate incoming responses with the corresponding probes using identifiers and sequence numbers.
4. **Measure dispersion:** Record the time difference between the arrivals of the corresponding replies.
5. **Estimate the rate:** Use the observed packet-pair timing to calculate an approximate network rate.
6. **Summarize measurements:** Report the measured results and statistical summaries supported by the implementation.

## Interpreting Results

The measured reply dispersion provides an estimate of network performance. It should not automatically be interpreted as the physical link's capacity or the exact available bandwidth.

Results can be affected by:

- Network congestion and queuing.
- Variable packet-processing delays.
- ICMP filtering and rate limiting.
- Delayed, lost, or unmatched replies.
- Changes in network conditions during testing.

Because the measurement uses ICMP reply timing, the observed rate may differ from the actual data-plane throughput.

## Testing Notes

For reproducible experiments, record:

- Target IPv4 address.
- Network connection used.
- Measurement results across multiple runs.
- Network conditions during testing.
- Any timeouts, missing replies, or unusual measurements.

Repeat measurements under different network conditions when possible, and compare results cautiously.

## Limitations

- ICMP reply timing does not directly measure application throughput.
- Packet-pair dispersion may be affected by queuing and processing delays.
- ICMP filtering or rate limiting can affect probe responses.
- Results depend on the implementation's probe sizes, timing method, and measurement procedure.

Interpret the output as an approximate measurement rather than a guaranteed value for link capacity.

## Academic Context

- **Course:** CS-30003 — Coding Assignment 1
- **Project:** P5 — Network Measurement Toolkit
- **Contributor:** Diptalal Pradhan
- **Module:** Bandwidth Estimation

This module demonstrates packet-pair probing and timing-based network measurement using C and raw sockets.

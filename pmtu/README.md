# Path MTU Discovery

## Objective
Determine the largest IPv4 packet size that can travel from the source to the destination without fragmentation.

## Approach
1. Construct IPv4 headers manually.
2. Set the Don't Fragment (DF) flag.
3. Send probes of different sizes.
4. Handle ICMP responses and timeouts.
5. Adjust probe sizes to estimate the path MTU.

## Current Status
- [x] IPv4 header structure
- [x] Checksum function
- [x] DF flag handling
- [ ] Raw socket transmission
- [ ] ICMP response parsing
- [ ] Probe size search
- [ ] PMTU result

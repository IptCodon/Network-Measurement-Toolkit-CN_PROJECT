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
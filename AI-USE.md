
This repository is a documentation of the use of AI tools in service of the project guidelines of the course. We(Rupen Raj Rout, Subhasis Baral, Diptalal Pradhan and Amlan Amrit Behera) use AI assistants purely as a collaborative aid to learn concepts, debug low-level C errors, write unit tests and structure Python visualization of data scripts. In line with the course policy, the team builds by hand all core protocol implementations, such as raw socket plumbing, custom checksum arithmetic, packet parsing, and response correlation. 

### Rupen Raj Rout (Ping Module & Integration)
* **Tools Used:** Gemini
* **Primary Applications:** 
  * **Protocol Concept Clarification:** Utilized as a tutor to understand the underlying bitwise operations required to manually calculate the RFC 1071 internet checksum for custom ICMP packet headers.
  * **Mathematical Verification:** Discussed the logic and mathematical approaches for accurately tracking Round Trip Time (RTT), network jitter, and packet loss percentages across continuous network polling.
  * **Debugging Support:** Analyzed terminal execution outputs with the AI to verify TTL (Time to Live) expiration behaviors and debug the custom ICMP Echo Request handling over raw sockets.
  * **Version Control Strategy:** Received structural guidance on establishing a sequential Git workflow and conflict-free branch merging to safely integrate the team's separate C modules.

Subhasis Baral (Traceroute Module)

* Tools Used: Gemini
* Primary Applications:
* Protocol Concept Clarification: Utilized as a tutor to understand the nested structure of ICMP error messages (such as `ICMP_TIME_EXCEEDED` and `ICMP_DEST_UNREACH`) and how to correctly calculate memory offsets to extract the inner IP headers.
* Implementation Strategy: Discussed the architectural logic required to effectively toggle between ICMP and UDP traceroute modes, specifically focusing on TTL manipulation and utilizing target port increments.
* Debugging Support: Analyzed raw socket parsing code with the AI to resolve payload alignment issues, ensuring accurate extraction and matching of the original process ID, sequence numbers, and destination ports for strict probe-reply correlation.
* Documentation & Formatting: Used the assistant to draft clear, standardized README instructions for compiling and executing the traceroute module and generating commit messages.

### Diptalal Pradhan (PMTU Discovery & Bandwidth Estimation)

* **Tools Used:** ChatGPT
* **Primary Applications:**
  * **Protocol Concept Clarification:** Used AI assistance to understand Path MTU Discovery, IPv4's Don't Fragment (DF) flag, ICMP responses, and binary search for estimating the maximum packet size along a network path.
  * **Implementation & Debugging Support:** Discussed raw socket programming, IPv4 and ICMP packet construction, checksum handling, probe-response matching, timeout handling, and troubleshooting C compilation and runtime errors.
  * **Bandwidth Estimation:** Used AI assistance to understand the packet-pair dispersion approach and interpret timing differences between corresponding ICMP replies, including the limitations of noisy measurements.
  * **Documentation Support:** Used AI assistance to organize and refine the PMTU and bandwidth module README files, including build instructions, execution steps, algorithm descriptions, and testing limitations.
  * **Version Control Guidance:** Received guidance on Git branching, commits, merging, resolving documentation conflicts, and updating the team's shared repository.

  ### Amlan Amrit Behera (Bufferbloat Detection & Integrated Toolkit Module)
- ** Tools Used: Gemini **
-** Primary Applications: **
  - Concurrency & Protocol Concept Clarification: Utilized as a tutor to understand the interaction between multi-threaded POSIX (`pthread`) execution and raw ICMP socket probing, specifically how concurrent high-rate UDP saturation traffic induces queuing delay inflation (bufferbloat) across intermediate router buffers.
  - System Integration & Implementation Strategy: Discussed C preprocessor macro techniques (`#define` / `#undef`) and header inclusion strategies to cleanly integrate the team's standalone Ping (`ping.c`) and Path MTU Discovery (`pmtu.c` & `pmtu.h`) modules into a unified measurement pipeline without symbol or `main()` function collisions.
  - Automation & Visualization Scripting: Used the assistant to structure the Python experiment orchestration script (`new_run_experiment.py`) and matplotlib plotting utility (`new_plot_result.py`) for parsing CSV telemetry and generating comparative baseline-versus-loaded RTT visualizations against system `ping` and `mtr` benchmarks.
  - Debugging & Version Control Support: Analyzed WSL network timeout behaviors (`HTTP 408` RPC errors) and received guidance on isolating feature branch commits (`pr-new-bufferbloat`), resolving divergent branch histories via `git rebase`, and formatting standardized `README.md` documentation.
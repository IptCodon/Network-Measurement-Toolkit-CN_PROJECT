
This repository is a documentation of the use of AI tools in service of the project guidelines of the course. We(Rupen Raj Rout, Subhasis Baral, Diptalal Pradhan and Amlan Amrit Behera) use AI assistants purely as a collaborative aid to learn concepts, debug low-level C errors, write unit tests and structure Python visualization of data scripts. In line with the course policy, the team builds by hand all core protocol implementations, such as raw socket plumbing, custom checksum arithmetic, packet parsing, and response correlation. 

### Rupen Raj Rout (Ping Module & Integration)
* **Tools Used:** Gemini
* **Primary Applications:** 
  * **Protocol Concept Clarification:** Utilized as a tutor to understand the underlying bitwise operations required to manually calculate the RFC 1071 internet checksum for custom ICMP packet headers.
  * **Mathematical Verification:** Discussed the logic and mathematical approaches for accurately tracking Round Trip Time (RTT), network jitter, and packet loss percentages across continuous network polling.
  * **Debugging Support:** Analyzed terminal execution outputs with the AI to verify TTL (Time to Live) expiration behaviors and debug the custom ICMP Echo Request handling over raw sockets.
  * **Version Control Strategy:** Received structural guidance on establishing a sequential Git workflow and conflict-free branch merging to safely integrate the team's separate C modules.

This repository is a documentation of the use of AI tools in service of the project guidelines of the course. We(Rupen Raj Rout, Subhasis Baral, Diptalal Pradhan and Amlan Amrit Behera) use AI assistants purely as a collaborative aid to learn concepts, debug low-level C errors, write unit tests and structure Python visualization of data scripts. In line with the course policy, the team builds by hand all core protocol implementations, such as raw socket plumbing, custom checksum arithmetic, packet parsing, and response correlation. We are doing an honest declaration here to demonstrate our academic integrity and preparedness for the individual viva.
Subhasis Baral (Traceroute Module)

* Tools Used: Gemini
* Primary Applications:
* Protocol Concept Clarification: Utilized as a tutor to understand the nested structure of ICMP error messages (such as `ICMP_TIME_EXCEEDED` and `ICMP_DEST_UNREACH`) and how to correctly calculate memory offsets to extract the inner IP headers.
* Implementation Strategy: Discussed the architectural logic required to effectively toggle between ICMP and UDP traceroute modes, specifically focusing on TTL manipulation and utilizing target port increments.
* Debugging Support: Analyzed raw socket parsing code with the AI to resolve payload alignment issues, ensuring accurate extraction and matching of the original process ID, sequence numbers, and destination ports for strict probe-reply correlation.
* Documentation & Formatting: Used the assistant to draft clear, standardized README instructions for compiling and executing the traceroute module and generating commit messages.

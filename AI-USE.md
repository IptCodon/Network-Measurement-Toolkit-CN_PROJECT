This repository is a documentation of the use of AI tools in service of the project guidelines of the course. We(Rupen Raj Rout, Subhasis Baral, Diptalal Pradhan and Amlan Amrit Behera) use AI assistants purely as a collaborative aid to learn concepts, debug low-level C errors, write unit tests and structure Python visualization of data scripts. In line with the course policy, the team builds by hand all core protocol implementations, such as raw socket plumbing, custom checksum arithmetic, packet parsing, and response correlation. 

### Rupen Raj Rout (Ping Module & Integration)
* **Tools Used:** Gemini
* **Primary Applications:** 
  * **Protocol Concept Clarification:** Utilized as a tutor to understand the underlying bitwise operations required to manually calculate the RFC 1071 internet checksum for custom ICMP packet headers.
  * **Mathematical Verification:** Discussed the logic and mathematical approaches for accurately tracking Round Trip Time (RTT), network jitter, and packet loss percentages across continuous network polling.
  * **Debugging Support:** Analyzed terminal execution outputs with the AI to verify TTL (Time to Live) expiration behaviors and debug the custom ICMP Echo Request handling over raw sockets.
  * **Version Control Strategy:** Received structural guidance on establishing a sequential Git workflow and conflict-free branch merging to safely integrate the team's separate C modules.
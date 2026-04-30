# FSC2H-State-Analysis: Investigating Race-Induced UAF in Sony Custom Extensions

## Technical Overview
This project explores a hypothesized vulnerability pathway within the proprietary Sony system call `sys_fsc2h_ctrl` (Syscall 597). The focus is on a synchronization failure during state transitions that may lead to a **Race-Induced Use-After-Free (UAF)**.

## Research Hypothesis
Modern firmware security (PS4 13.50 / PS5 13.20) necessitates shifting focus from base FreeBSD kernel bugs to proprietary hardware controllers. 
* **The Primitive**: A failure in atomic state transitions allows a `free()` operation to occur while concurrent threads maintain active references.
* **The Vector**: Entry is hypothesized via the BD-JB environment, leveraging established path traversal vectors to invoke custom syscalls.

## Repository Structure
* **LICENSE**: MIT Open Source License.
* **src/poc.c**: Logic simulation of the hypothesized race condition.
* **src/spray.c**: Heap spraying utility for memory reclamation.
* **docs/analysis.md**: Detailed engineering breakdown of UMA and state transitions.

This is the comprehensive documentation of our research journey and technical findings regarding the **fsc2h_gz_0day_reconstruction**. This content is designed to be placed in your README.md to serve as a complete reference for the global security community.
# **FSC2H-GZ-0Day-Analysis: Reconstructing the Gezine Race-Induced UAF**
## **1. Research Introduction**
This project represents a deep-dive technical reconstruction of the methodology likely utilized in the recent **0-day exploits** demonstrated by developer **Gezine** on **PlayStation 4 (firmware 13.50)** and **PlayStation 5 (firmware 13.20)**. As an independent researcher (**tHefishsop**), the goal was to identify the most plausible attack surface within the Sony-proprietary kernel extensions that would allow for such high-stability exploitation on modern, hardened firmware.
## **2. The Discovery: Why Syscall 597?**
Our analysis began by shifting focus away from the core FreeBSD kernel—which has seen extensive hardening—toward **Sony Custom Syscalls**. These proprietary extensions often manage hardware-specific interactions and lack the same level of public audit as open-source components.
Through deductive reasoning, we identified **sys_fsc2h_ctrl (Syscall 597)**, a controller responsible for file system and hardware-related path resolution, as a primary candidate for synchronization anomalies.
## **3. Technical Analysis of the Vulnerability**
### **3.1 The Vulnerability Primitive: Race-Induced UAF**
The core of this research identifies a **Race-Induced Use-After-Free (UAF)**. This occurs when the kernel fails to maintain atomicity during a shared object's state transition, allowing one thread to free the object while another thread still holds an active, stale reference (a dangling pointer).
### **3.2 The State Machine Logical Flaw**
The FSC2H controller utilizes an internal state machine to manage object lifecycles. We analyzed the following transition flow:
**INITIAL → RESOLVED → WAITING → COMPLETED**
 * **The Critical Window**: A race window exists between the CMD_RESOLVE and CMD_WAIT commands.
 * **The Conflict**: If a CMD_COMPLETE or a destructive cleanup command is issued by Thread B while Thread A is mid-transition into the WAITING state, the kernel may execute a free() on the object.
 * **The Result**: Thread A resumes execution, operating on memory that has been reclaimed, providing a UAF primitive.
## **4. Evidentiary Logic & Implementation**
### **4.1 Proof of Concept Logic (src/poc.c)**
The provided simulation demonstrates how two concurrent threads can be used to win the race:
 1. **Thread A (Victim)**: Enters the kernel via CMD_WAIT and yields execution (e.g., via tsleep).
 2. **Thread B (Trap)**: Initiates CMD_RESOLVE to bind its local context to the shared path object.
 3. **Trigger Phase**: A broadcast signal (CMD_COMPLETE) wakes both threads. If timed correctly, the kernel's logical failure leads to the "Victim" thread resuming with a pointer that has been freed by the "Trap" thread's exit or failure path.
### **4.2 Heap Spraying Strategy (src/spray.c)**
To turn this UAF into a functional exploit, we utilize the **Universal Memory Allocator (UMA)** behavior. Once the kernel object is freed, we use sendmsg() primitives to "spray" the heap with controlled data (e.g., a ROP chain). The goal is to ensure the vacated memory slot is filled with our payload before the victim thread attempts to use it again.
## **5. Security Mitigations & Overcoming Hurdles**
A modern kernel exploit must account for:
 * **SMEP/SMAP**: Our model assumes the use of **Kernel ROP (Return-Oriented Programming)** because direct execution of userland code is blocked.
 * **KASLR**: The research acknowledges that a separate **Information Leak** is required to determine the kernel's base address to calculate gadget offsets accurately.
## **6. Testable Predictions**
We have established specific markers for researchers to verify this hypothesis:
 1. **Thread Scaling**: Increasing thread concurrency should lead to a higher frequency of system instability or specific kernel panics.
 2. **Panic Signature**: Successful triggers should result in a Fatal trap 12: page fault in kernel mode, where the faulting address points to the controlled heap spray
 3.  This research framework integrates formal engineering discipline with hands-on system analysis to advance the understanding of proprietary kernel security.
*MIT License | Copyright (c) 2026 tHefishsop*

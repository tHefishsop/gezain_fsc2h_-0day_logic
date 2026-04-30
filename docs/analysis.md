# Technical Analysis: State Machine Corruption

## State Machine Lifecycle
The FSC2H controller manages objects through an unvalidated transition flow:
`INITIAL -> RESOLVED -> WAITING -> COMPLETED`

The vulnerability stems from the lack of atomicity during the `RESOLVED` to `WAITING` phase. A race condition allows a concurrent `free()` to target the object while the secondary thread assumes the object remains valid.

## Memory Allocator Gap
* **UMA Behavior**: The Orbis kernel utilizes the Universal Memory Allocator. If the target object is freed and immediately reclaimed via a heap spray, the kernel resumes execution using attacker-controlled data.
* **Domain Validation**: Proprietary extensions often fail to validate if a pointer remains within the expected heap zone boundaries before processing a `WAIT` operation.
* **Allocator Assumption**: While the target is likely in the `M_TEMP` zone, dedicated zones or reference counting may mitigate the reachability of the free primitive.

## Exploitation Hurdles
* **KASLR**: Requires a separate Information Leak to determine the kernel base address.
* **SMEP/SMAP**: Forces the use of pure Kernel ROP chains as userland execution is restricted.
* **Testable Predictions**: Scaling thread concurrency should increase system instability if the race condition hypothesis is correct.

# Memory Hierarchy Design Project

## Overview
The objective of this project is to design a memory hierarchy with an integrated memory controller responsible for handling load (LW) and store (SW) operations. The memory hierarchy consists of two levels of cache (L1 and L2) and main memory. Additionally, a victim cache is associated with L1 for improved performance.

## Memory Hierarchy Levels

### Specifications

| Memory Level   | Details |
|----------------|---------|
| **L1 Cache**   | - Direct-mapped cache<br>- 1 block per line<br>- 16 lines/sets<br>- Each block: 4 bytes (int) |
| **L2 Cache**   | - 8-way set-associative cache<br>- 1 block per line<br>- 16 sets<br>- Each block: 4 bytes (int)<br>- Replacement policy: LRU |
| **Victim Cache** | - Fully-associative<br>- 1 block per line<br>- 4 entries<br>- Each block: 4 bytes (int)<br>- Replacement policy: LRU |
| **Main Memory** | - 4096 lines<br>- Each line: 1 byte |

### Additional Notes:
- Each cache level is:
  - **Exclusive/Write-No-Allocate/Write-Through**
  - **Byte-addressable**
  - **Initially randomly initialized with all valid bits set to zero**

## Controller Operations

### Load Operation (LW)
- Search order: **L1 Cache → Victim Cache → L2 Cache**
- Update cache statistics for each level.
- If data is found, update the data, tag, and LRU positions as needed.
- Evicted data from L1 is placed in the Victim Cache; evicted data from the Victim Cache goes to L2.
![image](https://github.com/user-attachments/assets/5e0caa0e-ef72-424c-ba09-ca2d7b747a05)


### Store Operation (SW)
- Search order: **L1 Cache → Victim Cache -> L2 Cache**
- Update cache statistics and LRU positions.
- If data is present in both cache and memory, update both.
- If data is not present in the cache, update only main memory.
![image](https://github.com/user-attachments/assets/ae7d405e-34cd-4477-b228-4adbafa77421)


## Testing
The program entry point is `memory_driver.cpp`. 
Compiled with the command g++ *.cpp -o memory_driver and run with ./memory_driver <inputfile.txt>.
Print the miss-rate for L1 and L2, and the average access time (AAT) in the terminal. (Miss-rate reported only for LW)
AAT assumptions: L1 hit time: 1 cycle, Victim cache hit time: 1 cycle, L2 hit time: 8 cycles, Main memory hit time: 100 cycles

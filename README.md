# ShellOSX
**A Simulated OS Shell with Process Scheduling and Memory Management**

## Project Overview
ShellOSX is a streamlined operating system simulation built to demonstrate essential OS functionalities, including:
- Process scheduling (supporting First-Come-First-Serve, Shortest Job First, and Round Robin with aging policies)
- Advanced memory management through demand paging and an LRU (Least Recently Used) replacement policy
- Multi-process management within a customized shell environment

Developing this project sharpened my core systems programming skills, such as low-level process scheduling and memory allocation in C. The project also highlights concurrency, using pthreads to enable multi-threaded scheduling, while exploring memory efficiency through demand paging techniques and an optimized LRU page replacement policy.

---

## Key Objectives
1. **Multi-Process Scheduling**:
   - Implemented FCFS, SJF, and RR scheduling policies.
   - Added multi-threading for handling multiple processes.
2. **Memory Management**:
   - Simulated demand paging with a memory backing store.
   - Implemented LRU page replacement to optimize memory usage.
3. **Concurrent Execution**:
   - Enabled concurrent script execution using the exec command, supporting up to 3 concurrent processes.
   - Developed a background execution option, simulating asynchronous operations within the shell.

## Key Features
- **Custom Commands**:
   - `run [SCRIPT.TXT]`: Executes a script file line by line.
   - `exec [prog1 prog2 prog3] POLICY`: Executes up to 3 programs concurrently using the specified scheduling policy.
   - `quit`: Exits the shell, cleaning up memory and closing files.

- **Scheduling Policies**:
   - **FCFS**: Processes are executed in the order they are received.
   - **SJF**: Shortest jobs are prioritized, with aging to prevent starvation.
   - **RR**: Round Robin scheduling with a customizable time slice, supporting background and multi-threaded execution.

- **Memory Management**:
   - **Paging**: Dynamically loads pages for each process, storing inactive pages in a simulated backing store.
   - **LRU Replacement**: Replaces least-recently-used pages in the memory when new pages are required.

---

## Usage
1. **Compilation**:
   ```
   make clean; make mysh
   ```
2. **Running**:
   - **Interactive Mode**: `./mysh`
   - **Batch Mode**: `./mysh < testfile.txt`

## Example Execution
1. **Multi-Process Scheduling**:
   ```
   exec prog1 prog2 prog3 SJF
   ```
   Executes three programs using the Shortest Job First scheduling policy.

2. **Paging with LRU**:
   ```
   exec largeProgram RR
   ```
   Runs a large program under Round Robin scheduling with demand paging, ensuring efficient memory usage via LRU.

---

## Tools and Technologies
- **Language**: C
- **Threading**: POSIX threads (pthreads) for multi-threaded scheduling.
- **Version Control**: GitLab for collaboration and project tracking.
- **Makefile**: Compilation and linking are automated for easy testing and deployment.

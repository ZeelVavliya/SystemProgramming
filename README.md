# System Programming - Memory Management Simulation

A memory management simulator implementing a page replacement algorithm for virtual memory systems.

## Overview

This project simulates how operating systems manage memory using paging techniques. It implements a memory management system that tracks page tables and uses a Least Recently Used (LRU) page replacement algorithm to handle page faults. The simulation reads memory access patterns from an input file and demonstrates how virtual memory is mapped to physical RAM frames.

## Features

- Virtual memory simulation with paging
- Page tables for multiple processes
- LRU (Least Recently Used) page replacement algorithm
- Simulation of page faults and page evictions
- Memory access pattern analysis

## Technical Details

- Written in C
- Simulates RAM size of 16 units with page size of 2 units
- Supports up to 5 processes, each with 4 pages
- Models both local and global LRU replacement strategies

## Project Structure

```
system-programming/
├── simulation.c          # Main source code implementing the memory simulator
├── in.txt                # Input file with memory access patterns
├── out.txt               # Output file showing page tables and RAM state
├── out_1.txt             # Additional output file from test cases
├── out_2.txt             # Additional output file from test cases
└── README.md             # This file
```

## How It Works

1. **Initialization**: The program initializes virtual memory and page tables for each process
2. **Simulation**: It reads memory access requests (process IDs) from an input file
3. **Page Replacement**: When a page is not in RAM (page fault), it:
   - Checks for free frames in RAM
   - If no free frames exist, it evicts the least recently used page 
   - Loads the requested page into RAM
4. **Output**: After simulation, it outputs the state of page tables and RAM content

## Input Format

The input file contains space-separated integers, each representing a process ID (0-4) that is accessing memory. The simulation assumes sequential page access within each process.

Example: `0 2 1 4 3 2 2 0 2 1 4 4 4 0`

## Output Format

The program outputs:
1. Page tables for each process (0 = page in RAM, 1 = page in virtual memory)
2. Current RAM content showing process ID, page number, and last accessed time for each frame

Example:
```
Process 0: 1, 0, 0, 1
Process 1: 1, 0, 1, 1
Process 2: 1, 0, 0, 0
Process 3: 0, 1, 1, 1
Process 4: 1, 1, 1, 0
RAM Content:
0,2,13; 0,2,13; 2,3,8; 2,3,8; 1,1,9; 1,1,9; 4,3,12; 4,3,12; 3,0,4; 3,0,4; 2,1,5; 2,1,5; 2,2,6; 2,2,6; 0,1,7; 0,1,7
```

## Usage

```bash
# Compile the program
gcc -o simulation simulation.c

# Run the simulation
./simulation in.txt out.txt
```

Where:
- `in.txt` is the input file containing process access patterns
- `out.txt` is the output file where results will be written

## Key Concepts Demonstrated

- Memory management in operating systems
- Page replacement algorithms
- Virtual memory and physical memory mapping
- Process scheduling and memory access patterns

## Authors

- Yashwardhan Laharia (24295462)
- Zeel Vavliya (24264717, zeelvavliya@gmail.com)

## License

This project is part of CITS2002 System Programming coursework.
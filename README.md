# C-Based Virtual Machine Emulator

A lightweight, educational Virtual Machine (VM) emulator written in C. This project simulates a standard CPU architecture to execute custom bytecode, featuring a memory-mapped structure including registers, a stack, and a dynamic heap.

## How It Works
The VM processes programs using a classic **Fetch-Decode-Execute** cycle. It operates on a contiguous byte array that serves as the entire system memory, divided into segments for Data, Code, Heap, and Stack.



[Image of basic CPU fetch-decode-execute cycle]


## Features
* **Architecture:** Implements 4 general-purpose registers (`R0-R3`) and a flexible flags register (`FLAGS`) for conditional branching.
* **Memory Management:** Includes a basic `ALLOC` and `FREE` system for heap management and a stack-based structure for function calls (`CALL`/`RETURN`).
* **Instruction Set:** Supports over 40 opcodes, including arithmetic (`ADD`, `MUL`, `DIV`), bitwise operations (`AND`, `OR`, `SHL`), and conditional jumps (`JE`, `JL`, `JG`).
* **Extensible:** The modular design uses a lookup table of function pointers, making it simple to add new instructions.

## Quick Start
1.  **Clone the Repository** and open `main.c`.
2.  **Define your program** by populating the `program[]` byte array. Each instruction follows a 3-byte format: `[OPCODE, LEFT_OPERAND, RIGHT_OPERAND]`.
3.  **Compile** with a standard C compiler:
    ```bash
    gcc -o vm main.c
    ```
4.  **Execute** your bytecode:
    ```bash
    ./vm
    ```

## Project Structure
* **Memory Map:** The `program[]` array houses both your code and data. The `CODE_OFFSET` defines the entry point.
* **Registers:** Managed via `get_rx_value` and `set_rx_value` functions to ensure safe access.
* **Execution Loop:** The `main` function runs a continuous `while` loop, executing `cpu_fetch()`, `cpu_decode()`, and `cpu_execute()` until an `STP` (stop) or `HALT` instruction is reached.

# UVash: Custom Unix Shell

<div align="center">
  
  ![C](https://img.shields.io/badge/C-00599C?style=for-the-badge&logo=c&logoColor=white)
  ![Linux](https://img.shields.io/badge/Linux-FCC624?style=for-the-badge&logo=linux&logoColor=black)
  ![POSIX](https://img.shields.io/badge/API-POSIX_System_Calls-blue?style=for-the-badge)
  ![Academic](https://img.shields.io/badge/Academic_Project-2nd Year-purple?style=for-the-badge)

</div>

 A minimalist, high-performance Unix shell built in C using POSIX system calls. This project demonstrates low-level operating system concepts such as process creation, inter-process communication, and I/O redirection. It was made as a project for the **OS Structure** (2nd Year, 2nd Quarter) subject.

## Features

* **Interactive & Batch Modes:** Read commands dynamically from the user or process automation scripts via files.
* **Parallel Execution:** Execute multiple commands concurrently using the `&` operator (e.g., `ls & pwd & echo "Done"`). The shell efficiently forks processes and synchronizes using `waitpid`.
* **I/O Redirection:** Standard output and standard error redirection natively supported using the `>` operator (e.g., `ls -la > output.txt`).
* **Built-in Commands:** * `cd`: Native directory traversal.
  * `exit`: Graceful termination ensuring all memory is freed.
 
<p align="center">
  <img src="https://github.com/user-attachments/assets/598a07d8-590f-433a-bc11-aeb9f13efc23" alt="UVash Snapshot" width="75%" style="max-width: 100%; box-shadow: 0px 4px 10px rgba(0,0,0,0.5);" />
</p>

## Architecture

The shell is built around the fundamental POSIX API:
* `fork()`, `execvp()`, and `waitpid()` for process lifecycle management.
* `dup2()` and `open()` for file descriptor manipulation and routing.
* `getline()` and pointer arithmetic for zero-copy string parsing.

## Compilation
Use GCC to compile the source code:
```bash
gcc -Wall -Werror -O2 -o UVash UVash.c
```

## Usage

### Interactive Mode:

```bash
./UVash
UVash> ls -la & echo "Concurrent execution!"
```

### Batch Mode:

```bash
./UVash script.sh
```

**Original Author**

-Iván Moro Cienfuegos

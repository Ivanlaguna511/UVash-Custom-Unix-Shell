# UVash: Custom Unix Shell

A minimalist, high-performance Unix shell built in C using POSIX system calls. This project demonstrates low-level operating system concepts such as process creation, inter-process communication, and I/O redirection.

## Features

* **Interactive & Batch Modes:** Read commands dynamically from the user or process automation scripts via files.
* **Parallel Execution:** Execute multiple commands concurrently using the `&` operator (e.g., `ls & pwd & echo "Done"`). The shell efficiently forks processes and synchronizes using `waitpid`.
* **I/O Redirection:** Standard output and standard error redirection natively supported using the `>` operator (e.g., `ls -la > output.txt`).
* **Built-in Commands:** * `cd`: Native directory traversal.
  * `exit`: Graceful termination ensuring all memory is freed.
 
<img width="410" height="197" alt="snapshot" src="https://github.com/user-attachments/assets/598a07d8-590f-433a-bc11-aeb9f13efc23" />

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

## Compilation

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

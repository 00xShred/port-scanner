# Simple TCP Port Scanner

A minimal, single-threaded TCP port scanner written in C.
Designed for educational purposes to demonstrate the fundamentals of TCP connect-based scanning.

## Overview

The scanner tests a range of TCP ports on a target IP by attempting a full TCP connection (`connect()`).

**Key characteristics:**

- **Sequential:** Scans ports one by one (simple, but slow for large ranges).
- **Non-blocking I/O:** Uses non-blocking sockets with `select()` to avoid hanging.
- **Timeout-based:** Waits up to **1 second per port** before reporting a timeout.

## Build & Run

Compile:

```bash
gcc -o scanner scanner.c
```

Run:

```bash
./scanner <IP> <Start Port> <End Port>
```

Example:

```bash
./scanner 127.0.0.1 80 100
```

## Output Meaning

- `[OPEN]` — TCP connection succeeded (service listening)
- `[TIMEOUT]` — No response within timeout (likely filtered)
- `[ERROR]` — System or socket error
- _(No output)_ — Port closed (connection refused immediately)

## Example Use Cases

- **Verify local services**

  ```bash
  ./scanner 127.0.0.1 8080 8080
  ```

- **Check common service ports**

  ```bash
  ./scanner 127.0.0.1 22 3306
  ```

- **Detect filtered or non-responsive hosts**

  ```bash
  ./scanner 192.0.2.1 80 85
  ```

## Notes & Limitations

- Scanning large ranges (e.g. `1–65535`) is slow due to sequential execution.
- Uses a full TCP handshake (not stealthy).
- No service, version, or OS detection.
- Intended for learning, not production use.

---

**Disclaimer:**
Scan only systems you own or have explicit authorization to test. Unauthorized port scanning may be illegal.


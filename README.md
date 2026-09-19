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

## Limitations

- Sequential execution (slow for large ranges like `1-65535`).
- Full TCP handshake (connect-based, not stealthy).
- No service/version banner grabbing.

---

**Disclaimer:**
Scan only systems you own or have explicit authorization to test. Unauthorized port scanning may be illegal.


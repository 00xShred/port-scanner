#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>

volatile sig_atomic_t keep_running = 1;

void handle_signal(int sig) { keep_running = 0; }

int scan_port(const char *ip, int port, int timeout_sec);

int main(int argc, char *argv[]) {
  if (argc != 4) {
    printf("Usage: %s <IP Address> <Start Port> <End Port>\n", argv[0]);
    printf("Example: %s 192.168.1.1 1 100\n", argv[0]);
    return 1;
  }

  const char *target_ip = argv[1];
  int start_port = atoi(argv[2]);
  int end_port = atoi(argv[3]);
  const int TIMEOUT = 1;

  if (start_port <= 0 || end_port > 65535 || start_port > end_port) {
    printf("Error: Invalid port range [%d - %d].\n", start_port, end_port);
    return 1;
  }

  // Set up signal handler for graceful shutdown
  signal(SIGINT, handle_signal);
  signal(SIGTERM, handle_signal);

  printf("--- Starting Port Scan ---\n");
  printf("Target: %s | Range: %d to %d | Timeout: %d second(s)\n", target_ip,
         start_port, end_port, TIMEOUT);
  printf("Press Ctrl+C to stop the scan\n");
  printf("--------------------------\n");

  int open_ports = 0;
  int timeout_ports = 0;
  int error_ports = 0;

  // Loop through the requested range
  for (int port = start_port; port <= end_port && keep_running; port++) {
    int result = scan_port(target_ip, port, TIMEOUT);

    if (result == 0) {
      printf("[OPEN] %s:%d\n", target_ip, port);
      open_ports++;
    } else if (result == 1) {
      printf("[TIMEOUT] %s:%d\n", target_ip, port);
      timeout_ports++;
    } else if (result == -2) {
      printf("[ERROR] %s:%d - System error\n", target_ip, port);
      error_ports++;
    }

    // delay to avoid overwhelming the system
    usleep(1000);
  }

  printf("\n--- Scan Complete ---\n");
  printf("Summary: %d open, %d timed out, %d errors", open_ports, timeout_ports,
         error_ports);

  if (!keep_running) {
    printf(" (Scan interrupted by user)\n");
  } else {
    printf("\n");
  }

  return 0;
}

// Perform a single port scan with a timeout
int scan_port(const char *ip, int port, int timeout_sec) {
  int sock;
  struct sockaddr_in server_addr;

  // Create socket
  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    return -2; // System error
  }

  // Set to non-blocking mode
  if (fcntl(sock, F_SETFL, O_NONBLOCK) < 0) {
    close(sock);
    return -2;
  }

  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(port);
  if (inet_pton(AF_INET, ip, &server_addr.sin_addr) <= 0) {
    close(sock);
    return -2;
  }

  // Initiate non-blocking connect
  int res = connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr));

  if (res == 0) {
    close(sock);
    return 0;
  }

  if (res < 0 && errno != EINPROGRESS) {
    close(sock);
    return -1;
  }

  fd_set writefds;
  FD_ZERO(&writefds);
  FD_SET(sock, &writefds);

  struct timeval timeout;
  timeout.tv_sec = timeout_sec;
  timeout.tv_usec = 0;

  // Wait for socket to become ready for writing or timeout to expire
  int select_result = select(sock + 1, NULL, &writefds, NULL, &timeout);

  // Handle select() errors
  if (select_result < 0) {
    close(sock);
    return -2;
  }

  // Handle timeout
  if (select_result == 0) {
    close(sock);
    return 1;
  }

  if (!FD_ISSET(sock, &writefds)) {
    close(sock);
    return -2;
  }

  // Check if connection succeeded
  int so_error = 0;
  socklen_t len = sizeof(so_error);

  if (getsockopt(sock, SOL_SOCKET, SO_ERROR, &so_error, &len) < 0) {
    close(sock);
    return -2;
  }

  close(sock);

  return (so_error == 0) ? 0 : -1;
}

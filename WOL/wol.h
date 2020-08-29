

// language extension with same effect as include-guards
#pragma once


// include-guard with same effect as pragma once
#ifndef WOLHEADER
#define WOLHEADER


// cross platform enabling includes
#if defined(_WIN32)
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0600
#endif
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#endif


// cross platform enabling macros
#if defined(_WIN32)
#define ISVALIDSOCKET(s) ((s) != INVALID_SOCKET)
#define CLOSESOCKET(s) closesocket(s)
#define GETSOCKETERRNO() (WSAGetLastError())
#else
#define ISVALIDSOCKET(s) ((s) >= 0)
#define CLOSESOCKET(s) close(s)
#define SOCKET int
#define GETSOCKETERRNO() (errno)
#endif


// for enabling of IPv4+IPv6 dual stack sockets if needed
#if !defined(IPV6_V6ONLY)
#define IPV6_V6ONLY 27
#endif


// standard library includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// function prototypes
void print_invalidcall(void);
int is_valid_target(unsigned char*);
int is_macaddr(unsigned char*);


// macros
#ifndef MAG_PKG_LEN
#define MAG_PKG_LEN 102
#endif


#endif


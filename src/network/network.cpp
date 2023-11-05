#ifdef _WIN64 // WINDOWS
#ifndef _MSC_VER
#include <unistd.h>
#endif
#define _WINSOCK_DEPRECATED_NO_WARNINGS 1
#ifndef WINSOCK2_IMPORTED
#define WINSOCK2_IMPORTED
#include <winsock2.h>
#include <ws2tcpip.h>
#endif
#include <windows.h>
#else // NIX
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#endif // ...
#include "system_state.hpp"
#include "commands.hpp"
#include "SPSCQueue.h"
#include "network.hpp"
#include "serialization.hpp"

#define ZSTD_STATIC_LINKING_ONLY
#define XXH_NAMESPACE ZSTD_
#include "zstd.h"

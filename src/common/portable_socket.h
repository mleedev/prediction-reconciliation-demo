// portable_socket.h
// Portable socket abstraction layer

// What do I need to implement for sockets? (which ones are in this header)
//  [x] - initialize sockets
//  [ ] - create sockets
//  [ ] - bind server socket to port
//  [x] - set socket to non-blocking mode
//  [ ] - send packets
//  [ ] - receive packets
//  [ ] - error checks, handling
//  [x] - differentiate WSAEWOULDBLOCK vs. EWOULDBLOCK/EAGAIN
//  [x] - close/teardown of sockets

#ifndef PORTABLE_SOCKET_H_
#define PORTABLE_SOCKET_H_
// Windows include cleanup
#ifdef _WIN32
// <winsock2.h> internally pulls the <windows.h> dependency
// The WIN32_LEAN_AND_MEAN macro tells <windows.h> to exclude <winsock.h>
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
// NOMINMAX tells <windows.h> to not define legacy min/max macros
#ifndef NOMINMAX
#define NOMINMAX
#endif
#endif

// Header includes
#ifdef _WIN32
// Windows - WinSock headers
#include <winsock2.h>
#include <ws2tcpip.h> // inet_pton, inet_ntop, getaddrinfo
#else
// Linux/Mac - BSD Socket Headers
#include <sys/types.h>  // Historical standard, defensive include
#include <sys/socket.h> // Main socket header
#include <netinet/in.h> // sockaddr_in							- IPv4 socket address
#include <arpa/inet.h>  // inet_pton, inet_ntop					- network/presentation conversion
#include <unistd.h>     // close()
#include <fcntl.h>      // fcntl(sockfd, F_SETFL, O_NONBLOCK);	- Allow non-blocking
#include <errno.h>      // EWOULDBLOCK, EINPROGRESS, EAGAIN		- non-blocking errors
// Skip <netdb.h> since we aren't doing hostname resolution
#endif

// --- Abstracted types, names ---
// Socket handle is unsigned in Windows, signed in BSD - need to abstract type
#ifdef _WIN32
// Abstracted socket type
typedef SOCKET socket_t;
#else
// Abstracted socket type
typedef int socket_t;
#endif

// In windows we have to check for INVALID_SOCKET on socket() rather than -1 due to unsigned handle
//  We want to check for the same thing on both platforms, ideally
//  Define INVALID_SOCKET equivalent when using BSD
// Furthermore, on winsock, we check for SOCKET_ERROR on recvfrom() rather than -1
//  Define SOCKET_ERROR equivalent when using BSD
#ifndef _WIN32
#define INVALID_SOCKET (-1)
#define SOCKET_ERROR (-1)
#endif

// --- Initialize socket ---
/*
 * Socket Initialization
 * @return int error code if WSAStartup has an error
 */
inline int initializeSocket()
{
#ifdef _WIN32
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) return iResult;
    // Handle WSA v2.2 unsupported case
    if (wsaData.wVersion < MAKEWORD(2, 2))
    {
        WSACleanup();
        return WSAVERNOTSUPPORTED;
    }
#endif
    return 0;
}

// --- Create socket ---
// socket() is identical, 3 int params, on both platforms

// --- Bind server socket to port ---
// bind()'s first param is abstracted by us as socket_t
// bind()'s second param, a struct "sockaddr", is equivalent on both platforms
// bind()'s third param, an int is for the same address length value on both

// --- Set socket to non-blocking mode ---
/*
* Set socket to non-blocking mode
* Note that SOCK_NONBLOCK bitflag during socket creation is an option, 
*  but I wanted to go for safer coverage. It's also easier for me to read it back
*  when the non-blocking flag is set in a separate function.
* @param socket_t sock - socket to set to non-blocking mode
* @return bool - true if no error occurred, false if an error occurred
*/
inline bool setSocketNonblocking(socket_t sock)
{
#ifdef _WIN32
    u_long iMode = 1; // (iMode != 0) -> non-blocking mode enabled
    return ioctlsocket(sock, FIONBIO, &iMode) == 0;
#else
    // I almost did fcntl(sock, F_SETFL, O_NONBLOCK) by itself 
    //  but I learned that doing that function alone overwrites current flags.
    int flags = fcntl(sock, F_GETFL, 0);                    // Get the current flags
    if (flags < 0) return false;                               // Exit early if F_GETFL had an error
    return fcntl(sock, F_SETFL, flags | O_NONBLOCK) == 0;   // OR the non-blocking flag into current flags
#endif
}

// sendto() and recvfrom() are identical on both platforms

// --- WSAEWOULDBLOCK vs EWOULDBLOCK/EAGAIN ---
inline bool socketWouldBlock()
{
#ifdef _WIN32
    return WSAGetLastError() == WSAEWOULDBLOCK;
#else
    return errno == EWOULDBLOCK || errno == EAGAIN;
#endif
}

// --- Close sockets ---
// Socket close function is different on each platform
//  - winsock uses closesocket()
//  - BSD uses close()
// Alias "closesocket" to "close" in Unix
#ifndef _WIN32
inline int closesocket(socket_t sock)
{
    return close(sock);
}
#endif

// Teardown/Cleanup of sockets.
// BSD doesn't have a cleanup function.
inline int socketCleanup()
{
#ifdef _WIN32
    return WSACleanup();
#else
    return 0;
#endif
}

#endif // PORTABLE_SOCKET_H_
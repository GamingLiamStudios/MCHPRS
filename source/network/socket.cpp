#include "socket.h"

#include <exception>
#include <iostream>

#include "fmt/core.h"

#ifdef PLATFORM_WINDOWS

#define WIN32_LEAN_AND_MEAN
#define NOGDICAPMASKS        // CC_*, LC_*, PC_*, CP_*, TC_*, RC_
#define NOVIRTUALKEYCODES    // VK_*
#define NOWINMESSAGES        // WM_*, EM_*, LB_*, CB_*
#define NOWINSTYLES          // WS_*, CS_*, ES_*, LBS_*, SBS_*, CBS_*
#define NOSYSMETRICS         // SM_*
#define NOMENUS              // MF_*
#define NOICONS              // IDI_*
#define NOKEYSTATES          // MK_*
#define NOSYSCOMMANDS        // SC_*
#define NORASTEROPS          // Binary and Tertiary raster ops
#define NOSHOWWINDOW         // SW_*
#define OEMRESOURCE          // OEM Resource values
#define NOATOM               // Atom Manager routines
#define NOCLIPBOARD          // Clipboard routines
#define NOCOLOR              // Screen colors
#define NOCTLMGR             // Control and Dialog routines
#define NODRAWTEXT           // DrawText() and DT_*
#define NOGDI                // All GDI defines and routines
#define NOKERNEL             // All KERNEL defines and routines
#define NOUSER               // All USER defines and routines
#define NONLS                // All NLS defines and routines
#define NOMB                 // MB_* and MessageBox()
#define NOMEMMGR             // GMEM_*, LMEM_*, GHND, LHND, associated routines
#define NOMETAFILE           // typedef METAFILEPICT
#ifndef NOMINMAX             // Very quick fix
#define NOMINMAX             // Macros min(a,b) and max(a,b)
#endif
#define NOMSG               // typedef MSG and associated routines
#define NOOPENFILE          // OpenFile(), OemToAnsi, AnsiToOem, and OF_*
#define NOSCROLL            // SB_* and scrolling routines
#define NOSERVICE           // All Service Controller routines, SERVICE_ equates, etc.
#define NOSOUND             // Sound driver routines
#define NOTEXTMETRIC        // typedef TEXTMETRIC and associated routines
#define NOWH                // SetWindowsHook and WH_*
#define NOWINOFFSETS        // GWL_*, GCL_*, associated routines
#define NOCOMM              // COMM driver routines
#define NOKANJI             // Kanji support stuff.
#define NOHELP              // Help engine interface.
#define NOPROFILER          // Profiler interface.
#define NODEFERWINDOWPOS    // DeferWindowPos routines
#define NOMCX               // Modem Configuration Extensions

#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

#ifdef EWOULDBLOCK
#undef EWOULDBLOCK
#endif

#define EWOULDBLOCK WSAEWOULDBLOCK
#define get_error   WSAGetLastError
#elif PLATFORM_UNIX 1
#include <netdb.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <netinet/tcp.h>
#include <cerrno>
#include <fcntl.h>

#define INVALID_SOCKET -1
#define SOCKET_ERROR   -1
#define get_error()    errno
#endif

#ifdef _WINDOWS_

#endif

namespace
{
#ifdef PLATFORM_WINDOWS
    bool g_winsock_initialized;

    struct WinSockCleanup final
    {
        WinSockCleanup() { g_winsock_initialized = false; }

        ~WinSockCleanup() noexcept
        {
            g_winsock_initialized = false;
            WSACleanup();
        }
    } g_winsock_cleanup;    // RAII class to call WSACleanup on shutdown
#endif

    raw_socket_t new_socket()
    {
#ifdef PLATFORM_WINDOWS
        if (!g_winsock_initialized)    // Init winsock if this is the first created socket
        {
            WSADATA wsData;
            WORD    ver = MAKEWORD(2, 2);

            int ws0k = WSAStartup(ver, &wsData);

            if (ws0k != 0) std::__throw_runtime_error("Cannot init winsock");
            g_winsock_initialized = true;
        }
#endif

        return socket(AF_INET, SOCK_STREAM, 0);
    }

    std::string get_error_string(int errcode = get_error())
    {
#ifdef PLATFORM_WINDOWS
        std::string msg(256, '\0');

        FormatMessage(
          FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,    // flags
          NULL,                                                          // lpsource
          errcode,                                                       // message id
          MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),                     // languageid
          msg.data(),                                                    // output buffer
          msg.size(),                                                    // size of msgbuf, bytes
          NULL);                                                         // va_list of arguments

        if (!msg[0])
            sprintf(msg.data(), "%d", errcode);    // provide error # if no string available

        return msg;
#endif
        return std::to_string(errcode);
    }
}    // namespace

SocketBase::SocketBase() noexcept : _handle(new raw_socket_t(INVALID_SOCKET))
{
}

SocketBase::~SocketBase()
{
    if (_handle.use_count() <= 1)
    {
        if (is_valid()) { close(); }
        _handle.reset();
    }
}

bool SocketBase::operator==(const SocketBase &other) const noexcept
{
    return *_handle == *other._handle;
}

bool SocketBase::is_valid() const noexcept
{
    return *_handle != INVALID_SOCKET;
}

void SocketBase::close() noexcept
{
#ifdef PLATFORM_WINDOWS
    // See
    // https://docs.microsoft.com/en-us/troubleshoot/windows/win32/close-non-blocked-socket-memory-leak
    shutdown(*_handle, SD_BOTH);
    closesocket(*_handle);
#elif PLATFORM_UNIX 1
    ::close(_handle);
#endif

    *_handle = INVALID_SOCKET;

    std::cout << "Socket closed\n";
}

SocketBase::SocketBase(raw_socket_t handle) noexcept : _handle(new raw_socket_t(handle))
{
}

//
// CLIENT SOCKET
//

ClientSocket::ClientSocket() noexcept
{
}

int ClientSocket::read_bytes(uint8_t *buf, int buf_len) const
{
    int ret = recv(*_handle, reinterpret_cast<char *>(buf), buf_len, 0);
    if (ret < 0)    // Error handling
    {
        int err = get_error();
        return SOCK_NO_DATA;
    }
    return ret;
}

int ClientSocket::send_bytes(uint8_t const *buf, int buf_len) const noexcept
{
    int result = ::send(*_handle, reinterpret_cast<char const *>(buf), buf_len, 0);

    if (result < 0)
    {
        int err = get_error();
        std::cout << fmt::format(
          "Error occurred while sending bytes to client: \n{}: {}",
          err,
          get_error_string(err));
        return SOCK_ERROR;
    }
    return result;
}

bool ClientSocket::is_localhost() const noexcept
{
    sockaddr_in rem_addr {}, loc_addr {};
    uint32_t    len = sizeof(rem_addr);

#ifdef PLATFORM_WINDOWS
    getpeername(*_handle, (sockaddr *) &rem_addr, (int *) &len);
    getsockname(*_handle, (sockaddr *) &loc_addr, (int *) &len);
    return (rem_addr.sin_addr.S_un.S_addr == loc_addr.sin_addr.S_un.S_addr);
#elif PLATFORM_UNIX 1
    getpeername(*_handle, (sockaddr *) &rem_addr, &len);
    getsockname(*_handle, (sockaddr *) &loc_addr, &len);
    return (rem_addr.sin_addr.s_addr == loc_addr.sin_addr.s_addr);
#endif
}

ClientSocket ClientSocket::_from_raw_handle(raw_socket_t handle)
{
    return ClientSocket(handle);
}

ClientSocket::ClientSocket(raw_socket_t handle) noexcept : SocketBase(handle)
{
}

//
// SERVER SOCKET
//

ServerSocket ServerSocket::listen_on_port(uint16_t port)
{
    // 1. Create socket
    raw_socket_t handle = new_socket();
    if (handle == INVALID_SOCKET)
    {
        std::cout << "Failed to create socket: \n" << get_error_string() << "\n";
        return ServerSocket::invalid();
    }

    // 2. Bind socket to port
    sockaddr_in hint {};
    hint.sin_family = AF_INET;
    hint.sin_port   = htons(port);
#ifdef PLATFORM_WINDOWS
    hint.sin_addr.S_un.S_addr = INADDR_ANY;
#elif PLAFORM_UNIX
    hint.sin_addr.s_addr = INADDR_ANY;
#endif

    if (::bind(handle, reinterpret_cast<sockaddr *>(&hint), sizeof(hint)) < 0)
    {
        std::cout << "Could not bind socket! Error: \n" << get_error_string() << "\n";
        return ServerSocket::invalid();
    }

    int flag   = 1;
    int result = setsockopt(handle, IPPROTO_TCP, TCP_NODELAY, (char *) &flag, sizeof(flag));
    if (result < 0)
    {
        std::cout << "Could not disable nagle's algorithm! Error: \n" << get_error_string() << "\n";
        return ServerSocket::invalid();
    }

    // 3. Start listening on port
    if (listen(handle, SOMAXCONN) != 0)
    {
        std::cout
          << fmt::format("Failed to start listening on port {}: \n{}\n", port, get_error_string());
        return ServerSocket::invalid();
    }

    return ServerSocket(handle);
}

ClientSocket ServerSocket::accept_connection() const noexcept
{
    sockaddr_in client {};
    uint32_t    client_size = sizeof(client);

#ifdef PLATFORM_WINDOWS
    raw_socket_t new_handle = ::accept(
      *_handle,
      reinterpret_cast<sockaddr *>(&client),
      reinterpret_cast<int *>(&client_size));
#elif PLATFORM_UNIX 1
    raw_socket_t new_handle =
      ::accept(_handle, reinterpret_cast<sockaddr *>(&client), &client_size);
#endif

    return ClientSocket::_from_raw_handle(new_handle);
}

ServerSocket ServerSocket::invalid() noexcept
{
    return ServerSocket(INVALID_SOCKET);
}

ServerSocket::ServerSocket(raw_socket_t handle) noexcept : SocketBase(handle)
{
}

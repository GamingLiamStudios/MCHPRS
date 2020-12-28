#pragma once

#include <memory>
#include "util/types.h"

#if defined(_WIN32)
#define PLATFORM_WINDOWS
using raw_socket_t = u64;
#elif defined(unix) || defined(__unix__) || defined(__unix) || \
  (defined(__APPLE__) && defined(__MACH__))
#define PLATFORM_UNIX
using raw_socket_t = i32;
#else
#error "Unknown platform"
#endif

inline constexpr i32 SOCK_ERROR       = -2;
inline constexpr i32 SOCK_NO_DATA     = -1;
inline constexpr i32 SOCK_CONN_CLOSED = 0;

// RAII abstraction over platform-dependent socket stuff
class SocketBase
{
public:
    SocketBase() noexcept;    // Initialized to 'invalid' state

    ~SocketBase();

    bool operator==(const SocketBase &other) const noexcept;

    bool is_valid() const noexcept;

    void close() noexcept;

protected:
    explicit SocketBase(raw_socket_t handle) noexcept;

    std::shared_ptr<raw_socket_t> _handle;
};

class ClientSocket final : public SocketBase
{
public:
    friend class ServerSocket;

    ClientSocket() noexcept;    // Initializes to 'invalid'

    // Unlike berkeley sockets, this returns:
    // <= -2 if there was an error,
    //    -1 if there's no data,
    //     0 if connection closed and
    //     the number of bytes read otherwise
    i32 read_bytes(u8 *buf, i32 buf_len) const;

    i32 send_bytes(u8 const *buf, i32 buf_len) const noexcept;

    bool is_localhost() const noexcept;

private:
    static ClientSocket _from_raw_handle(raw_socket_t _handle);

    explicit ClientSocket(raw_socket_t) noexcept;
};

class ServerSocket final : public SocketBase
{
public:
    static ServerSocket listen_on_port(u16 port);

    [[nodiscard]] ClientSocket accept_connection() const noexcept;

private:
    static ServerSocket invalid() noexcept;

    ServerSocket(raw_socket_t handle) noexcept;
};

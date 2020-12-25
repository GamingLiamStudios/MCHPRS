#pragma once

#include <optional>
#include <string>
#include <memory>
#include <vector>
#include <atomic>
#include <cstdint>

#include "packets/serverbound.h"
#include "util/types.h"
#include "util/mpsc.h"

enum class NetworkState
{
    Handshake,
    Status,
    Login,
    Play
};

// This handles the TCP stream.
class NetworkClient
{
private:
    int                                 stream;    // TCP Stream
    mpsc::Receiver<ServerBoundPacket *> packets;
    std::shared_ptr<std::atomic_bool>   compressed;

    void listen(
      int                               stream,
      mpsc::Sender<ServerBoundPacket *> sender,
      std::shared_ptr<std::atomic_bool> compressed);

public:
    std::vector<ServerBoundPacket *> receive_packets();
    void                             set_compressed(bool compressed);
    // void send_packet(PacketEncoder &data); TODO
    void close_connection();

    // All NetworkClients are identified by this id.
    // If the client is a player, the player's entitiy id becomes the same.
    uint32_t                   id;
    NetworkState               state;
    bool                       alive;
    std::optional<std::string> username;
    std::optional<u128>        uuid;
};

// This represents the network portion of a minecraft server
class NetworkServer
{
private:
    mpsc::Receiver<NetworkClient> client_receiver;

    void listen(std::string bind_address, mpsc::Sender<NetworkClient> sender);

public:
    NetworkServer(std::string bind_address);
    void update();

    // These clients are either in the handshake, login, or ping state, once they
    // shift to play, they will be moved to a plot
    std::vector<NetworkClient> handshaking_clients;
};
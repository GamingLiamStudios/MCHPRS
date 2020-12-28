#include "network.h"

#include <exception>
#include <thread>

// NetworkClient

void NetworkClient::listen(
  ClientSocket &                    stream,
  mpsc::Sender<ServerBoundPacket *> sender,
  std::shared_ptr<std::atomic_bool> compressed)
{
    auto state = NetworkState::Handshake;
}

std::vector<ServerBoundPacket *> NetworkClient::receive_packets()
{
    std::vector<ServerBoundPacket *> packets;
    ServerBoundPacket *              packet;
    while (true)
    {
        if (!this->packets)
        {
            this->alive = false;
            break;
        }
        if (!this->packets.get()->try_dequeue(packet)) break;
        packets.push_back(std::move(packet));
    }
    return packets;
}

void NetworkClient::set_compressed(bool compressed)
{
    this->compressed.get()->store(compressed, std::memory_order_relaxed);
}

void NetworkClient::close_connection()
{
    this->alive = false;
}

// NetworkServer

void NetworkServer::listen(std::string bind_address, mpsc::Sender<NetworkClient> sender)
{
    auto         port_str = bind_address.substr(bind_address.find(':') + 1);
    ServerSocket socket   = ServerSocket::listen_on_port(std::stoi(port_str));

    size_t total_connections = 0;
    while (socket.is_valid())
    {
        auto stream = socket.accept_connection();
        if (!stream.is_valid()) break;

        auto packet_receiver = std::make_shared<moodycamel::ConcurrentQueue<NetworkClient>>();
        auto compressed      = std::make_shared<std::atomic_bool>(false);

        std::thread cthread(
          &NetworkClient::listen,
          stream,
          std::weak_ptr(packet_receiver),
          compressed);

        if (sender.expired()) break;
        sender.lock().get()->enqueue(NetworkClient(
          (u32) total_connections,
          std::move(stream),
          NetworkState::Handshake,
          std::move(packet_receiver),
          true,
          std::move(compressed),
          std::optional<std::string>(),
          std::optional<u128>()));

        total_connections++;
    }
}

NetworkServer::NetworkServer(std::string bind_address)
{
    this->client_receiver = std::make_shared<moodycamel::ConcurrentQueue<NetworkClient>>();
    std::thread nslisten(
      &NetworkServer::listen,
      std::move(bind_address),
      std::weak_ptr(client_receiver));
}

void NetworkServer::update()
{
    while (true)
    {
        if (!this->client_receiver)
            std::__throw_logic_error("Client receiver channel disconnected!");
        NetworkClient client;
        if (!this->client_receiver.get()->try_dequeue(client)) break;
        this->handshaking_clients.push_back(std::move(client));
    }
}
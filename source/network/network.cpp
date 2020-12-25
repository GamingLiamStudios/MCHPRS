#include "network.h"

#include <exception>
#include <thread>

// NetworkClient

void listen(
  int                               stream,
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
        if (!this->packets) return;
        if (!this->packets.get()->try_dequeue(packet)) break;
        packets.push_back(std::move(packet));
    }
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

void listen(std::string bind_address, mpsc::Sender<NetworkClient> sender)
{
}

NetworkServer::NetworkServer(std::string bind_address)
{
    this->client_receiver = std::make_shared<moodycamel::ConcurrentQueue<NetworkClient>>();
    std::thread nslisten(
      NetworkServer::listen,
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
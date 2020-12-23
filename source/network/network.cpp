#include "network.h"

#include <exception>

std::vector<ServerBoundPacket *> NetworkClient::receive_packets()
{
    throw std::logic_error("Function not yet Implemented");    // TODO
}

void NetworkClient::set_compressed(bool compressed)
{
    this->compressed.get()->store(compressed, std::memory_order_relaxed);
}

void NetworkClient::close_connection()
{
    this->alive = false;
}
#pragma once

#include <string>
#include <optional>
#include <vector>
#include <hash_map>

#include "player.h"
#include "network/network.h"

#include "util/types.h"
#include "util/mpsc.h"

namespace
{
    struct PlayerJoinInfo
    {
        std::string                username;
        u128                       uuid;
        std::optional<std::string> skin;
    };

    struct PlayerListEntry
    {
        i32                        plot_x;
        i32                        plot_z;
        std::string                username;
        u128                       uuid;
        Gamemode                   gamemode;
        std::optional<std::string> skin;
    };

    struct PlotListEntry
    {
        i32 plot_x;
        i32 plot_z;
        // mpsc::Sender<PrivMessage> priv_message_sender; // TODO
    };

}    // namespace

struct MinecraftServer
{
    NetworkServer network;
    /* TODO
    broadcaster: Bus<BroadcastMessage>,
    receiver: Receiver<Message>,
    plot_sender: Sender<Message>,
    */
    std::unordered_map<u128, PlayerListEntry> online_players;
    std::vector<PlotListEntry>                running_plots;
};

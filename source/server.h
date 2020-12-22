#pragma once

#include <string>
#include <optional>

#include "util.h"
#include "player.h"

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
        int                        plot_x;
        int                        plot_z;
        std::string                username;
        u128                       uuid;
        Gamemode                   gamemode;
        std::optional<std::string> skin;
    };

    struct PlotListEntry
    {
        int plot_x : i32;
        int plot_z : i32;
        // TODO: Some sort of inter-thread messaging system
        // priv_message_sender : mpsc::Sender<PrivMessage>
    };

}    // namespace

/* TODO: Some Networking stuff
struct MinecraftServer
{
    network: NetworkServer,
    broadcaster: Bus<BroadcastMessage>,
    receiver: Receiver<Message>,
    plot_sender: Sender<Message>,
    online_players: HashMap<u128, PlayerListEntry>,
    running_plots: Vec<PlotListEntry>,
};
*/

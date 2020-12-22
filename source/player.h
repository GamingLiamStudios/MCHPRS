#pragma once

#include <cstdint>
#include <optional>
#include <vector>

#include "util.h"

enum class Gamemode
{
    Creative  = 1,
    Spectator = 3
};

struct InventoryEntry
{
    uint32_t                            id;
    char                                slot;
    char                                count;
    short                               damage;
    std::optional<std::vector<uint8_t>> nbt;
};

struct PlayerData
{
    bool                        on_ground;
    bool                        flying;
    f64x3                       motion;
    f64x3                       position;
    f32x2                       rotation;
    std::vector<InventoryEntry> inventory;
    int                         selected_item_slot;
    float                       fly_speed;
    float                       walk_speed;
    Gamemode                    gamemode;
}

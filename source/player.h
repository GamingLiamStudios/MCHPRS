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
    u32                            id;
    i8                             slot;
    i8                             count;
    i16                            damage;
    std::optional<std::vector<u8>> nbt;
};

struct PlayerData
{
    bool                        on_ground;
    bool                        flying;
    f64x3                       motion;
    f64x3                       position;
    f32x2                       rotation;
    std::vector<InventoryEntry> inventory;
    i32                         selected_item_slot;
    float                       fly_speed;
    float                       walk_speed;
    Gamemode                    gamemode;
}

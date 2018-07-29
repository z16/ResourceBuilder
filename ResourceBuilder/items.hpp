#pragma once

#include "resource_parser.hpp"

#include <string>
#include <map>
#include <filesystem>
#include <optional>
#include <cstdint>

struct item
{
    std::uint16_t id;
    std::optional<std::uint16_t> flags;
    std::optional<std::uint16_t> stack;
    std::optional<std::uint16_t> type;
    std::optional<std::uint16_t> targets;
    std::optional<std::uint16_t> level;
    std::optional<std::uint16_t> slots;
    std::optional<std::uint16_t> races;
    std::optional<std::uint32_t> jobs;
    std::optional<std::uint16_t> superior_level;
    std::optional<std::uint16_t> shield_size;
    std::optional<std::uint8_t> max_charges;
    std::optional<std::uint8_t> cast_time;
    std::optional<std::uint16_t> cast_delay;
    std::optional<std::uint32_t> recast_delay;
    std::optional<std::uint8_t> item_level;
    std::optional<std::uint16_t> damage;
    std::optional<std::uint16_t> delay;
    std::optional<std::uint8_t> skill;
    std::optional<std::map<std::uint16_t, std::int8_t>> tp_moves;
    std::optional<std::string> en;
    std::optional<std::string> enp;
    std::optional<std::string> enl;
    std::optional<std::string> endesc;
};

class items
{
public:
    items(std::filesystem::path);

    bool contains(int) const;
    item const& operator[](int) const;

private:
    std::map<int, item> data;

    std::filesystem::path resources_path;
};

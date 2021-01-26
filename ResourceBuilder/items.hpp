#pragma once

#include "resource_parser.hpp"

#include <cstdint>
#include <string>
#include <map>
#include <filesystem>
#include <optional>
#include <cstdint>

struct item {
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

class items {
public:
	static const std::size_t entry_size = 0xC00;
	static const std::size_t string_offset = 0x280;
	static const std::size_t string_size = entry_size - string_offset;

	items(std::filesystem::path);

	bool contains(std::uint32_t) const;
	item const& operator[](std::uint32_t) const;

private:
	std::map<std::uint32_t, item> data;

	std::filesystem::path resources_path;
};

#include "items.hpp"

#include <string>
#include <map>
#include <string_view>
#include <algorithm>
#include <cctype>
#include <filesystem>

items::items(std::filesystem::path windower_path) :
    resources_path{std::move(windower_path)}
{
    parse_resources<item>(resources_path / "items.lua", [&](item& item, std::string const& key, std::string const& value)
    {
             if (key == "id")             item.id             = std::stoi(value);
        else if (key == "en")             item.en             = value;
        else if (key == "enp")            item.enp            = value;
        else if (key == "enl")            item.enl            = value;
        else if (key == "flags")          item.flags          = std::stoi(value);
        else if (key == "stack")          item.stack          = std::stoi(value);
        else if (key == "type")           item.type           = std::stoi(value);
        else if (key == "targets")        item.targets        = std::stoi(value);
        else if (key == "level")          item.level          = std::stoi(value);
        else if (key == "slots")          item.slots          = std::stoi(value);
        else if (key == "races")          item.races          = std::stoi(value);
        else if (key == "jobs")           item.jobs           = std::stoi(value);
        else if (key == "superior_level") item.superior_level = std::stoi(value);
        else if (key == "shield_size")    item.shield_size    = std::stoi(value);
        else if (key == "max_charges")    item.max_charges    = std::stoi(value);
        else if (key == "cast_time")      item.cast_time      = std::stoi(value);
        else if (key == "cast_delay")     item.cast_delay     = std::stoi(value);
        else if (key == "recast_delay")   item.recast_delay   = std::stoi(value);
        else if (key == "item_level")     item.item_level     = std::stoi(value);
        else if (key == "damage")         item.damage         = std::stoi(value);
        else if (key == "delay")          item.delay          = std::stoi(value);
        else if (key == "skill")          item.skill          = std::stoi(value);
    }, data);

    parse_resources<item>(resources_path / "item_descriptions.lua", [&](item& item, std::string const& key, std::string const& value)
    {
             if (key == "en")             item.endesc         = value;
    }, data);
}

bool items::contains(int id) const
{
    return data.find(id) != data.end();
}

item const& items::operator[](int const index) const
{
    return data.at(index);
}

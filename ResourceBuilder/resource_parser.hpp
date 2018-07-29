#pragma once

#include <map>
#include <sstream>
#include <fstream>
#include <utility>
#include <functional>
#include <string_view>
#include <filesystem>

template<typename T>
void parse_resources(std::filesystem::path const& path, std::function<void(T&, std::string_view, std::string const&)> mapper, std::map<int, T>& map)
{
    if (!std::filesystem::exists(path))
        return;

    std::ifstream file{path};
    std::string line;
    while (std::getline(file, line))
    {
        if (line.compare(0, 5, "    [") != 0)
        {
            continue;
        }

        auto id_start = line.find('[') + 1;
        auto id = std::stoi(line.substr(id_start, line.find(']') - id_start));
        auto found = map.find(id) != map.end();
        if (!found)
        {
            T entry{};
            entry.id = id;
            map[id] = entry;
        }
        T& entry = map.at(id);

        auto index = line.find('{') + 1;

        std::string_view key;

        auto quote = false;
        auto escaped = false;
        auto key_index = index;
        std::size_t value_index = 0;
        std::size_t quote_end = 0;
        for (; index < line.size(); ++index)
        {
            auto c = line[index];
            if (quote && c != '"')
            {
                continue;
            }

            switch (c)
            {
            case '\\':
                escaped = true;
                ++index;
                break;

            case '"':
                if (!quote && value_index == index)
                {
                    quote = true;
                    ++value_index;
                }
                else if (quote)
                {
                    quote_end = index;
                    quote = false;
                }
                break;

            case '=':
                key = {line.c_str() + key_index, index - key_index};
                value_index = index + 1;
                break;

            case ',':
            case '}':
                if (quote_end > value_index && quote_end < index - 1)
                {
                    break;
                }

                if (!escaped)
                {
                    mapper(entry, key, {line.c_str() + value_index, (quote_end > value_index ? quote_end : index) - value_index});
                }
                else
                {
                    std::string value{line.c_str() + value_index, (quote_end > value_index ? quote_end : index) - value_index};
                    std::size_t pos = 0;
                    while ((pos = value.find("\\\"", pos)) != std::string::npos)
                    {
                        value.replace(pos, 2, "\"");
                    }
                    pos = 0;
                    while ((pos = value.find("\\\n", pos)) != std::string::npos)
                    {
                        value.replace(pos, 2, "\n");
                    }
                    mapper(entry, key, value);
                }
                if (c == '}')
                {
                    break;
                }
                key_index = index + 1;
                quote_end = 0;
                break;
            }
        }
    }
}

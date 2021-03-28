#pragma once

#include <cstdint>
#include <map>
#include <sstream>
#include <fstream>
#include <utility>
#include <functional>
#include <string_view>
#include <filesystem>

template<typename T>
void parse_resources(std::filesystem::path const& path, std::function<void(T&, std::string const&, std::string const&)> mapper, std::map<std::uint32_t, T>& map) {
	if (!std::filesystem::exists(path)) {
		return;
	}

	std::ifstream file{path};
	std::string line;
	while (std::getline(file, line)) {
		if (line.compare(0, 5, "    [") != 0) {
			continue;
		}

		auto id_start = line.find('[') + 1;
		auto id_end = line.find(']');
		auto id = std::stoi(line.substr(id_start, id_end - id_start));
		auto found = map.find(id) != map.end();
		if (!found) {
			T entry{};
			entry.id = id;
			map[id] = entry;
		}
		T& entry = map.at(id);

		std::string key, value;
		auto current = &key;

		auto in_quote = false;
		auto in_escape = false;
		for (auto c : std::string_view{line.c_str() + id_end + 5}) {
			if (in_escape) {
				if (c == 'n') {
					current->push_back('\n');
				} else {
					if (c == '"') {
						in_quote = !in_quote;
					}

					current->push_back(c);
				}

				in_escape = false;
				continue;
			}

			if (c == '\\') {
				in_escape = true;
				continue;
			}

			if (in_quote) {
				if (c == '"') {
					in_quote = false;
				} else {
					current->push_back(c);
				}
				continue;
			}

			if (c == '"') {
				in_quote = true;
				continue;
			}

			if (c == '=') {
				current = &value;
				continue;
			}

			if (c == ',' || c == '}') {
				mapper(entry, key, value);
				if (c == '}')
					break;

				key = {};
				value = {};
				current = &key;
				continue;
			}

			current->push_back(c);
		}
	}
}

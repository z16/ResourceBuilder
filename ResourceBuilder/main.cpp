#include "items.hpp"

#include <cstdint>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <vector>
#include <array>
#include <iterator>

auto validate = false;

struct file {
	std::filesystem::path directory;
	std::string filename;
	std::uint16_t min_id;
	std::uint16_t max_id;
	std::streamoff string_offset;
};

std::vector<file> files{
	{"ROM\\118", "106.DAT", 0x0000, 0x1000, 0x18},
	{"ROM\\118", "107.DAT", 0x1000, 0x2000, 0x1C},
	{"ROM\\118", "110.DAT", 0x2000, 0x2200, 0x18},
	{"ROM\\301", "115.DAT", 0x2200, 0x2800, 0x18},
	{"ROM\\118", "109.DAT", 0x2800, 0x4000, 0x2C},
	{"ROM\\118", "108.DAT", 0x4000, 0x5A00, 0x38},
	{"ROM\\286", "73.DAT",  0x5A00, 0x7000, 0x2C},
	{"ROM\\217", "21.DAT",  0x7000, 0x7400, 0x54},
	//{"ROM\\288", "67.DAT",  0xF000, 0xF200, 0x70},
	//{"ROM\\174", "48.DAT",  0xFFFF, 0x0000, 0x10},
};

void read_into(std::ifstream& in, std::uint8_t* buffer, std::size_t size) {
	auto buffer_data = reinterpret_cast<char*>(buffer);
	in.read(buffer_data, size);
	for (std::size_t i = 0; i < size; ++i) {
		buffer[i] = buffer[i] << 3 | buffer[i] >> 5;
	}
}

template<std::size_t N>
void read_into(std::ifstream& in, std::array<std::uint8_t, N>& buffer) {
	read_into(in, buffer.data(), N);
}

void write_from(std::ofstream& out, std::uint8_t const* buffer, std::size_t size) {
	for (std::size_t i = 0; i < size; ++i) {
		out.put(buffer[i] << 5 | buffer[i] >> 3);
	}
}

template<std::size_t N>
void write_from(std::ofstream& out, std::array<std::uint8_t, N>& buffer) {
	write_from(out, buffer.data(), N);
}

template<typename T>
void write_basic(std::ofstream& out, T const& value, std::ifstream& in) {
	write_from(out, reinterpret_cast<std::uint8_t const*>(&value), sizeof(T));
	in.seekg(sizeof value, std::ios_base::cur);
}

template<std::size_t N>
void advance(std::ofstream& out, std::ifstream& in, std::streamoff offset = N) {
	static auto buffer = std::array<char, N>{};
	buffer = {};
	in.read(buffer.data(), offset);
	out.write(buffer.data(), offset);
}

template<std::size_t N>
void seek(std::ofstream& out, std::streampos pos, std::ifstream& in) {
	advance<N>(out, in, pos - out.tellp());
}

template<typename T>
void write(std::ofstream& out, std::optional<T> const& entry, std::ifstream& in) {
	if (!entry.has_value()) {
		advance<sizeof entry.value()>(out, in);
		return;
	}

	write_basic(out, entry.value(), in);
}

template<typename T>
void write(std::ofstream& out, std::optional<T> const& entry, std::ifstream& in, std::function<T(T)> selector) {
	if (!entry.has_value()) {
		advance<sizeof entry.value()>(out, in);
		return;
	}

	T value = selector(entry.value());
	write_basic(out, value, in);
}

void write_usable(std::ofstream& out, item const& item, std::ifstream& in) {
	write<std::uint8_t>(out, item.cast_time, in, [](auto time) { return time * 4; });
}

void write_armor(std::ofstream& out, item const& item, std::ifstream& in) {
	write(out, item.level, in);
	write(out, item.slots, in);
	write(out, item.races, in);
	write(out, item.jobs, in);

	write(out, item.superior_level, in);
	advance<1>(out, in);
	write(out, item.shield_size, in);
	write(out, item.max_charges, in);
	write<std::uint8_t>(out, item.cast_time, in, [](auto time) { return time * 4; });
	write(out, item.cast_delay, in);
	write(out, item.recast_delay, in);
	advance<2>(out, in);
	write(out, item.item_level, in);
}

void write_weapon(std::ofstream& out, item const& item, std::ifstream& in) {
	write(out, item.level, in);
	write(out, item.slots, in);
	write(out, item.races, in);
	write(out, item.jobs, in);

	advance<4>(out, in);
	write(out, item.damage, in);
	write(out, item.delay, in);
	advance<2>(out, in);
	write(out, item.skill, in);
	advance<5>(out, in);
	write(out, item.max_charges, in);
	write<std::uint8_t>(out, item.cast_time, in, [](auto time) { return time * 4; });
	write(out, item.cast_delay, in);
	write(out, item.recast_delay, in);
	advance<2>(out, in);
	write(out, item.item_level, in);
}

template<typename T>
T read(std::ifstream& in) {
	static auto buffer = std::array<std::uint8_t, sizeof(T)>{};
	static auto buffer_value_ptr = reinterpret_cast<T const*>(buffer.data());
	buffer = {};
	read_into(in, buffer);

	return *buffer_value_ptr;
}

template<typename T>
void write(std::ofstream& out, T const& value) {
	write_from(out, reinterpret_cast<std::uint8_t const*>(&value), sizeof T);
}

template<std::size_t N>
void write(std::ofstream& out, char const* value, std::size_t size = N) {
	write_from(out, reinterpret_cast<std::uint8_t const*>(value), size);
}

enum class string_type : std::uint32_t {
	string = 0,
	other = 1,
};

struct string_entry {
	std::uint32_t offset;
	string_type type;
	std::string value;

	string_entry(std::uint32_t offset, string_type type) : offset{offset}, type{type} {
	}
};

void print() {
	std::cout << std::endl;
}

template<typename T, typename... Args>
void print(T value, Args... rest) {
	std::cout << value << "\t";

	print(rest...);
}

template<typename T, typename... Args>
void assert(bool condition, T id, Args... args) {
	if (condition) {
		return;
	}

	std::cout << "  > violation for " << id << ": ";
	print(args...);
}

std::size_t index(std::ifstream& in) {
	return in.tellg() % ::items::entry_size;
}

void adjust_entry(string_entry& entry, std::optional<std::string> const& option) {
	if (!option.has_value()) {
		return;
	}

	auto const& value = option.value();

	auto size = 0x1C + value.size() + 4 & ~3;
	auto adjusted = std::string(size, '\0');
	std::copy_n(entry.value.cbegin(), 0x1C, adjusted.begin());
	std::copy(value.cbegin(), value.cend(), adjusted.begin() + 0x1C);
}

void write_strings(std::ofstream& out, item const& item, std::ifstream& in) {
	auto string_offset = index(in);

	auto count = read<std::uint32_t>(in);
	std::vector<string_entry> table;
	table.reserve(count);
	for (std::size_t i = 0; i < count; ++i) {
		auto offset = read<std::uint32_t>(in);
		auto type = read<string_type>(in);
		table.emplace_back(offset, type);
	}

	static auto buffer = std::array<std::uint8_t, ::items::string_size>{};
	static auto buffer_data = reinterpret_cast<char*>(buffer.data());
	for (std::size_t i = 0; i < count; ++i) {
		auto start = index(in);
		auto& entry = table[i];
		auto max = i + 1 < count
			? static_cast<std::size_t>(table[i + 1].offset) + string_offset
			: ::items::string_offset;

		buffer = {};

		auto buffer_size = max - start;
		read_into(in, buffer.data(), buffer_size);

		entry.value = {buffer_data, buffer_size};

		if (validate) {
			if (entry.type == string_type::string) {
				assert(buffer_size >= 0x1C, item.id, "String buffer too small", i);
				assert(buffer[0] == 1, item.id, "String buffer start not 0x01", i);
				for (auto j = 1; j < std::min(buffer_size, static_cast<std::size_t>(0x1C)); ++j) {
					assert(buffer[j] == 0, item.id, "String buffer not 0x00", i, j);
				}

				if (i + 1 < count) {
					auto length = strlen(entry.value.data() + 0x1C);
					assert((length + 4 & ~3) == buffer_size - 0x1C, item.id, "Mismatched string size", i);
				}
			} else if (entry.type == string_type::other) {
				assert(buffer_size == 4, item.id, "Other buffer > 4", i);
			} else {
				assert(false, item.id, "Other other?");
			}
		}
	}

	write(out, count);

	auto offset = static_cast<std::uint32_t>(sizeof count + count * (sizeof string_entry::offset + sizeof string_entry::type));
	for (std::size_t i = 0; i < count; ++i) {
		auto& entry = table[i];

		entry.offset = offset;

		switch (i) {
		case 0: // Name
			adjust_entry(entry, item.en);
			break;
		case 1: // Unknown
			break;
		case 2: // Log
			adjust_entry(entry, item.enl);
			break;
		case 3: // Plural
			adjust_entry(entry, item.enp);
			break;
		case 4: // Description
			adjust_entry(entry, item.endesc);
			break;
		}

		offset += static_cast<std::uint32_t>(entry.value.size());
	}

	for (auto const& entry : table) {
		write(out, entry.offset);
		write(out, entry.type);
	}

	for (auto const& entry : table) {
		write<::items::string_size>(out, entry.value.data(), entry.value.size());
	}

	out.seekp(::items::string_offset - out.tellp() % ::items::entry_size, std::ios_base::cur);
}

void write_item(std::ofstream& out, item const& item, std::streamoff string_offset, std::ifstream& in) {
	auto origin = out.tellp();
	write_basic(out, item.id, in);
	advance<2>(out, in);

	write(out, item.flags, in);
	write(out, item.stack, in);
	write(out, item.type, in);
	advance<2>(out, in);
	write(out, item.targets, in);

	if (item.id <= 0x0FFF) {
		// General
	} else if (item.id < 0x2000) {
		// Usable
		write_usable(out, item, in);
	} else if (item.id < 0x2200) {
		// Automaton
	} else if (item.id < 0x2800) {
		// General
	} else if (item.id < 0x4000) {
		// Armor
		write_armor(out, item, in);
	} else if (item.id < 0x5A00) {
		// Weapon
		write_weapon(out, item, in);
	} else if (item.id < 0x7000) {
		// Armor
		write_armor(out, item, in);
	} else if (item.id < 0x7400) {
		// Maze
	}

	seek<::items::entry_size>(out, origin + string_offset, in);

	write_strings(out, item, in);
}

int main(int argc, char** argv) {
	auto resources_path = std::filesystem::path{argv[1]};
	auto pol_path = std::filesystem::path{argv[2]};

	auto decode = false;
	auto backup = false;
	auto restore = false;

	for (auto i = 3; i < argc; ++i) {
		auto arg = std::string{argv[i]};
		if (arg == "--validate") {
			validate = true;
		} else if (arg == "--decode") {
			decode = true;
		} else if (arg == "--backup") {
			backup = true;
		} else if (arg == "--restore") {
			restore = true;
		}
	}

	std::cout << std::endl;

	if (restore) {
		for (auto const& file : files) {
			auto backup_file = resources_path / "backup" / file.directory / file.filename;
			if (!std::filesystem::exists(backup_file)) {
				std::cout << "No backup file found for " << (file.directory / file.filename).string() << std::endl;
				continue;
			}

			std::filesystem::copy_file(backup_file, pol_path / file.directory / file.filename, std::filesystem::copy_options::overwrite_existing);
			std::cout << "Restored " << (file.directory / file.filename).string() << std::endl;
		}
	} else {
		auto items = ::items{resources_path};

		for (auto const& file : files) {
			std::cout << "Doing " << (file.directory / file.filename).string() << std::endl;
			std::cout << "  > Reading from: " << (pol_path / file.directory / file.filename).string() << std::endl;
			std::cout << "  > Writing to: " << (resources_path / "results" / file.directory / file.filename).string() << std::endl;

			auto out_directory = resources_path / "results" / file.directory;
			std::filesystem::create_directories(out_directory);

			auto in = std::ifstream{pol_path / file.directory / file.filename, std::ios::binary};
			auto out = std::ofstream{out_directory / file.filename, std::ios::binary};

			for (auto i = file.min_id; i < file.max_id; ++i) {
				if (items.contains(i)) {
					write_item(out, items[i], file.string_offset, in);
				}

				seek<::items::entry_size>(out, ::items::entry_size * (static_cast<std::streamoff>(i - file.min_id) + 1), in);
			}

			if (validate && file.min_id != 0xFFFF) {
				auto current = in.tellg();
				in.seekg(0, std::ios_base::end);
				auto last = in.tellg();
				assert(current == last, "file size", "Input file not fully processed", last - current, static_cast<double>(last - current) / ::items::entry_size);
			}
		}

		if (decode) {
			std::cout << std::endl;

			for (auto const& file : files) {
				auto decoded_directory = resources_path / "decoded" / file.directory;
				std::filesystem::create_directories(decoded_directory);

				auto out = std::ifstream{resources_path / "results" / file.directory / file.filename, std::ios::binary};
				auto decoded = std::ofstream{decoded_directory / file.filename, std::ios::binary};

				auto buffer = std::array<std::uint8_t, ::items::entry_size>{};
				auto buffer_data = reinterpret_cast<char const*>(buffer.data());
				for (auto i = file.min_id; i < file.max_id; ++i) {
					buffer = {};
					read_into(out, buffer);
					decoded.write(buffer_data, sizeof buffer);
				}

				std::cout << "Decoded " << (decoded_directory / file.filename).string() << std::endl;
			}
		}

		if (backup) {
			std::cout << std::endl;

			for (auto const& file : files) {
				auto backup_directory = resources_path / "backup" / file.directory;
				std::filesystem::create_directories(backup_directory);

				std::filesystem::copy_file(pol_path / file.directory / file.filename, backup_directory / file.filename, std::filesystem::copy_options::overwrite_existing);

				std::cout << "Backed up " << (backup_directory / file.filename).string() << std::endl;
			}
		}
	}

	std::cout << std::endl;

	std::cout << "Done!" << std::endl;

	std::cin.get();
}

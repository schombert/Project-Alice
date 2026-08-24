#pragma once
#include <vector>
#include <functional>
#include <cstring>
#include <stdint.h>
#include <string>
#include <string_view>
#include <array>
#include <span>
#include <bit>
#include <assert.h>

namespace serialization {


class out_buffer {
private:
	std::vector<char> data_;
	std::vector<std::pair<size_t, std::function<void(out_buffer&)>>> pending_writes;
	std::vector<size_t> open_sections;
public:
	char const* data() const {
		return data_.data();
	}
	size_t size() const {
		return data_.size();
	}
	void finish_pending() {
		while(!pending_writes.empty()) {
			auto relocation_address = data_.data() + pending_writes.back().first;
			uint32_t new_address = uint32_t(data_.size());
			std::memcpy(relocation_address, &new_address, sizeof(uint32_t));

			pending_writes.back().second(*this);

			pending_writes.pop_back();
		}
	}
	void start_section() {
		auto sz_address = data_.size();
		uint32_t placeholder = 0;
		write(placeholder);
		open_sections.emplace_back(sz_address);
	}
	void finish_section() {
		assert(!open_sections.empty());

		auto size_address = data_.data() + open_sections.back();
		uint32_t bytes_in_section = uint32_t(data_.size() - open_sections.back());
		std::memcpy(size_address, &bytes_in_section, sizeof(uint32_t));

		open_sections.pop_back();
	}
	void finalize() {
		finish_pending();
		while(open_sections.empty() == false) {
			finish_section();
		}
	}

	template<typename T>
	void write(T const& d) {
		auto start_size = data_.size();
		data_.resize(start_size + sizeof(T), 0);
		std::memcpy(data_.data() + start_size, &d, sizeof(T));
	}
	template<typename T>
	void write_fixed(T const* d, size_t count) {
		auto start_size = data_.size();
		data_.resize(start_size + sizeof(T) * count, 0);
		std::memcpy(data_.data() + start_size, d, sizeof(T) * count);
	}
	template<typename T>
	void write_variable(T const* d, size_t count) {
		uint32_t c = uint32_t(count);
		write(c);
		write_fixed(d, count);
	}
	void write_relocation(std::function<void(out_buffer&)>&& f) {
		auto reloc_address = data_.size();
		uint32_t placeholder = 0;
		write(placeholder);
		pending_writes.emplace_back(reloc_address, std::move(f));
	}
	void write_relocation(size_t reloc_address, std::function<void(out_buffer&)>&& f) {
		pending_writes.emplace_back(reloc_address, std::move(f));
	}
	size_t get_data_position() const {
		return data_.size();
	}
	void write(std::string_view sv) {
		write_variable(sv.data(), sv.length());
	}
	void write(std::string const& s) {
		write_variable(s.data(), s.length());
	}
	void write(std::wstring_view sv) {
		write_variable(sv.data(), sv.length());
	}
	void write(std::wstring const& s) {
		write_variable(s.data(), s.length());
	}
};

class in_buffer {
private:
	char const* data;
	size_t size;
	size_t read_position = 0;
public:
	in_buffer(char const* data, size_t size) : data(data), size(size) { }
	in_buffer(char const* data, size_t size, size_t read_position) : data(data), size(size), read_position(read_position) {
	}

	operator bool() const noexcept {
		return data && read_position < size;
	}

	char const* view_data() const {
		return data;
	}
	size_t view_size() const {
		return size;
	}
	size_t view_read_position() const {
		return read_position;
	}

	template<typename T>
	T read() {
		T temp = T{ };
		if(read_position + sizeof(T) <= size) {
			std::memcpy(&temp, data + read_position, sizeof(T));
			read_position += sizeof(T);
		}
		return temp;
	}
	template<typename T>
	void read(T& out) {
		out = T{ };
		if(read_position + sizeof(T) <= size) {
			std::memcpy(&out, data + read_position, sizeof(T));
			read_position += sizeof(T);
		}
	}
	template<typename T>
	std::span<T const> read_fixed(size_t count) {
		auto len = std::min(count, (size - read_position) / sizeof(T));
		auto start = (T const*)(data + read_position);
		read_position += count * sizeof(T);
		return std::span<T const>(start, start + len);
	}
	template<typename T>
	std::span<T const> read_variable() {
		auto count = read<uint32_t>();
		return read_fixed<T>(size_t(count));
	}
	in_buffer read_relocation() {
		uint32_t offset = read<uint32_t>();
		return in_buffer(data, size, offset);
	}
	in_buffer read_section() {
		uint32_t section_size = read<uint32_t>();
		auto start_postion = read_position;
		read_position += (section_size - 4);
		return in_buffer(data, std::min(size_t(start_postion + section_size - 4), size), start_postion);
	}
	void read(std::string& out) {
		auto s = read_variable<char>();
		out = std::string(s.data(), s.size());
	}
	void read(std::wstring& out) {
		auto s = read_variable<wchar_t>();
		out = std::wstring(s.data(), s.size());
	}
};

template<>
inline std::string_view in_buffer::read<std::string_view>() {
	auto s = read_variable<char>();
	return std::string_view(s.data(), s.size());
}
template<>
inline std::wstring_view in_buffer::read<std::wstring_view>() {
	auto s = read_variable<wchar_t>();
	return std::wstring_view(s.data(), s.size());
}

}

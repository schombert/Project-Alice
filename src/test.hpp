#pragma once

//
// This file was automatically generated from: E:/VSProjects/Project Alice/src/test.txt
// EDIT AT YOUR OWN RISK; all changes will be lost upon regeneration
// NOT SUITABLE FOR USE IN CRITICAL SOFTWARE WHERE LIVES OR LIVELIHOODS DEPEND ON THE CORRECT OPERATION
//

#include <cstdint>
#include <cstddef>
#include <utility>
#include <vector>
#include <algorithm>
#include <array>
#include <memory>
#include <assert.h>
#include <cstring>
#include "common_types.hpp"
#ifndef DCON_NO_VE
#include "ve.hpp"
#endif

#ifdef NDEBUG
#ifdef _MSC_VER
#define DCON_RELEASE_INLINE __forceinline
#else
#define DCON_RELEASE_INLINE inline __attribute__((always_inline))
#endif
#else
#define DCON_RELEASE_INLINE inline
#endif
#pragma warning( push )
#pragma warning( disable : 4324 )

namespace dcon {
	struct load_record {
		bool person : 1;
		bool person_age : 1;
		load_record() {
			person = false;
			person_age = false;
		}
	};
	//
	// definition of strongly typed index for person_id
	//
	class person_id {
		public:
		using value_base_t = uint8_t;
		using zero_is_null_t = std::true_type;
		
		uint8_t value = 0;
		
		constexpr person_id() noexcept = default;
		explicit constexpr person_id(uint8_t v) noexcept : value(v + 1) {}
		constexpr person_id(person_id const& v) noexcept = default;
		constexpr person_id(person_id&& v) noexcept = default;
		
		person_id& operator=(person_id const& v) noexcept = default;
		person_id& operator=(person_id&& v) noexcept = default;
		constexpr bool operator==(person_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(person_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint8_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class person_id_pair {
		public:
		person_id left;
		person_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(person_id id) { return bool(id); }
	
}

#ifndef DCON_NO_VE
namespace ve {
	template<>
	struct value_to_vector_type_s<dcon::person_id> {
		using type = tagged_vector<dcon::person_id>;
	};
	
}

#endif
namespace dcon {
	namespace detail {
	}

	class data_container;

	namespace internal {
		class const_object_iterator_person;
		class object_iterator_person;

		class alignas(64) person_class {
			friend const_object_iterator_person;
			friend object_iterator_person;
			private:
			//
			// storage space for age of type int32_t
			//
			struct alignas(64) dtype_age {
				uint8_t padding[(63 + sizeof(int32_t)) & ~uint64_t(63)];
				int32_t values[(sizeof(int32_t) <= 64 ? (uint32_t(100) + (uint32_t(64) / uint32_t(sizeof(int32_t))) - uint32_t(1)) & ~(uint32_t(64) / uint32_t(sizeof(int32_t)) - uint32_t(1)) : uint32_t(100))];
				DCON_RELEASE_INLINE auto vptr() const { return values; }
				DCON_RELEASE_INLINE auto vptr() { return values; }
				dtype_age() { std::uninitialized_value_construct_n(values - 1, 1 + (sizeof(int32_t) <= 64 ? (uint32_t(100) + (uint32_t(64) / uint32_t(sizeof(int32_t))) - uint32_t(1)) & ~(uint32_t(64) / uint32_t(sizeof(int32_t)) - uint32_t(1)) : uint32_t(100))); }
			}
			m_age;
			
			uint32_t size_used = 0;


			public:
			friend data_container;
		};

	}

	class person_const_fat_id;
	class person_fat_id;
	class person_fat_id {
		friend data_container;
		public:
		data_container& container;
		person_id id;
		person_fat_id(data_container& c, person_id i) noexcept : container(c), id(i) {}
		person_fat_id(person_fat_id const& o) noexcept : container(o.container), id(o.id) {}
		DCON_RELEASE_INLINE operator person_id() const noexcept { return id; }
		DCON_RELEASE_INLINE person_fat_id& operator=(person_fat_id const& other) noexcept {
			assert(&container == &other.container);
			id = other.id;
			return *this;
		}
		DCON_RELEASE_INLINE person_fat_id& operator=(person_id other) noexcept {
			id = other;
			return *this;
		}
		DCON_RELEASE_INLINE bool operator==(person_fat_id const& other) const noexcept {
			assert(&container == &other.container);
			return id == other.id;
		}
		DCON_RELEASE_INLINE bool operator==(person_id other) const noexcept {
			return id == other;
		}
		DCON_RELEASE_INLINE bool operator!=(person_fat_id const& other) const noexcept {
			assert(&container == &other.container);
			return id != other.id;
		}
		DCON_RELEASE_INLINE bool operator!=(person_id other) const noexcept {
			return id != other;
		}
		explicit operator bool() const noexcept { return bool(id); }
		DCON_RELEASE_INLINE int32_t& get_age() const noexcept;
		DCON_RELEASE_INLINE void set_age(int32_t v) const noexcept;
		DCON_RELEASE_INLINE bool is_valid() const noexcept;
		
	};
	DCON_RELEASE_INLINE person_fat_id fatten(data_container& c, person_id id) noexcept {
		return person_fat_id(c, id);
	}
	
	class person_const_fat_id {
		friend data_container;
		public:
		data_container const& container;
		person_id id;
		person_const_fat_id(data_container const& c, person_id i) noexcept : container(c), id(i) {}
		person_const_fat_id(person_const_fat_id const& o) noexcept : container(o.container), id(o.id) {}
		person_const_fat_id(person_fat_id const& o) noexcept : container(o.container), id(o.id) {}
		DCON_RELEASE_INLINE operator person_id() const noexcept { return id; }
		DCON_RELEASE_INLINE person_const_fat_id& operator=(person_const_fat_id const& other) noexcept {
			assert(&container == &other.container);
			id = other.id;
			return *this;
		}
		DCON_RELEASE_INLINE person_const_fat_id& operator=(person_fat_id const& other) noexcept {
			assert(&container == &other.container);
			id = other.id;
			return *this;
		}
		DCON_RELEASE_INLINE person_const_fat_id& operator=(person_id other) noexcept {
			id = other;
			return *this;
		}
		DCON_RELEASE_INLINE bool operator==(person_const_fat_id const& other) const noexcept {
			assert(&container == &other.container);
			return id == other.id;
		}
		DCON_RELEASE_INLINE bool operator==(person_fat_id const& other) const noexcept {
			assert(&container == &other.container);
			return id == other.id;
		}
		DCON_RELEASE_INLINE bool operator==(person_id other) const noexcept {
			return id == other;
		}
		DCON_RELEASE_INLINE bool operator!=(person_const_fat_id const& other) const noexcept {
			assert(&container == &other.container);
			return id != other.id;
		}
		DCON_RELEASE_INLINE bool operator!=(person_fat_id const& other) const noexcept {
			assert(&container == &other.container);
			return id != other.id;
		}
		DCON_RELEASE_INLINE bool operator!=(person_id other) const noexcept {
			return id != other;
		}
		DCON_RELEASE_INLINE explicit operator bool() const noexcept { return bool(id); }
		DCON_RELEASE_INLINE int32_t get_age() const noexcept;
		DCON_RELEASE_INLINE bool is_valid() const noexcept;
		
	};
	DCON_RELEASE_INLINE bool operator==(person_fat_id const& l, person_const_fat_id const& other) noexcept {
		assert(&l.container == &other.container);
		return l.id == other.id;
	}
	DCON_RELEASE_INLINE bool operator!=(person_fat_id const& l, person_const_fat_id const& other) noexcept {
		assert(&l.container == &other.container);
		return l.id != other.id;
	}
	DCON_RELEASE_INLINE person_const_fat_id fatten(data_container const& c, person_id id) noexcept {
		return person_const_fat_id(c, id);
	}
	
	namespace internal {
		class object_iterator_person {
			private:
			data_container& container;
			uint32_t index = 0;
			public:
			object_iterator_person(data_container& c, uint32_t i) noexcept;
			DCON_RELEASE_INLINE object_iterator_person& operator++() noexcept;
			DCON_RELEASE_INLINE object_iterator_person& operator--() noexcept;
			DCON_RELEASE_INLINE bool operator==(object_iterator_person const& o) const noexcept {
				return &container == &o.container && index == o.index;
			}
			DCON_RELEASE_INLINE bool operator!=(object_iterator_person const& o) const noexcept {
				return !(*this == o);
			}
			DCON_RELEASE_INLINE person_fat_id operator*() const noexcept {
				return person_fat_id(container, person_id(person_id::value_base_t(index)));
			}
			DCON_RELEASE_INLINE object_iterator_person& operator+=(int32_t n) noexcept {
				index = uint32_t(int32_t(index) + n);
				return *this;
			}
			DCON_RELEASE_INLINE object_iterator_person& operator-=(int32_t n) noexcept {
				index = uint32_t(int32_t(index) - n);
				return *this;
			}
			DCON_RELEASE_INLINE object_iterator_person operator+(int32_t n) const noexcept {
				return object_iterator_person(container, uint32_t(int32_t(index) + n));
			}
			DCON_RELEASE_INLINE object_iterator_person operator-(int32_t n) const noexcept {
				return object_iterator_person(container, uint32_t(int32_t(index) - n));
			}
			DCON_RELEASE_INLINE int32_t operator-(object_iterator_person const& o) const noexcept {
				return int32_t(index) - int32_t(o.index);
			}
			DCON_RELEASE_INLINE bool operator>(object_iterator_person const& o) const noexcept {
				return index > o.index;
			}
			DCON_RELEASE_INLINE bool operator>=(object_iterator_person const& o) const noexcept {
				return index >= o.index;
			}
			DCON_RELEASE_INLINE bool operator<(object_iterator_person const& o) const noexcept {
				return index < o.index;
			}
			DCON_RELEASE_INLINE bool operator<=(object_iterator_person const& o) const noexcept {
				return index <= o.index;
			}
			DCON_RELEASE_INLINE person_fat_id operator[](int32_t n) const noexcept {
				return person_fat_id(container, person_id(person_id::value_base_t(int32_t(index) + n)));
			}
		};
		class const_object_iterator_person {
			private:
			data_container const& container;
			uint32_t index = 0;
			public:
			const_object_iterator_person(data_container const& c, uint32_t i) noexcept;
			DCON_RELEASE_INLINE const_object_iterator_person& operator++() noexcept;
			DCON_RELEASE_INLINE const_object_iterator_person& operator--() noexcept;
			DCON_RELEASE_INLINE bool operator==(const_object_iterator_person const& o) const noexcept {
				return &container == &o.container && index == o.index;
			}
			DCON_RELEASE_INLINE bool operator!=(const_object_iterator_person const& o) const noexcept {
				return !(*this == o);
			}
			DCON_RELEASE_INLINE person_const_fat_id operator*() const noexcept {
				return person_const_fat_id(container, person_id(person_id::value_base_t(index)));
			}
			DCON_RELEASE_INLINE const_object_iterator_person& operator+=(int32_t n) noexcept {
				index = uint32_t(int32_t(index) + n);
				return *this;
			}
			DCON_RELEASE_INLINE const_object_iterator_person& operator-=(int32_t n) noexcept {
				index = uint32_t(int32_t(index) - n);
				return *this;
			}
			DCON_RELEASE_INLINE const_object_iterator_person operator+(int32_t n) const noexcept {
				return const_object_iterator_person(container, uint32_t(int32_t(index) + n));
			}
			DCON_RELEASE_INLINE const_object_iterator_person operator-(int32_t n) const noexcept {
				return const_object_iterator_person(container, uint32_t(int32_t(index) - n));
			}
			DCON_RELEASE_INLINE int32_t operator-(const_object_iterator_person const& o) const noexcept {
				return int32_t(index) - int32_t(o.index);
			}
			DCON_RELEASE_INLINE bool operator>(const_object_iterator_person const& o) const noexcept {
				return index > o.index;
			}
			DCON_RELEASE_INLINE bool operator>=(const_object_iterator_person const& o) const noexcept {
				return index >= o.index;
			}
			DCON_RELEASE_INLINE bool operator<(const_object_iterator_person const& o) const noexcept {
				return index < o.index;
			}
			DCON_RELEASE_INLINE bool operator<=(const_object_iterator_person const& o) const noexcept {
				return index <= o.index;
			}
			DCON_RELEASE_INLINE person_const_fat_id operator[](int32_t n) const noexcept {
				return person_const_fat_id(container, person_id(person_id::value_base_t(int32_t(index) + n)));
			}
		};
		
	}

	class alignas(64) data_container {
		public:
		internal::person_class person;

		//
		// Functions for person:
		//
		//
		// accessors for person: age
		//
		DCON_RELEASE_INLINE int32_t const& person_get_age(person_id id) const noexcept {
			return person.m_age.vptr()[id.index()];
		}
		DCON_RELEASE_INLINE int32_t& person_get_age(person_id id) noexcept {
			return person.m_age.vptr()[id.index()];
		}
		#ifndef DCON_NO_VE
		DCON_RELEASE_INLINE ve::value_to_vector_type<int32_t> person_get_age(ve::contiguous_tags<person_id> id) const noexcept {
			return ve::load(id, person.m_age.vptr());
		}
		DCON_RELEASE_INLINE ve::value_to_vector_type<int32_t> person_get_age(ve::partial_contiguous_tags<person_id> id) const noexcept {
			return ve::load(id, person.m_age.vptr());
		}
		DCON_RELEASE_INLINE ve::value_to_vector_type<int32_t> person_get_age(ve::tagged_vector<person_id> id) const noexcept {
			return ve::load(id, person.m_age.vptr());
		}
		#endif
		DCON_RELEASE_INLINE void person_set_age(person_id id, int32_t value) noexcept {
			person.m_age.vptr()[id.index()] = value;
		}
		#ifndef DCON_NO_VE
		DCON_RELEASE_INLINE void person_set_age(ve::contiguous_tags<person_id> id, ve::value_to_vector_type<int32_t> values) noexcept {
			ve::store(id, person.m_age.vptr(), values);
		}
		DCON_RELEASE_INLINE void person_set_age(ve::partial_contiguous_tags<person_id> id, ve::value_to_vector_type<int32_t> values) noexcept {
			ve::store(id, person.m_age.vptr(), values);
		}
		DCON_RELEASE_INLINE void person_set_age(ve::tagged_vector<person_id> id, ve::value_to_vector_type<int32_t> values) noexcept {
			ve::store(id, person.m_age.vptr(), values);
		}
		#endif
		DCON_RELEASE_INLINE bool person_is_valid(person_id id) const noexcept {
			return bool(id) && uint32_t(id.index()) < person.size_used;
		}
		
		uint32_t person_size() const noexcept { return person.size_used; }


		//
		// container pop_back for person
		//
		void pop_back_person() {
			if(person.size_used == 0) return;
			person_id id_removed(person_id::value_base_t(person.size_used - 1));
			person.m_age.vptr()[id_removed.index()] = int32_t{};
			--person.size_used;
		}
		
		//
		// container resize for person
		//
		void person_resize(uint32_t new_size) {
			#ifndef DCON_USE_EXCEPTIONS
			if(new_size > 100) std::abort();
			#else
			if(new_size > 100) throw dcon::out_of_space{};
			#endif
			const uint32_t old_size = person.size_used;
			if(new_size < old_size) {
				std::fill_n(person.m_age.vptr() + new_size, old_size - new_size, int32_t{});
			} else if(new_size > old_size) {
			}
			person.size_used = new_size;
		}
		
		//
		// container create for person
		//
		person_id create_person() {
			person_id new_id(person_id::value_base_t(person.size_used));
			#ifndef DCON_USE_EXCEPTIONS
			if(person.size_used >= 100) std::abort();
			#else
			if(person.size_used >= 100) throw dcon::out_of_space{};
			#endif
			++person.size_used;
			return new_id;
		}
		
		template <typename T>
		DCON_RELEASE_INLINE void for_each_person(T&& func) {
			for(uint32_t i = 0; i < person.size_used; ++i) {
				person_id tmp = person_id(person_id::value_base_t(i));
				func(tmp);
			}
		}
		friend internal::const_object_iterator_person;
		friend internal::object_iterator_person;
		struct {
			internal::object_iterator_person begin() {
				data_container* container = reinterpret_cast<data_container*>(reinterpret_cast<std::byte*>(this) - offsetof(data_container, in_person));
				return internal::object_iterator_person(*container, uint32_t(0));
			}
			internal::object_iterator_person end() {
				data_container* container = reinterpret_cast<data_container*>(reinterpret_cast<std::byte*>(this) - offsetof(data_container, in_person));
				return internal::object_iterator_person(*container, container->person_size());
			}
			internal::const_object_iterator_person begin() const {
				data_container const* container = reinterpret_cast<data_container const*>(reinterpret_cast<std::byte const*>(this) - offsetof(data_container, in_person));
				return internal::const_object_iterator_person(*container, uint32_t(0));
			}
			internal::const_object_iterator_person end() const {
				data_container const* container = reinterpret_cast<data_container const*>(reinterpret_cast<std::byte const*>(this) - offsetof(data_container, in_person));
				return internal::const_object_iterator_person(*container, container->person_size());
			}
		}  in_person ;
		



		void reset() {
			person_resize(0);
		}

		#ifndef DCON_NO_VE
		ve::vectorizable_buffer<float, person_id> person_make_vectorizable_float_buffer() const noexcept {
			return ve::vectorizable_buffer<float, person_id>(person.size_used);
		}
		ve::vectorizable_buffer<int32_t, person_id> person_make_vectorizable_int_buffer() const noexcept {
			return ve::vectorizable_buffer<int32_t, person_id>(person.size_used);
		}
		template<typename F>
		DCON_RELEASE_INLINE void execute_serial_over_person(F&& functor) {
			ve::execute_serial<person_id>(person.size_used, functor);
		}
#ifndef VE_NO_TBB
		template<typename F>
		DCON_RELEASE_INLINE void execute_parallel_over_person(F&& functor) {
			ve::execute_parallel_exact<person_id>(person.size_used, functor);
		}
#endif
		#endif

		load_record serialize_entire_container_record() const noexcept {
			load_record result;
			result.person = true;
			result.person_age = true;
			return result;
		}
		
		//
		// calculate size (in bytes) required to serialize the desired objects, relationships, and properties
		//
		uint64_t serialize_size(load_record const& serialize_selection) const {
			uint64_t total_size = 0;
			if(serialize_selection.person) {
				dcon::record_header header(0, "uint32_t", "person", "$size");
				total_size += header.serialize_size();
				total_size += sizeof(uint32_t);
			}
			if(serialize_selection.person_age) {
				dcon::record_header iheader(0, "int32_t", "person", "age");
				total_size += iheader.serialize_size();
				total_size += sizeof(int32_t) * person.size_used;
			}
			return total_size;
		}
		
		//
		// serialize the desired objects, relationships, and properties
		//
		void serialize(std::byte*& output_buffer, load_record const& serialize_selection) const {
			if(serialize_selection.person) {
				dcon::record_header header(sizeof(uint32_t), "uint32_t", "person", "$size");
				header.serialize(output_buffer);
				*(reinterpret_cast<uint32_t*>(output_buffer)) = person.size_used;
				output_buffer += sizeof(uint32_t);
			}
			if(serialize_selection.person_age) {
				dcon::record_header header(sizeof(int32_t) * person.size_used, "int32_t", "person", "age");
				header.serialize(output_buffer);
				std::memcpy(reinterpret_cast<int32_t*>(output_buffer), person.m_age.vptr(), sizeof(int32_t) * person.size_used);
				output_buffer += sizeof(int32_t) * person.size_used;
			}
		}
		
		//
		// deserialize the desired objects, relationships, and properties
		//
		void deserialize(std::byte const*& input_buffer, std::byte const* end, load_record& serialize_selection) {
			while(input_buffer < end) {
				dcon::record_header header;
				header.deserialize(input_buffer, end);
				if(input_buffer + header.record_size <= end) {
					if(header.is_object("person")) {
						if(header.is_property("$size") && header.record_size == sizeof(uint32_t)) {
							person_resize(*(reinterpret_cast<uint32_t const*>(input_buffer)));
							serialize_selection.person = true;
						}
						else if(header.is_property("age")) {
							if(header.is_type("int32_t")) {
								std::memcpy(person.m_age.vptr(), reinterpret_cast<int32_t const*>(input_buffer), std::min(size_t(person.size_used) * sizeof(int32_t), header.record_size));
								serialize_selection.person_age = true;
							}
							else if(header.is_type("int8_t")) {
								for(uint32_t i = 0; i < std::min(person.size_used, uint32_t(header.record_size / sizeof(int8_t))); ++i) {
									person.m_age.vptr()[i] = int32_t(*(reinterpret_cast<int8_t const*>(input_buffer) + i));
								}
								serialize_selection.person_age = true;
							}
							else if(header.is_type("uint8_t")) {
								for(uint32_t i = 0; i < std::min(person.size_used, uint32_t(header.record_size / sizeof(uint8_t))); ++i) {
									person.m_age.vptr()[i] = int32_t(*(reinterpret_cast<uint8_t const*>(input_buffer) + i));
								}
								serialize_selection.person_age = true;
							}
							else if(header.is_type("int16_t")) {
								for(uint32_t i = 0; i < std::min(person.size_used, uint32_t(header.record_size / sizeof(int16_t))); ++i) {
									person.m_age.vptr()[i] = int32_t(*(reinterpret_cast<int16_t const*>(input_buffer) + i));
								}
								serialize_selection.person_age = true;
							}
							else if(header.is_type("uint16_t")) {
								for(uint32_t i = 0; i < std::min(person.size_used, uint32_t(header.record_size / sizeof(uint16_t))); ++i) {
									person.m_age.vptr()[i] = int32_t(*(reinterpret_cast<uint16_t const*>(input_buffer) + i));
								}
								serialize_selection.person_age = true;
							}
							else if(header.is_type("uint32_t")) {
								for(uint32_t i = 0; i < std::min(person.size_used, uint32_t(header.record_size / sizeof(uint32_t))); ++i) {
									person.m_age.vptr()[i] = int32_t(*(reinterpret_cast<uint32_t const*>(input_buffer) + i));
								}
								serialize_selection.person_age = true;
							}
							else if(header.is_type("int64_t")) {
								for(uint32_t i = 0; i < std::min(person.size_used, uint32_t(header.record_size / sizeof(int64_t))); ++i) {
									person.m_age.vptr()[i] = int32_t(*(reinterpret_cast<int64_t const*>(input_buffer) + i));
								}
								serialize_selection.person_age = true;
							}
							else if(header.is_type("uint64_t")) {
								for(uint32_t i = 0; i < std::min(person.size_used, uint32_t(header.record_size / sizeof(uint64_t))); ++i) {
									person.m_age.vptr()[i] = int32_t(*(reinterpret_cast<uint64_t const*>(input_buffer) + i));
								}
								serialize_selection.person_age = true;
							}
							else if(header.is_type("float")) {
								for(uint32_t i = 0; i < std::min(person.size_used, uint32_t(header.record_size / sizeof(float))); ++i) {
									person.m_age.vptr()[i] = int32_t(*(reinterpret_cast<float const*>(input_buffer) + i));
								}
								serialize_selection.person_age = true;
							}
							else if(header.is_type("double")) {
								for(uint32_t i = 0; i < std::min(person.size_used, uint32_t(header.record_size / sizeof(double))); ++i) {
									person.m_age.vptr()[i] = int32_t(*(reinterpret_cast<double const*>(input_buffer) + i));
								}
								serialize_selection.person_age = true;
							}
						}
					}
				}
				input_buffer += header.record_size;
			}
		}
		
		//
		// deserialize the desired objects, relationships, and properties where the mask is set
		//
		void deserialize(std::byte const*& input_buffer, std::byte const* end, load_record& serialize_selection, load_record const& mask) {
			while(input_buffer < end) {
				dcon::record_header header;
				header.deserialize(input_buffer, end);
				if(input_buffer + header.record_size <= end) {
					if(header.is_object("person") && mask.person) {
						if(header.is_property("$size") && header.record_size == sizeof(uint32_t)) {
							person_resize(*(reinterpret_cast<uint32_t const*>(input_buffer)));
							serialize_selection.person = true;
						}
						else if(header.is_property("age") && mask.person_age) {
							if(header.is_type("int32_t")) {
								std::memcpy(person.m_age.vptr(), reinterpret_cast<int32_t const*>(input_buffer), std::min(size_t(person.size_used) * sizeof(int32_t), header.record_size));
								serialize_selection.person_age = true;
							}
							else if(header.is_type("int8_t")) {
								for(uint32_t i = 0; i < std::min(person.size_used, uint32_t(header.record_size / sizeof(int8_t))); ++i) {
									person.m_age.vptr()[i] = int32_t(*(reinterpret_cast<int8_t const*>(input_buffer) + i));
								}
								serialize_selection.person_age = true;
							}
							else if(header.is_type("uint8_t")) {
								for(uint32_t i = 0; i < std::min(person.size_used, uint32_t(header.record_size / sizeof(uint8_t))); ++i) {
									person.m_age.vptr()[i] = int32_t(*(reinterpret_cast<uint8_t const*>(input_buffer) + i));
								}
								serialize_selection.person_age = true;
							}
							else if(header.is_type("int16_t")) {
								for(uint32_t i = 0; i < std::min(person.size_used, uint32_t(header.record_size / sizeof(int16_t))); ++i) {
									person.m_age.vptr()[i] = int32_t(*(reinterpret_cast<int16_t const*>(input_buffer) + i));
								}
								serialize_selection.person_age = true;
							}
							else if(header.is_type("uint16_t")) {
								for(uint32_t i = 0; i < std::min(person.size_used, uint32_t(header.record_size / sizeof(uint16_t))); ++i) {
									person.m_age.vptr()[i] = int32_t(*(reinterpret_cast<uint16_t const*>(input_buffer) + i));
								}
								serialize_selection.person_age = true;
							}
							else if(header.is_type("uint32_t")) {
								for(uint32_t i = 0; i < std::min(person.size_used, uint32_t(header.record_size / sizeof(uint32_t))); ++i) {
									person.m_age.vptr()[i] = int32_t(*(reinterpret_cast<uint32_t const*>(input_buffer) + i));
								}
								serialize_selection.person_age = true;
							}
							else if(header.is_type("int64_t")) {
								for(uint32_t i = 0; i < std::min(person.size_used, uint32_t(header.record_size / sizeof(int64_t))); ++i) {
									person.m_age.vptr()[i] = int32_t(*(reinterpret_cast<int64_t const*>(input_buffer) + i));
								}
								serialize_selection.person_age = true;
							}
							else if(header.is_type("uint64_t")) {
								for(uint32_t i = 0; i < std::min(person.size_used, uint32_t(header.record_size / sizeof(uint64_t))); ++i) {
									person.m_age.vptr()[i] = int32_t(*(reinterpret_cast<uint64_t const*>(input_buffer) + i));
								}
								serialize_selection.person_age = true;
							}
							else if(header.is_type("float")) {
								for(uint32_t i = 0; i < std::min(person.size_used, uint32_t(header.record_size / sizeof(float))); ++i) {
									person.m_age.vptr()[i] = int32_t(*(reinterpret_cast<float const*>(input_buffer) + i));
								}
								serialize_selection.person_age = true;
							}
							else if(header.is_type("double")) {
								for(uint32_t i = 0; i < std::min(person.size_used, uint32_t(header.record_size / sizeof(double))); ++i) {
									person.m_age.vptr()[i] = int32_t(*(reinterpret_cast<double const*>(input_buffer) + i));
								}
								serialize_selection.person_age = true;
							}
						}
					}
				}
				input_buffer += header.record_size;
			}
		}
		

	};

	DCON_RELEASE_INLINE int32_t& person_fat_id::get_age() const noexcept {
		return container.person_get_age(id);
	}
	DCON_RELEASE_INLINE void person_fat_id::set_age(int32_t v) const noexcept {
		container.person_set_age(id, v);
	}
	DCON_RELEASE_INLINE bool person_fat_id::is_valid() const noexcept {
		return container.person_is_valid(id);
	}
	
	DCON_RELEASE_INLINE int32_t person_const_fat_id::get_age() const noexcept {
		return container.person_get_age(id);
	}
	DCON_RELEASE_INLINE bool person_const_fat_id::is_valid() const noexcept {
		return container.person_is_valid(id);
	}
	

	namespace internal {
		DCON_RELEASE_INLINE object_iterator_person::object_iterator_person(data_container& c, uint32_t i) noexcept : container(c), index(i) {
		}
		DCON_RELEASE_INLINE const_object_iterator_person::const_object_iterator_person(data_container const& c, uint32_t i) noexcept : container(c), index(i) {
		}
		DCON_RELEASE_INLINE object_iterator_person& object_iterator_person::operator++() noexcept {
			++index;
			return *this;
		}
		DCON_RELEASE_INLINE const_object_iterator_person& const_object_iterator_person::operator++() noexcept {
			++index;
			return *this;
		}
		DCON_RELEASE_INLINE object_iterator_person& object_iterator_person::operator--() noexcept {
			--index;
			return *this;
		}
		DCON_RELEASE_INLINE const_object_iterator_person& const_object_iterator_person::operator--() noexcept {
			--index;
			return *this;
		}
		
	};


}

#undef DCON_RELEASE_INLINE
#pragma warning( pop )


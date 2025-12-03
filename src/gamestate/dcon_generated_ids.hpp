#pragma once

//
// This file was automatically generated from: C:/Users/Mi/Documents/Projects2024/Project-Alice/src/gamestate/dcon_generated.txt
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

#ifdef DCON_RELEASE_INLINE
#define DCON_RELEASE_INLINE_DEFINED
#endif
#ifndef DCON_RELEASE_INLINE_DEFINED
#ifdef NDEBUG
#ifdef _MSC_VER
#define DCON_RELEASE_INLINE __forceinline
#else
#define DCON_RELEASE_INLINE inline __attribute__((always_inline))
#endif
#else
#define DCON_RELEASE_INLINE inline
#endif
#ifdef _MSC_VER
#pragma warning( push )
#pragma warning( disable : 4324 )
#endif
#endif
namespace dcon {
	//
	// definition of strongly typed index for government_flag_id
	//
	class government_flag_id {
		public:
		using value_base_t = uint8_t;
		using zero_is_null_t = std::true_type;
		
		uint8_t value = 0;
		
		constexpr government_flag_id() noexcept = default;
		explicit constexpr government_flag_id(uint8_t v) noexcept : value(v + 1) {}
		constexpr government_flag_id(government_flag_id const& v) noexcept = default;
		constexpr government_flag_id(government_flag_id&& v) noexcept = default;
		
		government_flag_id& operator=(government_flag_id const& v) noexcept = default;
		government_flag_id& operator=(government_flag_id&& v) noexcept = default;
		constexpr bool operator==(government_flag_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(government_flag_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint8_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class government_flag_id_pair {
		public:
		government_flag_id left;
		government_flag_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(government_flag_id id) { return bool(id); }
	
	//
	// definition of strongly typed index for national_identity_id
	//
	class national_identity_id {
		public:
		using value_base_t = uint16_t;
		using zero_is_null_t = std::true_type;
		
		uint16_t value = 0;
		
		constexpr national_identity_id() noexcept = default;
		explicit constexpr national_identity_id(uint16_t v) noexcept : value(v + 1) {}
		constexpr national_identity_id(national_identity_id const& v) noexcept = default;
		constexpr national_identity_id(national_identity_id&& v) noexcept = default;
		
		national_identity_id& operator=(national_identity_id const& v) noexcept = default;
		national_identity_id& operator=(national_identity_id&& v) noexcept = default;
		constexpr bool operator==(national_identity_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(national_identity_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint16_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class national_identity_id_pair {
		public:
		national_identity_id left;
		national_identity_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(national_identity_id id) { return bool(id); }
	
	//
	// definition of strongly typed index for political_party_id
	//
	class political_party_id {
		public:
		using value_base_t = uint16_t;
		using zero_is_null_t = std::true_type;
		
		uint16_t value = 0;
		
		constexpr political_party_id() noexcept = default;
		explicit constexpr political_party_id(uint16_t v) noexcept : value(v + 1) {}
		constexpr political_party_id(political_party_id const& v) noexcept = default;
		constexpr political_party_id(political_party_id&& v) noexcept = default;
		
		political_party_id& operator=(political_party_id const& v) noexcept = default;
		political_party_id& operator=(political_party_id&& v) noexcept = default;
		constexpr bool operator==(political_party_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(political_party_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint16_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class political_party_id_pair {
		public:
		political_party_id left;
		political_party_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(political_party_id id) { return bool(id); }
	
	//
	// definition of strongly typed index for leader_images_id
	//
	class leader_images_id {
		public:
		using value_base_t = uint8_t;
		using zero_is_null_t = std::true_type;
		
		uint8_t value = 0;
		
		constexpr leader_images_id() noexcept = default;
		explicit constexpr leader_images_id(uint8_t v) noexcept : value(v + 1) {}
		constexpr leader_images_id(leader_images_id const& v) noexcept = default;
		constexpr leader_images_id(leader_images_id&& v) noexcept = default;
		
		leader_images_id& operator=(leader_images_id const& v) noexcept = default;
		leader_images_id& operator=(leader_images_id&& v) noexcept = default;
		constexpr bool operator==(leader_images_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(leader_images_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint8_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class leader_images_id_pair {
		public:
		leader_images_id left;
		leader_images_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(leader_images_id id) { return bool(id); }
	
	//
	// definition of strongly typed index for religion_id
	//
	class religion_id {
		public:
		using value_base_t = uint8_t;
		using zero_is_null_t = std::true_type;
		
		uint8_t value = 0;
		
		constexpr religion_id() noexcept = default;
		explicit constexpr religion_id(uint8_t v) noexcept : value(v + 1) {}
		constexpr religion_id(religion_id const& v) noexcept = default;
		constexpr religion_id(religion_id&& v) noexcept = default;
		
		religion_id& operator=(religion_id const& v) noexcept = default;
		religion_id& operator=(religion_id&& v) noexcept = default;
		constexpr bool operator==(religion_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(religion_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint8_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class religion_id_pair {
		public:
		religion_id left;
		religion_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(religion_id id) { return bool(id); }
	
	//
	// definition of strongly typed index for culture_group_id
	//
	class culture_group_id {
		public:
		using value_base_t = uint16_t;
		using zero_is_null_t = std::true_type;
		
		uint16_t value = 0;
		
		constexpr culture_group_id() noexcept = default;
		explicit constexpr culture_group_id(uint16_t v) noexcept : value(v + 1) {}
		constexpr culture_group_id(culture_group_id const& v) noexcept = default;
		constexpr culture_group_id(culture_group_id&& v) noexcept = default;
		
		culture_group_id& operator=(culture_group_id const& v) noexcept = default;
		culture_group_id& operator=(culture_group_id&& v) noexcept = default;
		constexpr bool operator==(culture_group_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(culture_group_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint16_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class culture_group_id_pair {
		public:
		culture_group_id left;
		culture_group_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(culture_group_id id) { return bool(id); }
	
	//
	// definition of strongly typed index for culture_id
	//
	class culture_id {
		public:
		using value_base_t = uint16_t;
		using zero_is_null_t = std::true_type;
		
		uint16_t value = 0;
		
		constexpr culture_id() noexcept = default;
		explicit constexpr culture_id(uint16_t v) noexcept : value(v + 1) {}
		constexpr culture_id(culture_id const& v) noexcept = default;
		constexpr culture_id(culture_id&& v) noexcept = default;
		
		culture_id& operator=(culture_id const& v) noexcept = default;
		culture_id& operator=(culture_id&& v) noexcept = default;
		constexpr bool operator==(culture_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(culture_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint16_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class culture_id_pair {
		public:
		culture_id left;
		culture_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(culture_id id) { return bool(id); }
	
	//
	// definition of strongly typed index for culture_group_membership_id
	//
	class culture_group_membership_id {
		public:
		using value_base_t = uint16_t;
		using zero_is_null_t = std::true_type;
		
		uint16_t value = 0;
		
		constexpr culture_group_membership_id() noexcept = default;
		explicit constexpr culture_group_membership_id(uint16_t v) noexcept : value(v + 1) {}
		constexpr culture_group_membership_id(culture_group_membership_id const& v) noexcept = default;
		constexpr culture_group_membership_id(culture_group_membership_id&& v) noexcept = default;
		
		culture_group_membership_id& operator=(culture_group_membership_id const& v) noexcept = default;
		culture_group_membership_id& operator=(culture_group_membership_id&& v) noexcept = default;
		constexpr bool operator==(culture_group_membership_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(culture_group_membership_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint16_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class culture_group_membership_id_pair {
		public:
		culture_group_membership_id left;
		culture_group_membership_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(culture_group_membership_id id) { return bool(id); }
	
	//
	// definition of strongly typed index for cultural_union_of_id
	//
	class cultural_union_of_id {
		public:
		using value_base_t = uint16_t;
		using zero_is_null_t = std::true_type;
		
		uint16_t value = 0;
		
		constexpr cultural_union_of_id() noexcept = default;
		explicit constexpr cultural_union_of_id(uint16_t v) noexcept : value(v + 1) {}
		constexpr cultural_union_of_id(cultural_union_of_id const& v) noexcept = default;
		constexpr cultural_union_of_id(cultural_union_of_id&& v) noexcept = default;
		
		cultural_union_of_id& operator=(cultural_union_of_id const& v) noexcept = default;
		cultural_union_of_id& operator=(cultural_union_of_id&& v) noexcept = default;
		constexpr bool operator==(cultural_union_of_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(cultural_union_of_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint16_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class cultural_union_of_id_pair {
		public:
		cultural_union_of_id left;
		cultural_union_of_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(cultural_union_of_id id) { return bool(id); }
	
	//
	// definition of strongly typed index for commodity_id
	//
	class commodity_id {
		public:
		using value_base_t = uint8_t;
		using zero_is_null_t = std::true_type;
		
		uint8_t value = 0;
		
		constexpr commodity_id() noexcept = default;
		explicit constexpr commodity_id(uint8_t v) noexcept : value(v + 1) {}
		constexpr commodity_id(commodity_id const& v) noexcept = default;
		constexpr commodity_id(commodity_id&& v) noexcept = default;
		
		commodity_id& operator=(commodity_id const& v) noexcept = default;
		commodity_id& operator=(commodity_id&& v) noexcept = default;
		constexpr bool operator==(commodity_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(commodity_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint8_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class commodity_id_pair {
		public:
		commodity_id left;
		commodity_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(commodity_id id) { return bool(id); }
	
	//
	// definition of strongly typed index for modifier_id
	//
	class modifier_id {
		public:
		using value_base_t = uint16_t;
		using zero_is_null_t = std::true_type;
		
		uint16_t value = 0;
		
		constexpr modifier_id() noexcept = default;
		explicit constexpr modifier_id(uint16_t v) noexcept : value(v + 1) {}
		constexpr modifier_id(modifier_id const& v) noexcept = default;
		constexpr modifier_id(modifier_id&& v) noexcept = default;
		
		modifier_id& operator=(modifier_id const& v) noexcept = default;
		modifier_id& operator=(modifier_id&& v) noexcept = default;
		constexpr bool operator==(modifier_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(modifier_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint16_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class modifier_id_pair {
		public:
		modifier_id left;
		modifier_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(modifier_id id) { return bool(id); }
	
	//
	// definition of strongly typed index for factory_type_id
	//
	class factory_type_id {
		public:
		using value_base_t = uint16_t;
		using zero_is_null_t = std::true_type;
		
		uint16_t value = 0;
		
		constexpr factory_type_id() noexcept = default;
		explicit constexpr factory_type_id(uint16_t v) noexcept : value(v + 1) {}
		constexpr factory_type_id(factory_type_id const& v) noexcept = default;
		constexpr factory_type_id(factory_type_id&& v) noexcept = default;
		
		factory_type_id& operator=(factory_type_id const& v) noexcept = default;
		factory_type_id& operator=(factory_type_id&& v) noexcept = default;
		constexpr bool operator==(factory_type_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(factory_type_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint16_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class factory_type_id_pair {
		public:
		factory_type_id left;
		factory_type_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(factory_type_id id) { return bool(id); }
	
	//
	// definition of strongly typed index for ideology_group_id
	//
	class ideology_group_id {
		public:
		using value_base_t = uint8_t;
		using zero_is_null_t = std::true_type;
		
		uint8_t value = 0;
		
		constexpr ideology_group_id() noexcept = default;
		explicit constexpr ideology_group_id(uint8_t v) noexcept : value(v + 1) {}
		constexpr ideology_group_id(ideology_group_id const& v) noexcept = default;
		constexpr ideology_group_id(ideology_group_id&& v) noexcept = default;
		
		ideology_group_id& operator=(ideology_group_id const& v) noexcept = default;
		ideology_group_id& operator=(ideology_group_id&& v) noexcept = default;
		constexpr bool operator==(ideology_group_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(ideology_group_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint8_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class ideology_group_id_pair {
		public:
		ideology_group_id left;
		ideology_group_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(ideology_group_id id) { return bool(id); }
	
	//
	// definition of strongly typed index for ideology_id
	//
	class ideology_id {
		public:
		using value_base_t = uint8_t;
		using zero_is_null_t = std::true_type;
		
		uint8_t value = 0;
		
		constexpr ideology_id() noexcept = default;
		explicit constexpr ideology_id(uint8_t v) noexcept : value(v + 1) {}
		constexpr ideology_id(ideology_id const& v) noexcept = default;
		constexpr ideology_id(ideology_id&& v) noexcept = default;
		
		ideology_id& operator=(ideology_id const& v) noexcept = default;
		ideology_id& operator=(ideology_id&& v) noexcept = default;
		constexpr bool operator==(ideology_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(ideology_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint8_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class ideology_id_pair {
		public:
		ideology_id left;
		ideology_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(ideology_id id) { return bool(id); }
	
	//
	// definition of strongly typed index for ideology_group_membership_id
	//
	class ideology_group_membership_id {
		public:
		using value_base_t = uint8_t;
		using zero_is_null_t = std::true_type;
		
		uint8_t value = 0;
		
		constexpr ideology_group_membership_id() noexcept = default;
		explicit constexpr ideology_group_membership_id(uint8_t v) noexcept : value(v + 1) {}
		constexpr ideology_group_membership_id(ideology_group_membership_id const& v) noexcept = default;
		constexpr ideology_group_membership_id(ideology_group_membership_id&& v) noexcept = default;
		
		ideology_group_membership_id& operator=(ideology_group_membership_id const& v) noexcept = default;
		ideology_group_membership_id& operator=(ideology_group_membership_id&& v) noexcept = default;
		constexpr bool operator==(ideology_group_membership_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(ideology_group_membership_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint8_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class ideology_group_membership_id_pair {
		public:
		ideology_group_membership_id left;
		ideology_group_membership_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(ideology_group_membership_id id) { return bool(id); }
	
	//
	// definition of strongly typed index for issue_id
	//
	class issue_id {
		public:
		using value_base_t = uint8_t;
		using zero_is_null_t = std::true_type;
		
		uint8_t value = 0;
		
		constexpr issue_id() noexcept = default;
		explicit constexpr issue_id(uint8_t v) noexcept : value(v + 1) {}
		constexpr issue_id(issue_id const& v) noexcept = default;
		constexpr issue_id(issue_id&& v) noexcept = default;
		
		issue_id& operator=(issue_id const& v) noexcept = default;
		issue_id& operator=(issue_id&& v) noexcept = default;
		constexpr bool operator==(issue_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(issue_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint8_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class issue_id_pair {
		public:
		issue_id left;
		issue_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(issue_id id) { return bool(id); }
	
	//
	// definition of strongly typed index for issue_option_id
	//
	class issue_option_id {
		public:
		using value_base_t = uint16_t;
		using zero_is_null_t = std::true_type;
		
		uint16_t value = 0;
		
		constexpr issue_option_id() noexcept = default;
		explicit constexpr issue_option_id(uint16_t v) noexcept : value(v + 1) {}
		constexpr issue_option_id(issue_option_id const& v) noexcept = default;
		constexpr issue_option_id(issue_option_id&& v) noexcept = default;
		
		issue_option_id& operator=(issue_option_id const& v) noexcept = default;
		issue_option_id& operator=(issue_option_id&& v) noexcept = default;
		constexpr bool operator==(issue_option_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(issue_option_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint16_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class issue_option_id_pair {
		public:
		issue_option_id left;
		issue_option_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(issue_option_id id) { return bool(id); }
	
	//
	// definition of strongly typed index for reform_id
	//
	class reform_id {
		public:
		using value_base_t = uint8_t;
		using zero_is_null_t = std::true_type;
		
		uint8_t value = 0;
		
		constexpr reform_id() noexcept = default;
		explicit constexpr reform_id(uint8_t v) noexcept : value(v + 1) {}
		constexpr reform_id(reform_id const& v) noexcept = default;
		constexpr reform_id(reform_id&& v) noexcept = default;
		
		reform_id& operator=(reform_id const& v) noexcept = default;
		reform_id& operator=(reform_id&& v) noexcept = default;
		constexpr bool operator==(reform_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(reform_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint8_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class reform_id_pair {
		public:
		reform_id left;
		reform_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(reform_id id) { return bool(id); }
	
	//
	// definition of strongly typed index for reform_option_id
	//
	class reform_option_id {
		public:
		using value_base_t = uint16_t;
		using zero_is_null_t = std::true_type;
		
		uint16_t value = 0;
		
		constexpr reform_option_id() noexcept = default;
		explicit constexpr reform_option_id(uint16_t v) noexcept : value(v + 1) {}
		constexpr reform_option_id(reform_option_id const& v) noexcept = default;
		constexpr reform_option_id(reform_option_id&& v) noexcept = default;
		
		reform_option_id& operator=(reform_option_id const& v) noexcept = default;
		reform_option_id& operator=(reform_option_id&& v) noexcept = default;
		constexpr bool operator==(reform_option_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(reform_option_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint16_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class reform_option_id_pair {
		public:
		reform_option_id left;
		reform_option_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(reform_option_id id) { return bool(id); }
	
	//
	// definition of strongly typed index for cb_type_id
	//
	class cb_type_id {
		public:
		using value_base_t = uint16_t;
		using zero_is_null_t = std::true_type;
		
		uint16_t value = 0;
		
		constexpr cb_type_id() noexcept = default;
		explicit constexpr cb_type_id(uint16_t v) noexcept : value(v + 1) {}
		constexpr cb_type_id(cb_type_id const& v) noexcept = default;
		constexpr cb_type_id(cb_type_id&& v) noexcept = default;
		
		cb_type_id& operator=(cb_type_id const& v) noexcept = default;
		cb_type_id& operator=(cb_type_id&& v) noexcept = default;
		constexpr bool operator==(cb_type_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(cb_type_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint16_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class cb_type_id_pair {
		public:
		cb_type_id left;
		cb_type_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(cb_type_id id) { return bool(id); }
	
	//
	// definition of strongly typed index for leader_trait_id
	//
	class leader_trait_id {
		public:
		using value_base_t = uint16_t;
		using zero_is_null_t = std::true_type;
		
		uint16_t value = 0;
		
		constexpr leader_trait_id() noexcept = default;
		explicit constexpr leader_trait_id(uint16_t v) noexcept : value(v + 1) {}
		constexpr leader_trait_id(leader_trait_id const& v) noexcept = default;
		constexpr leader_trait_id(leader_trait_id&& v) noexcept = default;
		
		leader_trait_id& operator=(leader_trait_id const& v) noexcept = default;
		leader_trait_id& operator=(leader_trait_id&& v) noexcept = default;
		constexpr bool operator==(leader_trait_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(leader_trait_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint16_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class leader_trait_id_pair {
		public:
		leader_trait_id left;
		leader_trait_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(leader_trait_id id) { return bool(id); }
	
	//
	// definition of strongly typed index for pop_type_id
	//
	class pop_type_id {
		public:
		using value_base_t = uint8_t;
		using zero_is_null_t = std::true_type;
		
		uint8_t value = 0;
		
		constexpr pop_type_id() noexcept = default;
		explicit constexpr pop_type_id(uint8_t v) noexcept : value(v + 1) {}
		constexpr pop_type_id(pop_type_id const& v) noexcept = default;
		constexpr pop_type_id(pop_type_id&& v) noexcept = default;
		
		pop_type_id& operator=(pop_type_id const& v) noexcept = default;
		pop_type_id& operator=(pop_type_id&& v) noexcept = default;
		constexpr bool operator==(pop_type_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(pop_type_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint8_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class pop_type_id_pair {
		public:
		pop_type_id left;
		pop_type_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(pop_type_id id) { return bool(id); }
	
	//
	// definition of strongly typed index for rebel_type_id
	//
	class rebel_type_id {
		public:
		using value_base_t = uint16_t;
		using zero_is_null_t = std::true_type;
		
		uint16_t value = 0;
		
		constexpr rebel_type_id() noexcept = default;
		explicit constexpr rebel_type_id(uint16_t v) noexcept : value(v + 1) {}
		constexpr rebel_type_id(rebel_type_id const& v) noexcept = default;
		constexpr rebel_type_id(rebel_type_id&& v) noexcept = default;
		
		rebel_type_id& operator=(rebel_type_id const& v) noexcept = default;
		rebel_type_id& operator=(rebel_type_id&& v) noexcept = default;
		constexpr bool operator==(rebel_type_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(rebel_type_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint16_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class rebel_type_id_pair {
		public:
		rebel_type_id left;
		rebel_type_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(rebel_type_id id) { return bool(id); }
	
	//
	// definition of strongly typed index for government_type_id
	//
	class government_type_id {
		public:
		using value_base_t = uint8_t;
		using zero_is_null_t = std::true_type;
		
		uint8_t value = 0;
		
		constexpr government_type_id() noexcept = default;
		explicit constexpr government_type_id(uint8_t v) noexcept : value(v + 1) {}
		constexpr government_type_id(government_type_id const& v) noexcept = default;
		constexpr government_type_id(government_type_id&& v) noexcept = default;
		
		government_type_id& operator=(government_type_id const& v) noexcept = default;
		government_type_id& operator=(government_type_id&& v) noexcept = default;
		constexpr bool operator==(government_type_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(government_type_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint8_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class government_type_id_pair {
		public:
		government_type_id left;
		government_type_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(government_type_id id) { return bool(id); }
	
	//
	// definition of strongly typed index for province_id
	//
	class province_id {
		public:
		using value_base_t = uint16_t;
		using zero_is_null_t = std::true_type;
		
		uint16_t value = 0;
		
		constexpr province_id() noexcept = default;
		explicit constexpr province_id(uint16_t v) noexcept : value(v + 1) {}
		constexpr province_id(province_id const& v) noexcept = default;
		constexpr province_id(province_id&& v) noexcept = default;
		
		province_id& operator=(province_id const& v) noexcept = default;
		province_id& operator=(province_id&& v) noexcept = default;
		constexpr bool operator==(province_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(province_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint16_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class province_id_pair {
		public:
		province_id left;
		province_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(province_id id) { return bool(id); }
	
	//
	// definition of strongly typed index for province_adjacency_id
	//
	class province_adjacency_id {
		public:
		using value_base_t = uint16_t;
		using zero_is_null_t = std::true_type;
		
		uint16_t value = 0;
		
		constexpr province_adjacency_id() noexcept = default;
		explicit constexpr province_adjacency_id(uint16_t v) noexcept : value(v + 1) {}
		constexpr province_adjacency_id(province_adjacency_id const& v) noexcept = default;
		constexpr province_adjacency_id(province_adjacency_id&& v) noexcept = default;
		
		province_adjacency_id& operator=(province_adjacency_id const& v) noexcept = default;
		province_adjacency_id& operator=(province_adjacency_id&& v) noexcept = default;
		constexpr bool operator==(province_adjacency_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(province_adjacency_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint16_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class province_adjacency_id_pair {
		public:
		province_adjacency_id left;
		province_adjacency_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(province_adjacency_id id) { return bool(id); }
	
	//
	// definition of strongly typed index for nation_adjacency_id
	//
	class nation_adjacency_id {
		public:
		using value_base_t = uint16_t;
		using zero_is_null_t = std::true_type;
		
		uint16_t value = 0;
		
		constexpr nation_adjacency_id() noexcept = default;
		explicit constexpr nation_adjacency_id(uint16_t v) noexcept : value(v + 1) {}
		constexpr nation_adjacency_id(nation_adjacency_id const& v) noexcept = default;
		constexpr nation_adjacency_id(nation_adjacency_id&& v) noexcept = default;
		
		nation_adjacency_id& operator=(nation_adjacency_id const& v) noexcept = default;
		nation_adjacency_id& operator=(nation_adjacency_id&& v) noexcept = default;
		constexpr bool operator==(nation_adjacency_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(nation_adjacency_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint16_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class nation_adjacency_id_pair {
		public:
		nation_adjacency_id left;
		nation_adjacency_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(nation_adjacency_id id) { return bool(id); }
	
	//
	// definition of strongly typed index for regiment_id
	//
	class regiment_id {
		public:
		using value_base_t = uint16_t;
		using zero_is_null_t = std::true_type;
		
		uint16_t value = 0;
		
		constexpr regiment_id() noexcept = default;
		explicit constexpr regiment_id(uint16_t v) noexcept : value(v + 1) {}
		constexpr regiment_id(regiment_id const& v) noexcept = default;
		constexpr regiment_id(regiment_id&& v) noexcept = default;
		
		regiment_id& operator=(regiment_id const& v) noexcept = default;
		regiment_id& operator=(regiment_id&& v) noexcept = default;
		constexpr bool operator==(regiment_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(regiment_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint16_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class regiment_id_pair {
		public:
		regiment_id left;
		regiment_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(regiment_id id) { return bool(id); }
	
	//
	// definition of strongly typed index for automated_army_group_id
	//
	class automated_army_group_id {
		public:
		using value_base_t = uint8_t;
		using zero_is_null_t = std::true_type;
		
		uint8_t value = 0;
		
		constexpr automated_army_group_id() noexcept = default;
		explicit constexpr automated_army_group_id(uint8_t v) noexcept : value(v + 1) {}
		constexpr automated_army_group_id(automated_army_group_id const& v) noexcept = default;
		constexpr automated_army_group_id(automated_army_group_id&& v) noexcept = default;
		
		automated_army_group_id& operator=(automated_army_group_id const& v) noexcept = default;
		automated_army_group_id& operator=(automated_army_group_id&& v) noexcept = default;
		constexpr bool operator==(automated_army_group_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(automated_army_group_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint8_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class automated_army_group_id_pair {
		public:
		automated_army_group_id left;
		automated_army_group_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(automated_army_group_id id) { return bool(id); }
	
	//
	// definition of strongly typed index for automated_army_group_membership_regiment_id
	//
	class automated_army_group_membership_regiment_id {
		public:
		using value_base_t = uint16_t;
		using zero_is_null_t = std::true_type;
		
		uint16_t value = 0;
		
		constexpr automated_army_group_membership_regiment_id() noexcept = default;
		explicit constexpr automated_army_group_membership_regiment_id(uint16_t v) noexcept : value(v + 1) {}
		constexpr automated_army_group_membership_regiment_id(automated_army_group_membership_regiment_id const& v) noexcept = default;
		constexpr automated_army_group_membership_regiment_id(automated_army_group_membership_regiment_id&& v) noexcept = default;
		
		automated_army_group_membership_regiment_id& operator=(automated_army_group_membership_regiment_id const& v) noexcept = default;
		automated_army_group_membership_regiment_id& operator=(automated_army_group_membership_regiment_id&& v) noexcept = default;
		constexpr bool operator==(automated_army_group_membership_regiment_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(automated_army_group_membership_regiment_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint16_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class automated_army_group_membership_regiment_id_pair {
		public:
		automated_army_group_membership_regiment_id left;
		automated_army_group_membership_regiment_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(automated_army_group_membership_regiment_id id) { return bool(id); }
	
	//
	// definition of strongly typed index for automated_army_group_membership_navy_id
	//
	class automated_army_group_membership_navy_id {
		public:
		using value_base_t = uint16_t;
		using zero_is_null_t = std::true_type;
		
		uint16_t value = 0;
		
		constexpr automated_army_group_membership_navy_id() noexcept = default;
		explicit constexpr automated_army_group_membership_navy_id(uint16_t v) noexcept : value(v + 1) {}
		constexpr automated_army_group_membership_navy_id(automated_army_group_membership_navy_id const& v) noexcept = default;
		constexpr automated_army_group_membership_navy_id(automated_army_group_membership_navy_id&& v) noexcept = default;
		
		automated_army_group_membership_navy_id& operator=(automated_army_group_membership_navy_id const& v) noexcept = default;
		automated_army_group_membership_navy_id& operator=(automated_army_group_membership_navy_id&& v) noexcept = default;
		constexpr bool operator==(automated_army_group_membership_navy_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(automated_army_group_membership_navy_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint16_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class automated_army_group_membership_navy_id_pair {
		public:
		automated_army_group_membership_navy_id left;
		automated_army_group_membership_navy_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(automated_army_group_membership_navy_id id) { return bool(id); }
	
	//
	// definition of strongly typed index for regiment_automation_data_id
	//
	class regiment_automation_data_id {
		public:
		using value_base_t = uint16_t;
		using zero_is_null_t = std::true_type;
		
		uint16_t value = 0;
		
		constexpr regiment_automation_data_id() noexcept = default;
		explicit constexpr regiment_automation_data_id(uint16_t v) noexcept : value(v + 1) {}
		constexpr regiment_automation_data_id(regiment_automation_data_id const& v) noexcept = default;
		constexpr regiment_automation_data_id(regiment_automation_data_id&& v) noexcept = default;
		
		regiment_automation_data_id& operator=(regiment_automation_data_id const& v) noexcept = default;
		regiment_automation_data_id& operator=(regiment_automation_data_id&& v) noexcept = default;
		constexpr bool operator==(regiment_automation_data_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(regiment_automation_data_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint16_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class regiment_automation_data_id_pair {
		public:
		regiment_automation_data_id left;
		regiment_automation_data_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(regiment_automation_data_id id) { return bool(id); }
	
	//
	// definition of strongly typed index for automation_id
	//
	class automation_id {
		public:
		using value_base_t = uint16_t;
		using zero_is_null_t = std::true_type;
		
		uint16_t value = 0;
		
		constexpr automation_id() noexcept = default;
		explicit constexpr automation_id(uint16_t v) noexcept : value(v + 1) {}
		constexpr automation_id(automation_id const& v) noexcept = default;
		constexpr automation_id(automation_id&& v) noexcept = default;
		
		automation_id& operator=(automation_id const& v) noexcept = default;
		automation_id& operator=(automation_id&& v) noexcept = default;
		constexpr bool operator==(automation_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(automation_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint16_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class automation_id_pair {
		public:
		automation_id left;
		automation_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(automation_id id) { return bool(id); }
	
	//
	// definition of strongly typed index for ship_id
	//
	class ship_id {
		public:
		using value_base_t = uint16_t;
		using zero_is_null_t = std::true_type;
		
		uint16_t value = 0;
		
		constexpr ship_id() noexcept = default;
		explicit constexpr ship_id(uint16_t v) noexcept : value(v + 1) {}
		constexpr ship_id(ship_id const& v) noexcept = default;
		constexpr ship_id(ship_id&& v) noexcept = default;
		
		ship_id& operator=(ship_id const& v) noexcept = default;
		ship_id& operator=(ship_id&& v) noexcept = default;
		constexpr bool operator==(ship_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(ship_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint16_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class ship_id_pair {
		public:
		ship_id left;
		ship_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(ship_id id) { return bool(id); }
	
	//
	// definition of strongly typed index for army_id
	//
	class army_id {
		public:
		using value_base_t = uint16_t;
		using zero_is_null_t = std::true_type;
		
		uint16_t value = 0;
		
		constexpr army_id() noexcept = default;
		explicit constexpr army_id(uint16_t v) noexcept : value(v + 1) {}
		constexpr army_id(army_id const& v) noexcept = default;
		constexpr army_id(army_id&& v) noexcept = default;
		
		army_id& operator=(army_id const& v) noexcept = default;
		army_id& operator=(army_id&& v) noexcept = default;
		constexpr bool operator==(army_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(army_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint16_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class army_id_pair {
		public:
		army_id left;
		army_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(army_id id) { return bool(id); }
	
	//
	// definition of strongly typed index for army_pursuit_id
	//
	class army_pursuit_id {
		public:
		using value_base_t = uint16_t;
		using zero_is_null_t = std::true_type;
		
		uint16_t value = 0;
		
		constexpr army_pursuit_id() noexcept = default;
		explicit constexpr army_pursuit_id(uint16_t v) noexcept : value(v + 1) {}
		constexpr army_pursuit_id(army_pursuit_id const& v) noexcept = default;
		constexpr army_pursuit_id(army_pursuit_id&& v) noexcept = default;
		
		army_pursuit_id& operator=(army_pursuit_id const& v) noexcept = default;
		army_pursuit_id& operator=(army_pursuit_id&& v) noexcept = default;
		constexpr bool operator==(army_pursuit_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(army_pursuit_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint16_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class army_pursuit_id_pair {
		public:
		army_pursuit_id left;
		army_pursuit_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(army_pursuit_id id) { return bool(id); }
	
	//
	// definition of strongly typed index for navy_id
	//
	class navy_id {
		public:
		using value_base_t = uint16_t;
		using zero_is_null_t = std::true_type;
		
		uint16_t value = 0;
		
		constexpr navy_id() noexcept = default;
		explicit constexpr navy_id(uint16_t v) noexcept : value(v + 1) {}
		constexpr navy_id(navy_id const& v) noexcept = default;
		constexpr navy_id(navy_id&& v) noexcept = default;
		
		navy_id& operator=(navy_id const& v) noexcept = default;
		navy_id& operator=(navy_id&& v) noexcept = default;
		constexpr bool operator==(navy_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(navy_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint16_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class navy_id_pair {
		public:
		navy_id left;
		navy_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(navy_id id) { return bool(id); }
	
	//
	// definition of strongly typed index for army_transport_id
	//
	class army_transport_id {
		public:
		using value_base_t = uint16_t;
		using zero_is_null_t = std::true_type;
		
		uint16_t value = 0;
		
		constexpr army_transport_id() noexcept = default;
		explicit constexpr army_transport_id(uint16_t v) noexcept : value(v + 1) {}
		constexpr army_transport_id(army_transport_id const& v) noexcept = default;
		constexpr army_transport_id(army_transport_id&& v) noexcept = default;
		
		army_transport_id& operator=(army_transport_id const& v) noexcept = default;
		army_transport_id& operator=(army_transport_id&& v) noexcept = default;
		constexpr bool operator==(army_transport_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(army_transport_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint16_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class army_transport_id_pair {
		public:
		army_transport_id left;
		army_transport_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(army_transport_id id) { return bool(id); }
	
	//
	// definition of strongly typed index for army_control_id
	//
	class army_control_id {
		public:
		using value_base_t = uint16_t;
		using zero_is_null_t = std::true_type;
		
		uint16_t value = 0;
		
		constexpr army_control_id() noexcept = default;
		explicit constexpr army_control_id(uint16_t v) noexcept : value(v + 1) {}
		constexpr army_control_id(army_control_id const& v) noexcept = default;
		constexpr army_control_id(army_control_id&& v) noexcept = default;
		
		army_control_id& operator=(army_control_id const& v) noexcept = default;
		army_control_id& operator=(army_control_id&& v) noexcept = default;
		constexpr bool operator==(army_control_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(army_control_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint16_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class army_control_id_pair {
		public:
		army_control_id left;
		army_control_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(army_control_id id) { return bool(id); }
	
	//
	// definition of strongly typed index for army_rebel_control_id
	//
	class army_rebel_control_id {
		public:
		using value_base_t = uint16_t;
		using zero_is_null_t = std::true_type;
		
		uint16_t value = 0;
		
		constexpr army_rebel_control_id() noexcept = default;
		explicit constexpr army_rebel_control_id(uint16_t v) noexcept : value(v + 1) {}
		constexpr army_rebel_control_id(army_rebel_control_id const& v) noexcept = default;
		constexpr army_rebel_control_id(army_rebel_control_id&& v) noexcept = default;
		
		army_rebel_control_id& operator=(army_rebel_control_id const& v) noexcept = default;
		army_rebel_control_id& operator=(army_rebel_control_id&& v) noexcept = default;
		constexpr bool operator==(army_rebel_control_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(army_rebel_control_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint16_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class army_rebel_control_id_pair {
		public:
		army_rebel_control_id left;
		army_rebel_control_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(army_rebel_control_id id) { return bool(id); }
	
	//
	// definition of strongly typed index for army_location_id
	//
	class army_location_id {
		public:
		using value_base_t = uint16_t;
		using zero_is_null_t = std::true_type;
		
		uint16_t value = 0;
		
		constexpr army_location_id() noexcept = default;
		explicit constexpr army_location_id(uint16_t v) noexcept : value(v + 1) {}
		constexpr army_location_id(army_location_id const& v) noexcept = default;
		constexpr army_location_id(army_location_id&& v) noexcept = default;
		
		army_location_id& operator=(army_location_id const& v) noexcept = default;
		army_location_id& operator=(army_location_id&& v) noexcept = default;
		constexpr bool operator==(army_location_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(army_location_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint16_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class army_location_id_pair {
		public:
		army_location_id left;
		army_location_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(army_location_id id) { return bool(id); }
	
	//
	// definition of strongly typed index for army_membership_id
	//
	class army_membership_id {
		public:
		using value_base_t = uint16_t;
		using zero_is_null_t = std::true_type;
		
		uint16_t value = 0;
		
		constexpr army_membership_id() noexcept = default;
		explicit constexpr army_membership_id(uint16_t v) noexcept : value(v + 1) {}
		constexpr army_membership_id(army_membership_id const& v) noexcept = default;
		constexpr army_membership_id(army_membership_id&& v) noexcept = default;
		
		army_membership_id& operator=(army_membership_id const& v) noexcept = default;
		army_membership_id& operator=(army_membership_id&& v) noexcept = default;
		constexpr bool operator==(army_membership_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(army_membership_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint16_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class army_membership_id_pair {
		public:
		army_membership_id left;
		army_membership_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(army_membership_id id) { return bool(id); }
	
	//
	// definition of strongly typed index for regiment_source_id
	//
	class regiment_source_id {
		public:
		using value_base_t = uint16_t;
		using zero_is_null_t = std::true_type;
		
		uint16_t value = 0;
		
		constexpr regiment_source_id() noexcept = default;
		explicit constexpr regiment_source_id(uint16_t v) noexcept : value(v + 1) {}
		constexpr regiment_source_id(regiment_source_id const& v) noexcept = default;
		constexpr regiment_source_id(regiment_source_id&& v) noexcept = default;
		
		regiment_source_id& operator=(regiment_source_id const& v) noexcept = default;
		regiment_source_id& operator=(regiment_source_id&& v) noexcept = default;
		constexpr bool operator==(regiment_source_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(regiment_source_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint16_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class regiment_source_id_pair {
		public:
		regiment_source_id left;
		regiment_source_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(regiment_source_id id) { return bool(id); }
	
	//
	// definition of strongly typed index for navy_control_id
	//
	class navy_control_id {
		public:
		using value_base_t = uint16_t;
		using zero_is_null_t = std::true_type;
		
		uint16_t value = 0;
		
		constexpr navy_control_id() noexcept = default;
		explicit constexpr navy_control_id(uint16_t v) noexcept : value(v + 1) {}
		constexpr navy_control_id(navy_control_id const& v) noexcept = default;
		constexpr navy_control_id(navy_control_id&& v) noexcept = default;
		
		navy_control_id& operator=(navy_control_id const& v) noexcept = default;
		navy_control_id& operator=(navy_control_id&& v) noexcept = default;
		constexpr bool operator==(navy_control_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(navy_control_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint16_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class navy_control_id_pair {
		public:
		navy_control_id left;
		navy_control_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(navy_control_id id) { return bool(id); }
	
	//
	// definition of strongly typed index for navy_location_id
	//
	class navy_location_id {
		public:
		using value_base_t = uint16_t;
		using zero_is_null_t = std::true_type;
		
		uint16_t value = 0;
		
		constexpr navy_location_id() noexcept = default;
		explicit constexpr navy_location_id(uint16_t v) noexcept : value(v + 1) {}
		constexpr navy_location_id(navy_location_id const& v) noexcept = default;
		constexpr navy_location_id(navy_location_id&& v) noexcept = default;
		
		navy_location_id& operator=(navy_location_id const& v) noexcept = default;
		navy_location_id& operator=(navy_location_id&& v) noexcept = default;
		constexpr bool operator==(navy_location_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(navy_location_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint16_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class navy_location_id_pair {
		public:
		navy_location_id left;
		navy_location_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(navy_location_id id) { return bool(id); }
	
	//
	// definition of strongly typed index for navy_membership_id
	//
	class navy_membership_id {
		public:
		using value_base_t = uint16_t;
		using zero_is_null_t = std::true_type;
		
		uint16_t value = 0;
		
		constexpr navy_membership_id() noexcept = default;
		explicit constexpr navy_membership_id(uint16_t v) noexcept : value(v + 1) {}
		constexpr navy_membership_id(navy_membership_id const& v) noexcept = default;
		constexpr navy_membership_id(navy_membership_id&& v) noexcept = default;
		
		navy_membership_id& operator=(navy_membership_id const& v) noexcept = default;
		navy_membership_id& operator=(navy_membership_id&& v) noexcept = default;
		constexpr bool operator==(navy_membership_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(navy_membership_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint16_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class navy_membership_id_pair {
		public:
		navy_membership_id left;
		navy_membership_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(navy_membership_id id) { return bool(id); }
	
	//
	// definition of strongly typed index for naval_battle_id
	//
	class naval_battle_id {
		public:
		using value_base_t = uint16_t;
		using zero_is_null_t = std::true_type;
		
		uint16_t value = 0;
		
		constexpr naval_battle_id() noexcept = default;
		explicit constexpr naval_battle_id(uint16_t v) noexcept : value(v + 1) {}
		constexpr naval_battle_id(naval_battle_id const& v) noexcept = default;
		constexpr naval_battle_id(naval_battle_id&& v) noexcept = default;
		
		naval_battle_id& operator=(naval_battle_id const& v) noexcept = default;
		naval_battle_id& operator=(naval_battle_id&& v) noexcept = default;
		constexpr bool operator==(naval_battle_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(naval_battle_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint16_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class naval_battle_id_pair {
		public:
		naval_battle_id left;
		naval_battle_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(naval_battle_id id) { return bool(id); }
	
	//
	// definition of strongly typed index for naval_battle_location_id
	//
	class naval_battle_location_id {
		public:
		using value_base_t = uint16_t;
		using zero_is_null_t = std::true_type;
		
		uint16_t value = 0;
		
		constexpr naval_battle_location_id() noexcept = default;
		explicit constexpr naval_battle_location_id(uint16_t v) noexcept : value(v + 1) {}
		constexpr naval_battle_location_id(naval_battle_location_id const& v) noexcept = default;
		constexpr naval_battle_location_id(naval_battle_location_id&& v) noexcept = default;
		
		naval_battle_location_id& operator=(naval_battle_location_id const& v) noexcept = default;
		naval_battle_location_id& operator=(naval_battle_location_id&& v) noexcept = default;
		constexpr bool operator==(naval_battle_location_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(naval_battle_location_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint16_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class naval_battle_location_id_pair {
		public:
		naval_battle_location_id left;
		naval_battle_location_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(naval_battle_location_id id) { return bool(id); }
	
	//
	// definition of strongly typed index for naval_battle_in_war_id
	//
	class naval_battle_in_war_id {
		public:
		using value_base_t = uint16_t;
		using zero_is_null_t = std::true_type;
		
		uint16_t value = 0;
		
		constexpr naval_battle_in_war_id() noexcept = default;
		explicit constexpr naval_battle_in_war_id(uint16_t v) noexcept : value(v + 1) {}
		constexpr naval_battle_in_war_id(naval_battle_in_war_id const& v) noexcept = default;
		constexpr naval_battle_in_war_id(naval_battle_in_war_id&& v) noexcept = default;
		
		naval_battle_in_war_id& operator=(naval_battle_in_war_id const& v) noexcept = default;
		naval_battle_in_war_id& operator=(naval_battle_in_war_id&& v) noexcept = default;
		constexpr bool operator==(naval_battle_in_war_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(naval_battle_in_war_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint16_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class naval_battle_in_war_id_pair {
		public:
		naval_battle_in_war_id left;
		naval_battle_in_war_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(naval_battle_in_war_id id) { return bool(id); }
	
	//
	// definition of strongly typed index for navy_battle_participation_id
	//
	class navy_battle_participation_id {
		public:
		using value_base_t = uint16_t;
		using zero_is_null_t = std::true_type;
		
		uint16_t value = 0;
		
		constexpr navy_battle_participation_id() noexcept = default;
		explicit constexpr navy_battle_participation_id(uint16_t v) noexcept : value(v + 1) {}
		constexpr navy_battle_participation_id(navy_battle_participation_id const& v) noexcept = default;
		constexpr navy_battle_participation_id(navy_battle_participation_id&& v) noexcept = default;
		
		navy_battle_participation_id& operator=(navy_battle_participation_id const& v) noexcept = default;
		navy_battle_participation_id& operator=(navy_battle_participation_id&& v) noexcept = default;
		constexpr bool operator==(navy_battle_participation_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(navy_battle_participation_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint16_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class navy_battle_participation_id_pair {
		public:
		navy_battle_participation_id left;
		navy_battle_participation_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(navy_battle_participation_id id) { return bool(id); }
	
	//
	// definition of strongly typed index for attacking_admiral_id
	//
	class attacking_admiral_id {
		public:
		using value_base_t = uint16_t;
		using zero_is_null_t = std::true_type;
		
		uint16_t value = 0;
		
		constexpr attacking_admiral_id() noexcept = default;
		explicit constexpr attacking_admiral_id(uint16_t v) noexcept : value(v + 1) {}
		constexpr attacking_admiral_id(attacking_admiral_id const& v) noexcept = default;
		constexpr attacking_admiral_id(attacking_admiral_id&& v) noexcept = default;
		
		attacking_admiral_id& operator=(attacking_admiral_id const& v) noexcept = default;
		attacking_admiral_id& operator=(attacking_admiral_id&& v) noexcept = default;
		constexpr bool operator==(attacking_admiral_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(attacking_admiral_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint16_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class attacking_admiral_id_pair {
		public:
		attacking_admiral_id left;
		attacking_admiral_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(attacking_admiral_id id) { return bool(id); }
	
	//
	// definition of strongly typed index for defending_admiral_id
	//
	class defending_admiral_id {
		public:
		using value_base_t = uint16_t;
		using zero_is_null_t = std::true_type;
		
		uint16_t value = 0;
		
		constexpr defending_admiral_id() noexcept = default;
		explicit constexpr defending_admiral_id(uint16_t v) noexcept : value(v + 1) {}
		constexpr defending_admiral_id(defending_admiral_id const& v) noexcept = default;
		constexpr defending_admiral_id(defending_admiral_id&& v) noexcept = default;
		
		defending_admiral_id& operator=(defending_admiral_id const& v) noexcept = default;
		defending_admiral_id& operator=(defending_admiral_id&& v) noexcept = default;
		constexpr bool operator==(defending_admiral_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(defending_admiral_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint16_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class defending_admiral_id_pair {
		public:
		defending_admiral_id left;
		defending_admiral_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(defending_admiral_id id) { return bool(id); }
	
	//
	// definition of strongly typed index for land_battle_id
	//
	class land_battle_id {
		public:
		using value_base_t = uint16_t;
		using zero_is_null_t = std::true_type;
		
		uint16_t value = 0;
		
		constexpr land_battle_id() noexcept = default;
		explicit constexpr land_battle_id(uint16_t v) noexcept : value(v + 1) {}
		constexpr land_battle_id(land_battle_id const& v) noexcept = default;
		constexpr land_battle_id(land_battle_id&& v) noexcept = default;
		
		land_battle_id& operator=(land_battle_id const& v) noexcept = default;
		land_battle_id& operator=(land_battle_id&& v) noexcept = default;
		constexpr bool operator==(land_battle_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(land_battle_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint16_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class land_battle_id_pair {
		public:
		land_battle_id left;
		land_battle_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(land_battle_id id) { return bool(id); }
	
	//
	// definition of strongly typed index for land_battle_location_id
	//
	class land_battle_location_id {
		public:
		using value_base_t = uint16_t;
		using zero_is_null_t = std::true_type;
		
		uint16_t value = 0;
		
		constexpr land_battle_location_id() noexcept = default;
		explicit constexpr land_battle_location_id(uint16_t v) noexcept : value(v + 1) {}
		constexpr land_battle_location_id(land_battle_location_id const& v) noexcept = default;
		constexpr land_battle_location_id(land_battle_location_id&& v) noexcept = default;
		
		land_battle_location_id& operator=(land_battle_location_id const& v) noexcept = default;
		land_battle_location_id& operator=(land_battle_location_id&& v) noexcept = default;
		constexpr bool operator==(land_battle_location_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(land_battle_location_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint16_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class land_battle_location_id_pair {
		public:
		land_battle_location_id left;
		land_battle_location_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(land_battle_location_id id) { return bool(id); }
	
	//
	// definition of strongly typed index for land_battle_in_war_id
	//
	class land_battle_in_war_id {
		public:
		using value_base_t = uint16_t;
		using zero_is_null_t = std::true_type;
		
		uint16_t value = 0;
		
		constexpr land_battle_in_war_id() noexcept = default;
		explicit constexpr land_battle_in_war_id(uint16_t v) noexcept : value(v + 1) {}
		constexpr land_battle_in_war_id(land_battle_in_war_id const& v) noexcept = default;
		constexpr land_battle_in_war_id(land_battle_in_war_id&& v) noexcept = default;
		
		land_battle_in_war_id& operator=(land_battle_in_war_id const& v) noexcept = default;
		land_battle_in_war_id& operator=(land_battle_in_war_id&& v) noexcept = default;
		constexpr bool operator==(land_battle_in_war_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(land_battle_in_war_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint16_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class land_battle_in_war_id_pair {
		public:
		land_battle_in_war_id left;
		land_battle_in_war_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(land_battle_in_war_id id) { return bool(id); }
	
	//
	// definition of strongly typed index for army_battle_participation_id
	//
	class army_battle_participation_id {
		public:
		using value_base_t = uint16_t;
		using zero_is_null_t = std::true_type;
		
		uint16_t value = 0;
		
		constexpr army_battle_participation_id() noexcept = default;
		explicit constexpr army_battle_participation_id(uint16_t v) noexcept : value(v + 1) {}
		constexpr army_battle_participation_id(army_battle_participation_id const& v) noexcept = default;
		constexpr army_battle_participation_id(army_battle_participation_id&& v) noexcept = default;
		
		army_battle_participation_id& operator=(army_battle_participation_id const& v) noexcept = default;
		army_battle_participation_id& operator=(army_battle_participation_id&& v) noexcept = default;
		constexpr bool operator==(army_battle_participation_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(army_battle_participation_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint16_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class army_battle_participation_id_pair {
		public:
		army_battle_participation_id left;
		army_battle_participation_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(army_battle_participation_id id) { return bool(id); }
	
	//
	// definition of strongly typed index for attacking_general_id
	//
	class attacking_general_id {
		public:
		using value_base_t = uint16_t;
		using zero_is_null_t = std::true_type;
		
		uint16_t value = 0;
		
		constexpr attacking_general_id() noexcept = default;
		explicit constexpr attacking_general_id(uint16_t v) noexcept : value(v + 1) {}
		constexpr attacking_general_id(attacking_general_id const& v) noexcept = default;
		constexpr attacking_general_id(attacking_general_id&& v) noexcept = default;
		
		attacking_general_id& operator=(attacking_general_id const& v) noexcept = default;
		attacking_general_id& operator=(attacking_general_id&& v) noexcept = default;
		constexpr bool operator==(attacking_general_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(attacking_general_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint16_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class attacking_general_id_pair {
		public:
		attacking_general_id left;
		attacking_general_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(attacking_general_id id) { return bool(id); }
	
	//
	// definition of strongly typed index for defending_general_id
	//
	class defending_general_id {
		public:
		using value_base_t = uint16_t;
		using zero_is_null_t = std::true_type;
		
		uint16_t value = 0;
		
		constexpr defending_general_id() noexcept = default;
		explicit constexpr defending_general_id(uint16_t v) noexcept : value(v + 1) {}
		constexpr defending_general_id(defending_general_id const& v) noexcept = default;
		constexpr defending_general_id(defending_general_id&& v) noexcept = default;
		
		defending_general_id& operator=(defending_general_id const& v) noexcept = default;
		defending_general_id& operator=(defending_general_id&& v) noexcept = default;
		constexpr bool operator==(defending_general_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(defending_general_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint16_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class defending_general_id_pair {
		public:
		defending_general_id left;
		defending_general_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(defending_general_id id) { return bool(id); }
	
	//
	// definition of strongly typed index for leader_id
	//
	class leader_id {
		public:
		using value_base_t = uint16_t;
		using zero_is_null_t = std::true_type;
		
		uint16_t value = 0;
		
		constexpr leader_id() noexcept = default;
		explicit constexpr leader_id(uint16_t v) noexcept : value(v + 1) {}
		constexpr leader_id(leader_id const& v) noexcept = default;
		constexpr leader_id(leader_id&& v) noexcept = default;
		
		leader_id& operator=(leader_id const& v) noexcept = default;
		leader_id& operator=(leader_id&& v) noexcept = default;
		constexpr bool operator==(leader_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(leader_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint16_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class leader_id_pair {
		public:
		leader_id left;
		leader_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(leader_id id) { return bool(id); }
	
	//
	// definition of strongly typed index for army_leadership_id
	//
	class army_leadership_id {
		public:
		using value_base_t = uint16_t;
		using zero_is_null_t = std::true_type;
		
		uint16_t value = 0;
		
		constexpr army_leadership_id() noexcept = default;
		explicit constexpr army_leadership_id(uint16_t v) noexcept : value(v + 1) {}
		constexpr army_leadership_id(army_leadership_id const& v) noexcept = default;
		constexpr army_leadership_id(army_leadership_id&& v) noexcept = default;
		
		army_leadership_id& operator=(army_leadership_id const& v) noexcept = default;
		army_leadership_id& operator=(army_leadership_id&& v) noexcept = default;
		constexpr bool operator==(army_leadership_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(army_leadership_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint16_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class army_leadership_id_pair {
		public:
		army_leadership_id left;
		army_leadership_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(army_leadership_id id) { return bool(id); }
	
	//
	// definition of strongly typed index for navy_leadership_id
	//
	class navy_leadership_id {
		public:
		using value_base_t = uint16_t;
		using zero_is_null_t = std::true_type;
		
		uint16_t value = 0;
		
		constexpr navy_leadership_id() noexcept = default;
		explicit constexpr navy_leadership_id(uint16_t v) noexcept : value(v + 1) {}
		constexpr navy_leadership_id(navy_leadership_id const& v) noexcept = default;
		constexpr navy_leadership_id(navy_leadership_id&& v) noexcept = default;
		
		navy_leadership_id& operator=(navy_leadership_id const& v) noexcept = default;
		navy_leadership_id& operator=(navy_leadership_id&& v) noexcept = default;
		constexpr bool operator==(navy_leadership_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(navy_leadership_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint16_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class navy_leadership_id_pair {
		public:
		navy_leadership_id left;
		navy_leadership_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(navy_leadership_id id) { return bool(id); }
	
	//
	// definition of strongly typed index for leader_loyalty_id
	//
	class leader_loyalty_id {
		public:
		using value_base_t = uint16_t;
		using zero_is_null_t = std::true_type;
		
		uint16_t value = 0;
		
		constexpr leader_loyalty_id() noexcept = default;
		explicit constexpr leader_loyalty_id(uint16_t v) noexcept : value(v + 1) {}
		constexpr leader_loyalty_id(leader_loyalty_id const& v) noexcept = default;
		constexpr leader_loyalty_id(leader_loyalty_id&& v) noexcept = default;
		
		leader_loyalty_id& operator=(leader_loyalty_id const& v) noexcept = default;
		leader_loyalty_id& operator=(leader_loyalty_id&& v) noexcept = default;
		constexpr bool operator==(leader_loyalty_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(leader_loyalty_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint16_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class leader_loyalty_id_pair {
		public:
		leader_loyalty_id left;
		leader_loyalty_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(leader_loyalty_id id) { return bool(id); }
	
	//
	// definition of strongly typed index for war_id
	//
	class war_id {
		public:
		using value_base_t = uint8_t;
		using zero_is_null_t = std::true_type;
		
		uint8_t value = 0;
		
		constexpr war_id() noexcept = default;
		explicit constexpr war_id(uint8_t v) noexcept : value(v + 1) {}
		constexpr war_id(war_id const& v) noexcept = default;
		constexpr war_id(war_id&& v) noexcept = default;
		
		war_id& operator=(war_id const& v) noexcept = default;
		war_id& operator=(war_id&& v) noexcept = default;
		constexpr bool operator==(war_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(war_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint8_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class war_id_pair {
		public:
		war_id left;
		war_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(war_id id) { return bool(id); }
	
	//
	// definition of strongly typed index for peace_offer_id
	//
	class peace_offer_id {
		public:
		using value_base_t = uint8_t;
		using zero_is_null_t = std::true_type;
		
		uint8_t value = 0;
		
		constexpr peace_offer_id() noexcept = default;
		explicit constexpr peace_offer_id(uint8_t v) noexcept : value(v + 1) {}
		constexpr peace_offer_id(peace_offer_id const& v) noexcept = default;
		constexpr peace_offer_id(peace_offer_id&& v) noexcept = default;
		
		peace_offer_id& operator=(peace_offer_id const& v) noexcept = default;
		peace_offer_id& operator=(peace_offer_id&& v) noexcept = default;
		constexpr bool operator==(peace_offer_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(peace_offer_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint8_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class peace_offer_id_pair {
		public:
		peace_offer_id left;
		peace_offer_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(peace_offer_id id) { return bool(id); }
	
	//
	// definition of strongly typed index for pending_peace_offer_id
	//
	class pending_peace_offer_id {
		public:
		using value_base_t = uint8_t;
		using zero_is_null_t = std::true_type;
		
		uint8_t value = 0;
		
		constexpr pending_peace_offer_id() noexcept = default;
		explicit constexpr pending_peace_offer_id(uint8_t v) noexcept : value(v + 1) {}
		constexpr pending_peace_offer_id(pending_peace_offer_id const& v) noexcept = default;
		constexpr pending_peace_offer_id(pending_peace_offer_id&& v) noexcept = default;
		
		pending_peace_offer_id& operator=(pending_peace_offer_id const& v) noexcept = default;
		pending_peace_offer_id& operator=(pending_peace_offer_id&& v) noexcept = default;
		constexpr bool operator==(pending_peace_offer_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(pending_peace_offer_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint8_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class pending_peace_offer_id_pair {
		public:
		pending_peace_offer_id left;
		pending_peace_offer_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(pending_peace_offer_id id) { return bool(id); }
	
	//
	// definition of strongly typed index for war_settlement_id
	//
	class war_settlement_id {
		public:
		using value_base_t = uint8_t;
		using zero_is_null_t = std::true_type;
		
		uint8_t value = 0;
		
		constexpr war_settlement_id() noexcept = default;
		explicit constexpr war_settlement_id(uint8_t v) noexcept : value(v + 1) {}
		constexpr war_settlement_id(war_settlement_id const& v) noexcept = default;
		constexpr war_settlement_id(war_settlement_id&& v) noexcept = default;
		
		war_settlement_id& operator=(war_settlement_id const& v) noexcept = default;
		war_settlement_id& operator=(war_settlement_id&& v) noexcept = default;
		constexpr bool operator==(war_settlement_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(war_settlement_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint8_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class war_settlement_id_pair {
		public:
		war_settlement_id left;
		war_settlement_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(war_settlement_id id) { return bool(id); }
	
	//
	// definition of strongly typed index for wargoal_id
	//
	class wargoal_id {
		public:
		using value_base_t = uint16_t;
		using zero_is_null_t = std::true_type;
		
		uint16_t value = 0;
		
		constexpr wargoal_id() noexcept = default;
		explicit constexpr wargoal_id(uint16_t v) noexcept : value(v + 1) {}
		constexpr wargoal_id(wargoal_id const& v) noexcept = default;
		constexpr wargoal_id(wargoal_id&& v) noexcept = default;
		
		wargoal_id& operator=(wargoal_id const& v) noexcept = default;
		wargoal_id& operator=(wargoal_id&& v) noexcept = default;
		constexpr bool operator==(wargoal_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(wargoal_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint16_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class wargoal_id_pair {
		public:
		wargoal_id left;
		wargoal_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(wargoal_id id) { return bool(id); }
	
	//
	// definition of strongly typed index for war_participant_id
	//
	class war_participant_id {
		public:
		using value_base_t = uint16_t;
		using zero_is_null_t = std::true_type;
		
		uint16_t value = 0;
		
		constexpr war_participant_id() noexcept = default;
		explicit constexpr war_participant_id(uint16_t v) noexcept : value(v + 1) {}
		constexpr war_participant_id(war_participant_id const& v) noexcept = default;
		constexpr war_participant_id(war_participant_id&& v) noexcept = default;
		
		war_participant_id& operator=(war_participant_id const& v) noexcept = default;
		war_participant_id& operator=(war_participant_id&& v) noexcept = default;
		constexpr bool operator==(war_participant_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(war_participant_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint16_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class war_participant_id_pair {
		public:
		war_participant_id left;
		war_participant_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(war_participant_id id) { return bool(id); }
	
	//
	// definition of strongly typed index for wargoals_attached_id
	//
	class wargoals_attached_id {
		public:
		using value_base_t = uint16_t;
		using zero_is_null_t = std::true_type;
		
		uint16_t value = 0;
		
		constexpr wargoals_attached_id() noexcept = default;
		explicit constexpr wargoals_attached_id(uint16_t v) noexcept : value(v + 1) {}
		constexpr wargoals_attached_id(wargoals_attached_id const& v) noexcept = default;
		constexpr wargoals_attached_id(wargoals_attached_id&& v) noexcept = default;
		
		wargoals_attached_id& operator=(wargoals_attached_id const& v) noexcept = default;
		wargoals_attached_id& operator=(wargoals_attached_id&& v) noexcept = default;
		constexpr bool operator==(wargoals_attached_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(wargoals_attached_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint16_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class wargoals_attached_id_pair {
		public:
		wargoals_attached_id left;
		wargoals_attached_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(wargoals_attached_id id) { return bool(id); }
	
	//
	// definition of strongly typed index for peace_offer_item_id
	//
	class peace_offer_item_id {
		public:
		using value_base_t = uint16_t;
		using zero_is_null_t = std::true_type;
		
		uint16_t value = 0;
		
		constexpr peace_offer_item_id() noexcept = default;
		explicit constexpr peace_offer_item_id(uint16_t v) noexcept : value(v + 1) {}
		constexpr peace_offer_item_id(peace_offer_item_id const& v) noexcept = default;
		constexpr peace_offer_item_id(peace_offer_item_id&& v) noexcept = default;
		
		peace_offer_item_id& operator=(peace_offer_item_id const& v) noexcept = default;
		peace_offer_item_id& operator=(peace_offer_item_id&& v) noexcept = default;
		constexpr bool operator==(peace_offer_item_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(peace_offer_item_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint16_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class peace_offer_item_id_pair {
		public:
		peace_offer_item_id left;
		peace_offer_item_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(peace_offer_item_id id) { return bool(id); }
	
	//
	// definition of strongly typed index for state_definition_id
	//
	class state_definition_id {
		public:
		using value_base_t = uint16_t;
		using zero_is_null_t = std::true_type;
		
		uint16_t value = 0;
		
		constexpr state_definition_id() noexcept = default;
		explicit constexpr state_definition_id(uint16_t v) noexcept : value(v + 1) {}
		constexpr state_definition_id(state_definition_id const& v) noexcept = default;
		constexpr state_definition_id(state_definition_id&& v) noexcept = default;
		
		state_definition_id& operator=(state_definition_id const& v) noexcept = default;
		state_definition_id& operator=(state_definition_id&& v) noexcept = default;
		constexpr bool operator==(state_definition_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(state_definition_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint16_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class state_definition_id_pair {
		public:
		state_definition_id left;
		state_definition_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(state_definition_id id) { return bool(id); }
	
	//
	// definition of strongly typed index for administration_id
	//
	class administration_id {
		public:
		using value_base_t = uint16_t;
		using zero_is_null_t = std::true_type;
		
		uint16_t value = 0;
		
		constexpr administration_id() noexcept = default;
		explicit constexpr administration_id(uint16_t v) noexcept : value(v + 1) {}
		constexpr administration_id(administration_id const& v) noexcept = default;
		constexpr administration_id(administration_id&& v) noexcept = default;
		
		administration_id& operator=(administration_id const& v) noexcept = default;
		administration_id& operator=(administration_id&& v) noexcept = default;
		constexpr bool operator==(administration_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(administration_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint16_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class administration_id_pair {
		public:
		administration_id left;
		administration_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(administration_id id) { return bool(id); }
	
	//
	// definition of strongly typed index for nation_administration_id
	//
	class nation_administration_id {
		public:
		using value_base_t = uint16_t;
		using zero_is_null_t = std::true_type;
		
		uint16_t value = 0;
		
		constexpr nation_administration_id() noexcept = default;
		explicit constexpr nation_administration_id(uint16_t v) noexcept : value(v + 1) {}
		constexpr nation_administration_id(nation_administration_id const& v) noexcept = default;
		constexpr nation_administration_id(nation_administration_id&& v) noexcept = default;
		
		nation_administration_id& operator=(nation_administration_id const& v) noexcept = default;
		nation_administration_id& operator=(nation_administration_id&& v) noexcept = default;
		constexpr bool operator==(nation_administration_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(nation_administration_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint16_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class nation_administration_id_pair {
		public:
		nation_administration_id left;
		nation_administration_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(nation_administration_id id) { return bool(id); }
	
	//
	// definition of strongly typed index for state_instance_id
	//
	class state_instance_id {
		public:
		using value_base_t = uint16_t;
		using zero_is_null_t = std::true_type;
		
		uint16_t value = 0;
		
		constexpr state_instance_id() noexcept = default;
		explicit constexpr state_instance_id(uint16_t v) noexcept : value(v + 1) {}
		constexpr state_instance_id(state_instance_id const& v) noexcept = default;
		constexpr state_instance_id(state_instance_id&& v) noexcept = default;
		
		state_instance_id& operator=(state_instance_id const& v) noexcept = default;
		state_instance_id& operator=(state_instance_id&& v) noexcept = default;
		constexpr bool operator==(state_instance_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(state_instance_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint16_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class state_instance_id_pair {
		public:
		state_instance_id left;
		state_instance_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(state_instance_id id) { return bool(id); }
	
	//
	// definition of strongly typed index for colonization_id
	//
	class colonization_id {
		public:
		using value_base_t = uint16_t;
		using zero_is_null_t = std::true_type;
		
		uint16_t value = 0;
		
		constexpr colonization_id() noexcept = default;
		explicit constexpr colonization_id(uint16_t v) noexcept : value(v + 1) {}
		constexpr colonization_id(colonization_id const& v) noexcept = default;
		constexpr colonization_id(colonization_id&& v) noexcept = default;
		
		colonization_id& operator=(colonization_id const& v) noexcept = default;
		colonization_id& operator=(colonization_id&& v) noexcept = default;
		constexpr bool operator==(colonization_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(colonization_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint16_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class colonization_id_pair {
		public:
		colonization_id left;
		colonization_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(colonization_id id) { return bool(id); }
	
	//
	// definition of strongly typed index for state_ownership_id
	//
	class state_ownership_id {
		public:
		using value_base_t = uint16_t;
		using zero_is_null_t = std::true_type;
		
		uint16_t value = 0;
		
		constexpr state_ownership_id() noexcept = default;
		explicit constexpr state_ownership_id(uint16_t v) noexcept : value(v + 1) {}
		constexpr state_ownership_id(state_ownership_id const& v) noexcept = default;
		constexpr state_ownership_id(state_ownership_id&& v) noexcept = default;
		
		state_ownership_id& operator=(state_ownership_id const& v) noexcept = default;
		state_ownership_id& operator=(state_ownership_id&& v) noexcept = default;
		constexpr bool operator==(state_ownership_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(state_ownership_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint16_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class state_ownership_id_pair {
		public:
		state_ownership_id left;
		state_ownership_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(state_ownership_id id) { return bool(id); }
	
	//
	// definition of strongly typed index for flashpoint_focus_id
	//
	class flashpoint_focus_id {
		public:
		using value_base_t = uint16_t;
		using zero_is_null_t = std::true_type;
		
		uint16_t value = 0;
		
		constexpr flashpoint_focus_id() noexcept = default;
		explicit constexpr flashpoint_focus_id(uint16_t v) noexcept : value(v + 1) {}
		constexpr flashpoint_focus_id(flashpoint_focus_id const& v) noexcept = default;
		constexpr flashpoint_focus_id(flashpoint_focus_id&& v) noexcept = default;
		
		flashpoint_focus_id& operator=(flashpoint_focus_id const& v) noexcept = default;
		flashpoint_focus_id& operator=(flashpoint_focus_id&& v) noexcept = default;
		constexpr bool operator==(flashpoint_focus_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(flashpoint_focus_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint16_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class flashpoint_focus_id_pair {
		public:
		flashpoint_focus_id left;
		flashpoint_focus_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(flashpoint_focus_id id) { return bool(id); }
	
	//
	// definition of strongly typed index for abstract_state_membership_id
	//
	class abstract_state_membership_id {
		public:
		using value_base_t = uint16_t;
		using zero_is_null_t = std::true_type;
		
		uint16_t value = 0;
		
		constexpr abstract_state_membership_id() noexcept = default;
		explicit constexpr abstract_state_membership_id(uint16_t v) noexcept : value(v + 1) {}
		constexpr abstract_state_membership_id(abstract_state_membership_id const& v) noexcept = default;
		constexpr abstract_state_membership_id(abstract_state_membership_id&& v) noexcept = default;
		
		abstract_state_membership_id& operator=(abstract_state_membership_id const& v) noexcept = default;
		abstract_state_membership_id& operator=(abstract_state_membership_id&& v) noexcept = default;
		constexpr bool operator==(abstract_state_membership_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(abstract_state_membership_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint16_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class abstract_state_membership_id_pair {
		public:
		abstract_state_membership_id left;
		abstract_state_membership_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(abstract_state_membership_id id) { return bool(id); }
	
	//
	// definition of strongly typed index for region_id
	//
	class region_id {
		public:
		using value_base_t = uint16_t;
		using zero_is_null_t = std::true_type;
		
		uint16_t value = 0;
		
		constexpr region_id() noexcept = default;
		explicit constexpr region_id(uint16_t v) noexcept : value(v + 1) {}
		constexpr region_id(region_id const& v) noexcept = default;
		constexpr region_id(region_id&& v) noexcept = default;
		
		region_id& operator=(region_id const& v) noexcept = default;
		region_id& operator=(region_id&& v) noexcept = default;
		constexpr bool operator==(region_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(region_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint16_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class region_id_pair {
		public:
		region_id left;
		region_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(region_id id) { return bool(id); }
	
	//
	// definition of strongly typed index for region_membership_id
	//
	class region_membership_id {
		public:
		using value_base_t = uint16_t;
		using zero_is_null_t = std::true_type;
		
		uint16_t value = 0;
		
		constexpr region_membership_id() noexcept = default;
		explicit constexpr region_membership_id(uint16_t v) noexcept : value(v + 1) {}
		constexpr region_membership_id(region_membership_id const& v) noexcept = default;
		constexpr region_membership_id(region_membership_id&& v) noexcept = default;
		
		region_membership_id& operator=(region_membership_id const& v) noexcept = default;
		region_membership_id& operator=(region_membership_id&& v) noexcept = default;
		constexpr bool operator==(region_membership_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(region_membership_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint16_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class region_membership_id_pair {
		public:
		region_membership_id left;
		region_membership_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(region_membership_id id) { return bool(id); }
	
	//
	// definition of strongly typed index for core_id
	//
	class core_id {
		public:
		using value_base_t = uint16_t;
		using zero_is_null_t = std::true_type;
		
		uint16_t value = 0;
		
		constexpr core_id() noexcept = default;
		explicit constexpr core_id(uint16_t v) noexcept : value(v + 1) {}
		constexpr core_id(core_id const& v) noexcept = default;
		constexpr core_id(core_id&& v) noexcept = default;
		
		core_id& operator=(core_id const& v) noexcept = default;
		core_id& operator=(core_id&& v) noexcept = default;
		constexpr bool operator==(core_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(core_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint16_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class core_id_pair {
		public:
		core_id left;
		core_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(core_id id) { return bool(id); }
	
	//
	// definition of strongly typed index for identity_holder_id
	//
	class identity_holder_id {
		public:
		using value_base_t = uint16_t;
		using zero_is_null_t = std::true_type;
		
		uint16_t value = 0;
		
		constexpr identity_holder_id() noexcept = default;
		explicit constexpr identity_holder_id(uint16_t v) noexcept : value(v + 1) {}
		constexpr identity_holder_id(identity_holder_id const& v) noexcept = default;
		constexpr identity_holder_id(identity_holder_id&& v) noexcept = default;
		
		identity_holder_id& operator=(identity_holder_id const& v) noexcept = default;
		identity_holder_id& operator=(identity_holder_id&& v) noexcept = default;
		constexpr bool operator==(identity_holder_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(identity_holder_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint16_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class identity_holder_id_pair {
		public:
		identity_holder_id left;
		identity_holder_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(identity_holder_id id) { return bool(id); }
	
	//
	// definition of strongly typed index for technology_id
	//
	class technology_id {
		public:
		using value_base_t = uint16_t;
		using zero_is_null_t = std::true_type;
		
		uint16_t value = 0;
		
		constexpr technology_id() noexcept = default;
		explicit constexpr technology_id(uint16_t v) noexcept : value(v + 1) {}
		constexpr technology_id(technology_id const& v) noexcept = default;
		constexpr technology_id(technology_id&& v) noexcept = default;
		
		technology_id& operator=(technology_id const& v) noexcept = default;
		technology_id& operator=(technology_id&& v) noexcept = default;
		constexpr bool operator==(technology_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(technology_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint16_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class technology_id_pair {
		public:
		technology_id left;
		technology_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(technology_id id) { return bool(id); }
	
	//
	// definition of strongly typed index for invention_id
	//
	class invention_id {
		public:
		using value_base_t = uint16_t;
		using zero_is_null_t = std::true_type;
		
		uint16_t value = 0;
		
		constexpr invention_id() noexcept = default;
		explicit constexpr invention_id(uint16_t v) noexcept : value(v + 1) {}
		constexpr invention_id(invention_id const& v) noexcept = default;
		constexpr invention_id(invention_id&& v) noexcept = default;
		
		invention_id& operator=(invention_id const& v) noexcept = default;
		invention_id& operator=(invention_id&& v) noexcept = default;
		constexpr bool operator==(invention_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(invention_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint16_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class invention_id_pair {
		public:
		invention_id left;
		invention_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(invention_id id) { return bool(id); }
	
	//
	// definition of strongly typed index for nation_id
	//
	class nation_id {
		public:
		using value_base_t = uint16_t;
		using zero_is_null_t = std::true_type;
		
		uint16_t value = 0;
		
		constexpr nation_id() noexcept = default;
		explicit constexpr nation_id(uint16_t v) noexcept : value(v + 1) {}
		constexpr nation_id(nation_id const& v) noexcept = default;
		constexpr nation_id(nation_id&& v) noexcept = default;
		
		nation_id& operator=(nation_id const& v) noexcept = default;
		nation_id& operator=(nation_id&& v) noexcept = default;
		constexpr bool operator==(nation_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(nation_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint16_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class nation_id_pair {
		public:
		nation_id left;
		nation_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(nation_id id) { return bool(id); }
	
	//
	// definition of strongly typed index for market_id
	//
	class market_id {
		public:
		using value_base_t = uint16_t;
		using zero_is_null_t = std::true_type;
		
		uint16_t value = 0;
		
		constexpr market_id() noexcept = default;
		explicit constexpr market_id(uint16_t v) noexcept : value(v + 1) {}
		constexpr market_id(market_id const& v) noexcept = default;
		constexpr market_id(market_id&& v) noexcept = default;
		
		market_id& operator=(market_id const& v) noexcept = default;
		market_id& operator=(market_id&& v) noexcept = default;
		constexpr bool operator==(market_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(market_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint16_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class market_id_pair {
		public:
		market_id left;
		market_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(market_id id) { return bool(id); }
	
	//
	// definition of strongly typed index for local_market_id
	//
	class local_market_id {
		public:
		using value_base_t = uint16_t;
		using zero_is_null_t = std::true_type;
		
		uint16_t value = 0;
		
		constexpr local_market_id() noexcept = default;
		explicit constexpr local_market_id(uint16_t v) noexcept : value(v + 1) {}
		constexpr local_market_id(local_market_id const& v) noexcept = default;
		constexpr local_market_id(local_market_id&& v) noexcept = default;
		
		local_market_id& operator=(local_market_id const& v) noexcept = default;
		local_market_id& operator=(local_market_id&& v) noexcept = default;
		constexpr bool operator==(local_market_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(local_market_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint16_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class local_market_id_pair {
		public:
		local_market_id left;
		local_market_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(local_market_id id) { return bool(id); }
	
	//
	// definition of strongly typed index for trade_route_id
	//
	class trade_route_id {
		public:
		using value_base_t = uint32_t;
		using zero_is_null_t = std::true_type;
		
		uint32_t value = 0;
		
		constexpr trade_route_id() noexcept = default;
		explicit constexpr trade_route_id(uint32_t v) noexcept : value(v + 1) {}
		constexpr trade_route_id(trade_route_id const& v) noexcept = default;
		constexpr trade_route_id(trade_route_id&& v) noexcept = default;
		
		trade_route_id& operator=(trade_route_id const& v) noexcept = default;
		trade_route_id& operator=(trade_route_id&& v) noexcept = default;
		constexpr bool operator==(trade_route_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(trade_route_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint32_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class trade_route_id_pair {
		public:
		trade_route_id left;
		trade_route_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(trade_route_id id) { return bool(id); }
	
	//
	// definition of strongly typed index for diplomatic_relation_id
	//
	class diplomatic_relation_id {
		public:
		using value_base_t = uint32_t;
		using zero_is_null_t = std::true_type;
		
		uint32_t value = 0;
		
		constexpr diplomatic_relation_id() noexcept = default;
		explicit constexpr diplomatic_relation_id(uint32_t v) noexcept : value(v + 1) {}
		constexpr diplomatic_relation_id(diplomatic_relation_id const& v) noexcept = default;
		constexpr diplomatic_relation_id(diplomatic_relation_id&& v) noexcept = default;
		
		diplomatic_relation_id& operator=(diplomatic_relation_id const& v) noexcept = default;
		diplomatic_relation_id& operator=(diplomatic_relation_id&& v) noexcept = default;
		constexpr bool operator==(diplomatic_relation_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(diplomatic_relation_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint32_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class diplomatic_relation_id_pair {
		public:
		diplomatic_relation_id left;
		diplomatic_relation_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(diplomatic_relation_id id) { return bool(id); }
	
	//
	// definition of strongly typed index for unilateral_relationship_id
	//
	class unilateral_relationship_id {
		public:
		using value_base_t = uint32_t;
		using zero_is_null_t = std::true_type;
		
		uint32_t value = 0;
		
		constexpr unilateral_relationship_id() noexcept = default;
		explicit constexpr unilateral_relationship_id(uint32_t v) noexcept : value(v + 1) {}
		constexpr unilateral_relationship_id(unilateral_relationship_id const& v) noexcept = default;
		constexpr unilateral_relationship_id(unilateral_relationship_id&& v) noexcept = default;
		
		unilateral_relationship_id& operator=(unilateral_relationship_id const& v) noexcept = default;
		unilateral_relationship_id& operator=(unilateral_relationship_id&& v) noexcept = default;
		constexpr bool operator==(unilateral_relationship_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(unilateral_relationship_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint32_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class unilateral_relationship_id_pair {
		public:
		unilateral_relationship_id left;
		unilateral_relationship_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(unilateral_relationship_id id) { return bool(id); }
	
	//
	// definition of strongly typed index for gp_relationship_id
	//
	class gp_relationship_id {
		public:
		using value_base_t = uint16_t;
		using zero_is_null_t = std::true_type;
		
		uint16_t value = 0;
		
		constexpr gp_relationship_id() noexcept = default;
		explicit constexpr gp_relationship_id(uint16_t v) noexcept : value(v + 1) {}
		constexpr gp_relationship_id(gp_relationship_id const& v) noexcept = default;
		constexpr gp_relationship_id(gp_relationship_id&& v) noexcept = default;
		
		gp_relationship_id& operator=(gp_relationship_id const& v) noexcept = default;
		gp_relationship_id& operator=(gp_relationship_id&& v) noexcept = default;
		constexpr bool operator==(gp_relationship_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(gp_relationship_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint16_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class gp_relationship_id_pair {
		public:
		gp_relationship_id left;
		gp_relationship_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(gp_relationship_id id) { return bool(id); }
	
	//
	// definition of strongly typed index for factory_id
	//
	class factory_id {
		public:
		using value_base_t = uint16_t;
		using zero_is_null_t = std::true_type;
		
		uint16_t value = 0;
		
		constexpr factory_id() noexcept = default;
		explicit constexpr factory_id(uint16_t v) noexcept : value(v + 1) {}
		constexpr factory_id(factory_id const& v) noexcept = default;
		constexpr factory_id(factory_id&& v) noexcept = default;
		
		factory_id& operator=(factory_id const& v) noexcept = default;
		factory_id& operator=(factory_id&& v) noexcept = default;
		constexpr bool operator==(factory_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(factory_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint16_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class factory_id_pair {
		public:
		factory_id left;
		factory_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(factory_id id) { return bool(id); }
	
	//
	// definition of strongly typed index for factory_location_id
	//
	class factory_location_id {
		public:
		using value_base_t = uint16_t;
		using zero_is_null_t = std::true_type;
		
		uint16_t value = 0;
		
		constexpr factory_location_id() noexcept = default;
		explicit constexpr factory_location_id(uint16_t v) noexcept : value(v + 1) {}
		constexpr factory_location_id(factory_location_id const& v) noexcept = default;
		constexpr factory_location_id(factory_location_id&& v) noexcept = default;
		
		factory_location_id& operator=(factory_location_id const& v) noexcept = default;
		factory_location_id& operator=(factory_location_id&& v) noexcept = default;
		constexpr bool operator==(factory_location_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(factory_location_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint16_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class factory_location_id_pair {
		public:
		factory_location_id left;
		factory_location_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(factory_location_id id) { return bool(id); }
	
	//
	// definition of strongly typed index for province_ownership_id
	//
	class province_ownership_id {
		public:
		using value_base_t = uint16_t;
		using zero_is_null_t = std::true_type;
		
		uint16_t value = 0;
		
		constexpr province_ownership_id() noexcept = default;
		explicit constexpr province_ownership_id(uint16_t v) noexcept : value(v + 1) {}
		constexpr province_ownership_id(province_ownership_id const& v) noexcept = default;
		constexpr province_ownership_id(province_ownership_id&& v) noexcept = default;
		
		province_ownership_id& operator=(province_ownership_id const& v) noexcept = default;
		province_ownership_id& operator=(province_ownership_id&& v) noexcept = default;
		constexpr bool operator==(province_ownership_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(province_ownership_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint16_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class province_ownership_id_pair {
		public:
		province_ownership_id left;
		province_ownership_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(province_ownership_id id) { return bool(id); }
	
	//
	// definition of strongly typed index for province_control_id
	//
	class province_control_id {
		public:
		using value_base_t = uint16_t;
		using zero_is_null_t = std::true_type;
		
		uint16_t value = 0;
		
		constexpr province_control_id() noexcept = default;
		explicit constexpr province_control_id(uint16_t v) noexcept : value(v + 1) {}
		constexpr province_control_id(province_control_id const& v) noexcept = default;
		constexpr province_control_id(province_control_id&& v) noexcept = default;
		
		province_control_id& operator=(province_control_id const& v) noexcept = default;
		province_control_id& operator=(province_control_id&& v) noexcept = default;
		constexpr bool operator==(province_control_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(province_control_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint16_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class province_control_id_pair {
		public:
		province_control_id left;
		province_control_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(province_control_id id) { return bool(id); }
	
	//
	// definition of strongly typed index for province_rebel_control_id
	//
	class province_rebel_control_id {
		public:
		using value_base_t = uint16_t;
		using zero_is_null_t = std::true_type;
		
		uint16_t value = 0;
		
		constexpr province_rebel_control_id() noexcept = default;
		explicit constexpr province_rebel_control_id(uint16_t v) noexcept : value(v + 1) {}
		constexpr province_rebel_control_id(province_rebel_control_id const& v) noexcept = default;
		constexpr province_rebel_control_id(province_rebel_control_id&& v) noexcept = default;
		
		province_rebel_control_id& operator=(province_rebel_control_id const& v) noexcept = default;
		province_rebel_control_id& operator=(province_rebel_control_id&& v) noexcept = default;
		constexpr bool operator==(province_rebel_control_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(province_rebel_control_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint16_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class province_rebel_control_id_pair {
		public:
		province_rebel_control_id left;
		province_rebel_control_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(province_rebel_control_id id) { return bool(id); }
	
	//
	// definition of strongly typed index for province_land_construction_id
	//
	class province_land_construction_id {
		public:
		using value_base_t = uint16_t;
		using zero_is_null_t = std::true_type;
		
		uint16_t value = 0;
		
		constexpr province_land_construction_id() noexcept = default;
		explicit constexpr province_land_construction_id(uint16_t v) noexcept : value(v + 1) {}
		constexpr province_land_construction_id(province_land_construction_id const& v) noexcept = default;
		constexpr province_land_construction_id(province_land_construction_id&& v) noexcept = default;
		
		province_land_construction_id& operator=(province_land_construction_id const& v) noexcept = default;
		province_land_construction_id& operator=(province_land_construction_id&& v) noexcept = default;
		constexpr bool operator==(province_land_construction_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(province_land_construction_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint16_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class province_land_construction_id_pair {
		public:
		province_land_construction_id left;
		province_land_construction_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(province_land_construction_id id) { return bool(id); }
	
	//
	// definition of strongly typed index for province_naval_construction_id
	//
	class province_naval_construction_id {
		public:
		using value_base_t = uint16_t;
		using zero_is_null_t = std::true_type;
		
		uint16_t value = 0;
		
		constexpr province_naval_construction_id() noexcept = default;
		explicit constexpr province_naval_construction_id(uint16_t v) noexcept : value(v + 1) {}
		constexpr province_naval_construction_id(province_naval_construction_id const& v) noexcept = default;
		constexpr province_naval_construction_id(province_naval_construction_id&& v) noexcept = default;
		
		province_naval_construction_id& operator=(province_naval_construction_id const& v) noexcept = default;
		province_naval_construction_id& operator=(province_naval_construction_id&& v) noexcept = default;
		constexpr bool operator==(province_naval_construction_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(province_naval_construction_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint16_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class province_naval_construction_id_pair {
		public:
		province_naval_construction_id left;
		province_naval_construction_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(province_naval_construction_id id) { return bool(id); }
	
	//
	// definition of strongly typed index for province_building_construction_id
	//
	class province_building_construction_id {
		public:
		using value_base_t = uint16_t;
		using zero_is_null_t = std::true_type;
		
		uint16_t value = 0;
		
		constexpr province_building_construction_id() noexcept = default;
		explicit constexpr province_building_construction_id(uint16_t v) noexcept : value(v + 1) {}
		constexpr province_building_construction_id(province_building_construction_id const& v) noexcept = default;
		constexpr province_building_construction_id(province_building_construction_id&& v) noexcept = default;
		
		province_building_construction_id& operator=(province_building_construction_id const& v) noexcept = default;
		province_building_construction_id& operator=(province_building_construction_id&& v) noexcept = default;
		constexpr bool operator==(province_building_construction_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(province_building_construction_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint16_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class province_building_construction_id_pair {
		public:
		province_building_construction_id left;
		province_building_construction_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(province_building_construction_id id) { return bool(id); }
	
	//
	// definition of strongly typed index for factory_construction_id
	//
	class factory_construction_id {
		public:
		using value_base_t = uint16_t;
		using zero_is_null_t = std::true_type;
		
		uint16_t value = 0;
		
		constexpr factory_construction_id() noexcept = default;
		explicit constexpr factory_construction_id(uint16_t v) noexcept : value(v + 1) {}
		constexpr factory_construction_id(factory_construction_id const& v) noexcept = default;
		constexpr factory_construction_id(factory_construction_id&& v) noexcept = default;
		
		factory_construction_id& operator=(factory_construction_id const& v) noexcept = default;
		factory_construction_id& operator=(factory_construction_id&& v) noexcept = default;
		constexpr bool operator==(factory_construction_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(factory_construction_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint16_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class factory_construction_id_pair {
		public:
		factory_construction_id left;
		factory_construction_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(factory_construction_id id) { return bool(id); }
	
	//
	// definition of strongly typed index for overlord_id
	//
	class overlord_id {
		public:
		using value_base_t = uint16_t;
		using zero_is_null_t = std::true_type;
		
		uint16_t value = 0;
		
		constexpr overlord_id() noexcept = default;
		explicit constexpr overlord_id(uint16_t v) noexcept : value(v + 1) {}
		constexpr overlord_id(overlord_id const& v) noexcept = default;
		constexpr overlord_id(overlord_id&& v) noexcept = default;
		
		overlord_id& operator=(overlord_id const& v) noexcept = default;
		overlord_id& operator=(overlord_id&& v) noexcept = default;
		constexpr bool operator==(overlord_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(overlord_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint16_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class overlord_id_pair {
		public:
		overlord_id left;
		overlord_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(overlord_id id) { return bool(id); }
	
	//
	// definition of strongly typed index for rebel_faction_id
	//
	class rebel_faction_id {
		public:
		using value_base_t = uint16_t;
		using zero_is_null_t = std::true_type;
		
		uint16_t value = 0;
		
		constexpr rebel_faction_id() noexcept = default;
		explicit constexpr rebel_faction_id(uint16_t v) noexcept : value(v + 1) {}
		constexpr rebel_faction_id(rebel_faction_id const& v) noexcept = default;
		constexpr rebel_faction_id(rebel_faction_id&& v) noexcept = default;
		
		rebel_faction_id& operator=(rebel_faction_id const& v) noexcept = default;
		rebel_faction_id& operator=(rebel_faction_id&& v) noexcept = default;
		constexpr bool operator==(rebel_faction_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(rebel_faction_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint16_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class rebel_faction_id_pair {
		public:
		rebel_faction_id left;
		rebel_faction_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(rebel_faction_id id) { return bool(id); }
	
	//
	// definition of strongly typed index for rebellion_within_id
	//
	class rebellion_within_id {
		public:
		using value_base_t = uint16_t;
		using zero_is_null_t = std::true_type;
		
		uint16_t value = 0;
		
		constexpr rebellion_within_id() noexcept = default;
		explicit constexpr rebellion_within_id(uint16_t v) noexcept : value(v + 1) {}
		constexpr rebellion_within_id(rebellion_within_id const& v) noexcept = default;
		constexpr rebellion_within_id(rebellion_within_id&& v) noexcept = default;
		
		rebellion_within_id& operator=(rebellion_within_id const& v) noexcept = default;
		rebellion_within_id& operator=(rebellion_within_id&& v) noexcept = default;
		constexpr bool operator==(rebellion_within_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(rebellion_within_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint16_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class rebellion_within_id_pair {
		public:
		rebellion_within_id left;
		rebellion_within_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(rebellion_within_id id) { return bool(id); }
	
	//
	// definition of strongly typed index for movement_id
	//
	class movement_id {
		public:
		using value_base_t = uint16_t;
		using zero_is_null_t = std::true_type;
		
		uint16_t value = 0;
		
		constexpr movement_id() noexcept = default;
		explicit constexpr movement_id(uint16_t v) noexcept : value(v + 1) {}
		constexpr movement_id(movement_id const& v) noexcept = default;
		constexpr movement_id(movement_id&& v) noexcept = default;
		
		movement_id& operator=(movement_id const& v) noexcept = default;
		movement_id& operator=(movement_id&& v) noexcept = default;
		constexpr bool operator==(movement_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(movement_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint16_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class movement_id_pair {
		public:
		movement_id left;
		movement_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(movement_id id) { return bool(id); }
	
	//
	// definition of strongly typed index for movement_within_id
	//
	class movement_within_id {
		public:
		using value_base_t = uint16_t;
		using zero_is_null_t = std::true_type;
		
		uint16_t value = 0;
		
		constexpr movement_within_id() noexcept = default;
		explicit constexpr movement_within_id(uint16_t v) noexcept : value(v + 1) {}
		constexpr movement_within_id(movement_within_id const& v) noexcept = default;
		constexpr movement_within_id(movement_within_id&& v) noexcept = default;
		
		movement_within_id& operator=(movement_within_id const& v) noexcept = default;
		movement_within_id& operator=(movement_within_id&& v) noexcept = default;
		constexpr bool operator==(movement_within_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(movement_within_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint16_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class movement_within_id_pair {
		public:
		movement_within_id left;
		movement_within_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(movement_within_id id) { return bool(id); }
	
	//
	// definition of strongly typed index for pop_movement_membership_id
	//
	class pop_movement_membership_id {
		public:
		using value_base_t = uint32_t;
		using zero_is_null_t = std::true_type;
		
		uint32_t value = 0;
		
		constexpr pop_movement_membership_id() noexcept = default;
		explicit constexpr pop_movement_membership_id(uint32_t v) noexcept : value(v + 1) {}
		constexpr pop_movement_membership_id(pop_movement_membership_id const& v) noexcept = default;
		constexpr pop_movement_membership_id(pop_movement_membership_id&& v) noexcept = default;
		
		pop_movement_membership_id& operator=(pop_movement_membership_id const& v) noexcept = default;
		pop_movement_membership_id& operator=(pop_movement_membership_id&& v) noexcept = default;
		constexpr bool operator==(pop_movement_membership_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(pop_movement_membership_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint32_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class pop_movement_membership_id_pair {
		public:
		pop_movement_membership_id left;
		pop_movement_membership_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(pop_movement_membership_id id) { return bool(id); }
	
	//
	// definition of strongly typed index for pop_rebellion_membership_id
	//
	class pop_rebellion_membership_id {
		public:
		using value_base_t = uint32_t;
		using zero_is_null_t = std::true_type;
		
		uint32_t value = 0;
		
		constexpr pop_rebellion_membership_id() noexcept = default;
		explicit constexpr pop_rebellion_membership_id(uint32_t v) noexcept : value(v + 1) {}
		constexpr pop_rebellion_membership_id(pop_rebellion_membership_id const& v) noexcept = default;
		constexpr pop_rebellion_membership_id(pop_rebellion_membership_id&& v) noexcept = default;
		
		pop_rebellion_membership_id& operator=(pop_rebellion_membership_id const& v) noexcept = default;
		pop_rebellion_membership_id& operator=(pop_rebellion_membership_id&& v) noexcept = default;
		constexpr bool operator==(pop_rebellion_membership_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(pop_rebellion_membership_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint32_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class pop_rebellion_membership_id_pair {
		public:
		pop_rebellion_membership_id left;
		pop_rebellion_membership_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(pop_rebellion_membership_id id) { return bool(id); }
	
	//
	// definition of strongly typed index for pop_id
	//
	class pop_id {
		public:
		using value_base_t = uint32_t;
		using zero_is_null_t = std::true_type;
		
		uint32_t value = 0;
		
		constexpr pop_id() noexcept = default;
		explicit constexpr pop_id(uint32_t v) noexcept : value(v + 1) {}
		constexpr pop_id(pop_id const& v) noexcept = default;
		constexpr pop_id(pop_id&& v) noexcept = default;
		
		pop_id& operator=(pop_id const& v) noexcept = default;
		pop_id& operator=(pop_id&& v) noexcept = default;
		constexpr bool operator==(pop_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(pop_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint32_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class pop_id_pair {
		public:
		pop_id left;
		pop_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(pop_id id) { return bool(id); }
	
	//
	// definition of strongly typed index for pop_location_id
	//
	class pop_location_id {
		public:
		using value_base_t = uint32_t;
		using zero_is_null_t = std::true_type;
		
		uint32_t value = 0;
		
		constexpr pop_location_id() noexcept = default;
		explicit constexpr pop_location_id(uint32_t v) noexcept : value(v + 1) {}
		constexpr pop_location_id(pop_location_id const& v) noexcept = default;
		constexpr pop_location_id(pop_location_id&& v) noexcept = default;
		
		pop_location_id& operator=(pop_location_id const& v) noexcept = default;
		pop_location_id& operator=(pop_location_id&& v) noexcept = default;
		constexpr bool operator==(pop_location_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(pop_location_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint32_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class pop_location_id_pair {
		public:
		pop_location_id left;
		pop_location_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(pop_location_id id) { return bool(id); }
	
	//
	// definition of strongly typed index for national_event_id
	//
	class national_event_id {
		public:
		using value_base_t = uint16_t;
		using zero_is_null_t = std::true_type;
		
		uint16_t value = 0;
		
		constexpr national_event_id() noexcept = default;
		explicit constexpr national_event_id(uint16_t v) noexcept : value(v + 1) {}
		constexpr national_event_id(national_event_id const& v) noexcept = default;
		constexpr national_event_id(national_event_id&& v) noexcept = default;
		
		national_event_id& operator=(national_event_id const& v) noexcept = default;
		national_event_id& operator=(national_event_id&& v) noexcept = default;
		constexpr bool operator==(national_event_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(national_event_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint16_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class national_event_id_pair {
		public:
		national_event_id left;
		national_event_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(national_event_id id) { return bool(id); }
	
	//
	// definition of strongly typed index for provincial_event_id
	//
	class provincial_event_id {
		public:
		using value_base_t = uint16_t;
		using zero_is_null_t = std::true_type;
		
		uint16_t value = 0;
		
		constexpr provincial_event_id() noexcept = default;
		explicit constexpr provincial_event_id(uint16_t v) noexcept : value(v + 1) {}
		constexpr provincial_event_id(provincial_event_id const& v) noexcept = default;
		constexpr provincial_event_id(provincial_event_id&& v) noexcept = default;
		
		provincial_event_id& operator=(provincial_event_id const& v) noexcept = default;
		provincial_event_id& operator=(provincial_event_id&& v) noexcept = default;
		constexpr bool operator==(provincial_event_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(provincial_event_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint16_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class provincial_event_id_pair {
		public:
		provincial_event_id left;
		provincial_event_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(provincial_event_id id) { return bool(id); }
	
	//
	// definition of strongly typed index for free_national_event_id
	//
	class free_national_event_id {
		public:
		using value_base_t = uint16_t;
		using zero_is_null_t = std::true_type;
		
		uint16_t value = 0;
		
		constexpr free_national_event_id() noexcept = default;
		explicit constexpr free_national_event_id(uint16_t v) noexcept : value(v + 1) {}
		constexpr free_national_event_id(free_national_event_id const& v) noexcept = default;
		constexpr free_national_event_id(free_national_event_id&& v) noexcept = default;
		
		free_national_event_id& operator=(free_national_event_id const& v) noexcept = default;
		free_national_event_id& operator=(free_national_event_id&& v) noexcept = default;
		constexpr bool operator==(free_national_event_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(free_national_event_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint16_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class free_national_event_id_pair {
		public:
		free_national_event_id left;
		free_national_event_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(free_national_event_id id) { return bool(id); }
	
	//
	// definition of strongly typed index for free_provincial_event_id
	//
	class free_provincial_event_id {
		public:
		using value_base_t = uint16_t;
		using zero_is_null_t = std::true_type;
		
		uint16_t value = 0;
		
		constexpr free_provincial_event_id() noexcept = default;
		explicit constexpr free_provincial_event_id(uint16_t v) noexcept : value(v + 1) {}
		constexpr free_provincial_event_id(free_provincial_event_id const& v) noexcept = default;
		constexpr free_provincial_event_id(free_provincial_event_id&& v) noexcept = default;
		
		free_provincial_event_id& operator=(free_provincial_event_id const& v) noexcept = default;
		free_provincial_event_id& operator=(free_provincial_event_id&& v) noexcept = default;
		constexpr bool operator==(free_provincial_event_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(free_provincial_event_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint16_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class free_provincial_event_id_pair {
		public:
		free_provincial_event_id left;
		free_provincial_event_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(free_provincial_event_id id) { return bool(id); }
	
	//
	// definition of strongly typed index for national_focus_id
	//
	class national_focus_id {
		public:
		using value_base_t = uint8_t;
		using zero_is_null_t = std::true_type;
		
		uint8_t value = 0;
		
		constexpr national_focus_id() noexcept = default;
		explicit constexpr national_focus_id(uint8_t v) noexcept : value(v + 1) {}
		constexpr national_focus_id(national_focus_id const& v) noexcept = default;
		constexpr national_focus_id(national_focus_id&& v) noexcept = default;
		
		national_focus_id& operator=(national_focus_id const& v) noexcept = default;
		national_focus_id& operator=(national_focus_id&& v) noexcept = default;
		constexpr bool operator==(national_focus_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(national_focus_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint8_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class national_focus_id_pair {
		public:
		national_focus_id left;
		national_focus_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(national_focus_id id) { return bool(id); }
	
	//
	// definition of strongly typed index for stored_trigger_id
	//
	class stored_trigger_id {
		public:
		using value_base_t = uint16_t;
		using zero_is_null_t = std::true_type;
		
		uint16_t value = 0;
		
		constexpr stored_trigger_id() noexcept = default;
		explicit constexpr stored_trigger_id(uint16_t v) noexcept : value(v + 1) {}
		constexpr stored_trigger_id(stored_trigger_id const& v) noexcept = default;
		constexpr stored_trigger_id(stored_trigger_id&& v) noexcept = default;
		
		stored_trigger_id& operator=(stored_trigger_id const& v) noexcept = default;
		stored_trigger_id& operator=(stored_trigger_id&& v) noexcept = default;
		constexpr bool operator==(stored_trigger_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(stored_trigger_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint16_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class stored_trigger_id_pair {
		public:
		stored_trigger_id left;
		stored_trigger_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(stored_trigger_id id) { return bool(id); }
	
	//
	// definition of strongly typed index for gamerule_id
	//
	class gamerule_id {
		public:
		using value_base_t = uint8_t;
		using zero_is_null_t = std::true_type;
		
		uint8_t value = 0;
		
		constexpr gamerule_id() noexcept = default;
		explicit constexpr gamerule_id(uint8_t v) noexcept : value(v + 1) {}
		constexpr gamerule_id(gamerule_id const& v) noexcept = default;
		constexpr gamerule_id(gamerule_id&& v) noexcept = default;
		
		gamerule_id& operator=(gamerule_id const& v) noexcept = default;
		gamerule_id& operator=(gamerule_id&& v) noexcept = default;
		constexpr bool operator==(gamerule_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(gamerule_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint8_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class gamerule_id_pair {
		public:
		gamerule_id left;
		gamerule_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(gamerule_id id) { return bool(id); }
	
	//
	// definition of strongly typed index for decision_id
	//
	class decision_id {
		public:
		using value_base_t = uint16_t;
		using zero_is_null_t = std::true_type;
		
		uint16_t value = 0;
		
		constexpr decision_id() noexcept = default;
		explicit constexpr decision_id(uint16_t v) noexcept : value(v + 1) {}
		constexpr decision_id(decision_id const& v) noexcept = default;
		constexpr decision_id(decision_id&& v) noexcept = default;
		
		decision_id& operator=(decision_id const& v) noexcept = default;
		decision_id& operator=(decision_id&& v) noexcept = default;
		constexpr bool operator==(decision_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(decision_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint16_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class decision_id_pair {
		public:
		decision_id left;
		decision_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(decision_id id) { return bool(id); }
	
	//
	// definition of strongly typed index for locale_id
	//
	class locale_id {
		public:
		using value_base_t = uint16_t;
		using zero_is_null_t = std::true_type;
		
		uint16_t value = 0;
		
		constexpr locale_id() noexcept = default;
		explicit constexpr locale_id(uint16_t v) noexcept : value(v + 1) {}
		constexpr locale_id(locale_id const& v) noexcept = default;
		constexpr locale_id(locale_id&& v) noexcept = default;
		
		locale_id& operator=(locale_id const& v) noexcept = default;
		locale_id& operator=(locale_id&& v) noexcept = default;
		constexpr bool operator==(locale_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(locale_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint16_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class locale_id_pair {
		public:
		locale_id left;
		locale_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(locale_id id) { return bool(id); }
	
	//
	// definition of strongly typed index for mp_player_id
	//
	class mp_player_id {
		public:
		using value_base_t = uint16_t;
		using zero_is_null_t = std::true_type;
		
		uint16_t value = 0;
		
		constexpr mp_player_id() noexcept = default;
		explicit constexpr mp_player_id(uint16_t v) noexcept : value(v + 1) {}
		constexpr mp_player_id(mp_player_id const& v) noexcept = default;
		constexpr mp_player_id(mp_player_id&& v) noexcept = default;
		
		mp_player_id& operator=(mp_player_id const& v) noexcept = default;
		mp_player_id& operator=(mp_player_id&& v) noexcept = default;
		constexpr bool operator==(mp_player_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(mp_player_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint16_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class mp_player_id_pair {
		public:
		mp_player_id left;
		mp_player_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(mp_player_id id) { return bool(id); }
	
	//
	// definition of strongly typed index for player_nation_id
	//
	class player_nation_id {
		public:
		using value_base_t = uint16_t;
		using zero_is_null_t = std::true_type;
		
		uint16_t value = 0;
		
		constexpr player_nation_id() noexcept = default;
		explicit constexpr player_nation_id(uint16_t v) noexcept : value(v + 1) {}
		constexpr player_nation_id(player_nation_id const& v) noexcept = default;
		constexpr player_nation_id(player_nation_id&& v) noexcept = default;
		
		player_nation_id& operator=(player_nation_id const& v) noexcept = default;
		player_nation_id& operator=(player_nation_id&& v) noexcept = default;
		constexpr bool operator==(player_nation_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(player_nation_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint16_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class player_nation_id_pair {
		public:
		player_nation_id left;
		player_nation_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(player_nation_id id) { return bool(id); }
	
	//
	// definition of strongly typed index for scripted_interaction_id
	//
	class scripted_interaction_id {
		public:
		using value_base_t = uint16_t;
		using zero_is_null_t = std::true_type;
		
		uint16_t value = 0;
		
		constexpr scripted_interaction_id() noexcept = default;
		explicit constexpr scripted_interaction_id(uint16_t v) noexcept : value(v + 1) {}
		constexpr scripted_interaction_id(scripted_interaction_id const& v) noexcept = default;
		constexpr scripted_interaction_id(scripted_interaction_id&& v) noexcept = default;
		
		scripted_interaction_id& operator=(scripted_interaction_id const& v) noexcept = default;
		scripted_interaction_id& operator=(scripted_interaction_id&& v) noexcept = default;
		constexpr bool operator==(scripted_interaction_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(scripted_interaction_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint16_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class scripted_interaction_id_pair {
		public:
		scripted_interaction_id left;
		scripted_interaction_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(scripted_interaction_id id) { return bool(id); }
	
	//
	// definition of strongly typed index for ui_variable_id
	//
	class ui_variable_id {
		public:
		using value_base_t = uint16_t;
		using zero_is_null_t = std::true_type;
		
		uint16_t value = 0;
		
		constexpr ui_variable_id() noexcept = default;
		explicit constexpr ui_variable_id(uint16_t v) noexcept : value(v + 1) {}
		constexpr ui_variable_id(ui_variable_id const& v) noexcept = default;
		constexpr ui_variable_id(ui_variable_id&& v) noexcept = default;
		
		ui_variable_id& operator=(ui_variable_id const& v) noexcept = default;
		ui_variable_id& operator=(ui_variable_id&& v) noexcept = default;
		constexpr bool operator==(ui_variable_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(ui_variable_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint16_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class ui_variable_id_pair {
		public:
		ui_variable_id left;
		ui_variable_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(ui_variable_id id) { return bool(id); }
	
	//
	// definition of strongly typed index for text_key
	//
	class text_key {
		public:
		using value_base_t = uint32_t;
		using zero_is_null_t = std::true_type;
		
		uint32_t value = 0;
		
		constexpr text_key() noexcept = default;
		explicit constexpr text_key(uint32_t v) noexcept : value(v + 1) {}
		constexpr text_key(text_key const& v) noexcept = default;
		constexpr text_key(text_key&& v) noexcept = default;
		
		text_key& operator=(text_key const& v) noexcept = default;
		text_key& operator=(text_key&& v) noexcept = default;
		constexpr bool operator==(text_key v) const noexcept { return value == v.value; }
		constexpr bool operator!=(text_key v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint32_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class text_key_pair {
		public:
		text_key left;
		text_key right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(text_key id) { return bool(id); }
	
	//
	// definition of strongly typed index for texture_id
	//
	class texture_id {
		public:
		using value_base_t = uint16_t;
		using zero_is_null_t = std::true_type;
		
		uint16_t value = 0;
		
		constexpr texture_id() noexcept = default;
		explicit constexpr texture_id(uint16_t v) noexcept : value(v + 1) {}
		constexpr texture_id(texture_id const& v) noexcept = default;
		constexpr texture_id(texture_id&& v) noexcept = default;
		
		texture_id& operator=(texture_id const& v) noexcept = default;
		texture_id& operator=(texture_id&& v) noexcept = default;
		constexpr bool operator==(texture_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(texture_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint16_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class texture_id_pair {
		public:
		texture_id left;
		texture_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(texture_id id) { return bool(id); }
	
	//
	// definition of strongly typed index for gfx_object_id
	//
	class gfx_object_id {
		public:
		using value_base_t = uint16_t;
		using zero_is_null_t = std::true_type;
		
		uint16_t value = 0;
		
		constexpr gfx_object_id() noexcept = default;
		explicit constexpr gfx_object_id(uint16_t v) noexcept : value(v + 1) {}
		constexpr gfx_object_id(gfx_object_id const& v) noexcept = default;
		constexpr gfx_object_id(gfx_object_id&& v) noexcept = default;
		
		gfx_object_id& operator=(gfx_object_id const& v) noexcept = default;
		gfx_object_id& operator=(gfx_object_id&& v) noexcept = default;
		constexpr bool operator==(gfx_object_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(gfx_object_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint16_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class gfx_object_id_pair {
		public:
		gfx_object_id left;
		gfx_object_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(gfx_object_id id) { return bool(id); }
	
	//
	// definition of strongly typed index for gui_def_id
	//
	class gui_def_id {
		public:
		using value_base_t = uint16_t;
		using zero_is_null_t = std::true_type;
		
		uint16_t value = 0;
		
		constexpr gui_def_id() noexcept = default;
		explicit constexpr gui_def_id(uint16_t v) noexcept : value(v + 1) {}
		constexpr gui_def_id(gui_def_id const& v) noexcept = default;
		constexpr gui_def_id(gui_def_id&& v) noexcept = default;
		
		gui_def_id& operator=(gui_def_id const& v) noexcept = default;
		gui_def_id& operator=(gui_def_id&& v) noexcept = default;
		constexpr bool operator==(gui_def_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(gui_def_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint16_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class gui_def_id_pair {
		public:
		gui_def_id left;
		gui_def_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(gui_def_id id) { return bool(id); }
	
	//
	// definition of strongly typed index for crime_id
	//
	class crime_id {
		public:
		using value_base_t = uint8_t;
		using zero_is_null_t = std::true_type;
		
		uint8_t value = 0;
		
		constexpr crime_id() noexcept = default;
		explicit constexpr crime_id(uint8_t v) noexcept : value(v + 1) {}
		constexpr crime_id(crime_id const& v) noexcept = default;
		constexpr crime_id(crime_id&& v) noexcept = default;
		
		crime_id& operator=(crime_id const& v) noexcept = default;
		crime_id& operator=(crime_id&& v) noexcept = default;
		constexpr bool operator==(crime_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(crime_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint8_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class crime_id_pair {
		public:
		crime_id left;
		crime_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(crime_id id) { return bool(id); }
	
	//
	// definition of strongly typed index for unit_type_id
	//
	class unit_type_id {
		public:
		using value_base_t = uint8_t;
		using zero_is_null_t = std::true_type;
		
		uint8_t value = 0;
		
		constexpr unit_type_id() noexcept = default;
		explicit constexpr unit_type_id(uint8_t v) noexcept : value(v + 1) {}
		constexpr unit_type_id(unit_type_id const& v) noexcept = default;
		constexpr unit_type_id(unit_type_id&& v) noexcept = default;
		
		unit_type_id& operator=(unit_type_id const& v) noexcept = default;
		unit_type_id& operator=(unit_type_id&& v) noexcept = default;
		constexpr bool operator==(unit_type_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(unit_type_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint8_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class unit_type_id_pair {
		public:
		unit_type_id left;
		unit_type_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(unit_type_id id) { return bool(id); }
	
	//
	// definition of strongly typed index for unit_name_id
	//
	class unit_name_id {
		public:
		using value_base_t = uint32_t;
		using zero_is_null_t = std::true_type;
		
		uint32_t value = 0;
		
		constexpr unit_name_id() noexcept = default;
		explicit constexpr unit_name_id(uint32_t v) noexcept : value(v + 1) {}
		constexpr unit_name_id(unit_name_id const& v) noexcept = default;
		constexpr unit_name_id(unit_name_id&& v) noexcept = default;
		
		unit_name_id& operator=(unit_name_id const& v) noexcept = default;
		unit_name_id& operator=(unit_name_id&& v) noexcept = default;
		constexpr bool operator==(unit_name_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(unit_name_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint32_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class unit_name_id_pair {
		public:
		unit_name_id left;
		unit_name_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(unit_name_id id) { return bool(id); }
	
	//
	// definition of strongly typed index for national_variable_id
	//
	class national_variable_id {
		public:
		using value_base_t = uint16_t;
		using zero_is_null_t = std::true_type;
		
		uint16_t value = 0;
		
		constexpr national_variable_id() noexcept = default;
		explicit constexpr national_variable_id(uint16_t v) noexcept : value(v + 1) {}
		constexpr national_variable_id(national_variable_id const& v) noexcept = default;
		constexpr national_variable_id(national_variable_id&& v) noexcept = default;
		
		national_variable_id& operator=(national_variable_id const& v) noexcept = default;
		national_variable_id& operator=(national_variable_id&& v) noexcept = default;
		constexpr bool operator==(national_variable_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(national_variable_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint16_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class national_variable_id_pair {
		public:
		national_variable_id left;
		national_variable_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(national_variable_id id) { return bool(id); }
	
	//
	// definition of strongly typed index for national_flag_id
	//
	class national_flag_id {
		public:
		using value_base_t = uint16_t;
		using zero_is_null_t = std::true_type;
		
		uint16_t value = 0;
		
		constexpr national_flag_id() noexcept = default;
		explicit constexpr national_flag_id(uint16_t v) noexcept : value(v + 1) {}
		constexpr national_flag_id(national_flag_id const& v) noexcept = default;
		constexpr national_flag_id(national_flag_id&& v) noexcept = default;
		
		national_flag_id& operator=(national_flag_id const& v) noexcept = default;
		national_flag_id& operator=(national_flag_id&& v) noexcept = default;
		constexpr bool operator==(national_flag_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(national_flag_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint16_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class national_flag_id_pair {
		public:
		national_flag_id left;
		national_flag_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(national_flag_id id) { return bool(id); }
	
	//
	// definition of strongly typed index for global_flag_id
	//
	class global_flag_id {
		public:
		using value_base_t = uint16_t;
		using zero_is_null_t = std::true_type;
		
		uint16_t value = 0;
		
		constexpr global_flag_id() noexcept = default;
		explicit constexpr global_flag_id(uint16_t v) noexcept : value(v + 1) {}
		constexpr global_flag_id(global_flag_id const& v) noexcept = default;
		constexpr global_flag_id(global_flag_id&& v) noexcept = default;
		
		global_flag_id& operator=(global_flag_id const& v) noexcept = default;
		global_flag_id& operator=(global_flag_id&& v) noexcept = default;
		constexpr bool operator==(global_flag_id v) const noexcept { return value == v.value; }
		constexpr bool operator!=(global_flag_id v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint16_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class global_flag_id_pair {
		public:
		global_flag_id left;
		global_flag_id right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(global_flag_id id) { return bool(id); }
	
	//
	// definition of strongly typed index for trigger_key
	//
	class trigger_key {
		public:
		using value_base_t = uint16_t;
		using zero_is_null_t = std::true_type;
		
		uint16_t value = 0;
		
		constexpr trigger_key() noexcept = default;
		explicit constexpr trigger_key(uint16_t v) noexcept : value(v + 1) {}
		constexpr trigger_key(trigger_key const& v) noexcept = default;
		constexpr trigger_key(trigger_key&& v) noexcept = default;
		
		trigger_key& operator=(trigger_key const& v) noexcept = default;
		trigger_key& operator=(trigger_key&& v) noexcept = default;
		constexpr bool operator==(trigger_key v) const noexcept { return value == v.value; }
		constexpr bool operator!=(trigger_key v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint16_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class trigger_key_pair {
		public:
		trigger_key left;
		trigger_key right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(trigger_key id) { return bool(id); }
	
	//
	// definition of strongly typed index for effect_key
	//
	class effect_key {
		public:
		using value_base_t = uint16_t;
		using zero_is_null_t = std::true_type;
		
		uint16_t value = 0;
		
		constexpr effect_key() noexcept = default;
		explicit constexpr effect_key(uint16_t v) noexcept : value(v + 1) {}
		constexpr effect_key(effect_key const& v) noexcept = default;
		constexpr effect_key(effect_key&& v) noexcept = default;
		
		effect_key& operator=(effect_key const& v) noexcept = default;
		effect_key& operator=(effect_key&& v) noexcept = default;
		constexpr bool operator==(effect_key v) const noexcept { return value == v.value; }
		constexpr bool operator!=(effect_key v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint16_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class effect_key_pair {
		public:
		effect_key left;
		effect_key right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(effect_key id) { return bool(id); }
	
	//
	// definition of strongly typed index for value_modifier_key
	//
	class value_modifier_key {
		public:
		using value_base_t = uint16_t;
		using zero_is_null_t = std::true_type;
		
		uint16_t value = 0;
		
		constexpr value_modifier_key() noexcept = default;
		explicit constexpr value_modifier_key(uint16_t v) noexcept : value(v + 1) {}
		constexpr value_modifier_key(value_modifier_key const& v) noexcept = default;
		constexpr value_modifier_key(value_modifier_key&& v) noexcept = default;
		
		value_modifier_key& operator=(value_modifier_key const& v) noexcept = default;
		value_modifier_key& operator=(value_modifier_key&& v) noexcept = default;
		constexpr bool operator==(value_modifier_key v) const noexcept { return value == v.value; }
		constexpr bool operator!=(value_modifier_key v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint16_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class value_modifier_key_pair {
		public:
		value_modifier_key left;
		value_modifier_key right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(value_modifier_key id) { return bool(id); }
	
	//
	// definition of strongly typed index for pop_demographics_key
	//
	class pop_demographics_key {
		public:
		using value_base_t = uint16_t;
		using zero_is_null_t = std::true_type;
		
		uint16_t value = 0;
		
		constexpr pop_demographics_key() noexcept = default;
		explicit constexpr pop_demographics_key(uint16_t v) noexcept : value(v + 1) {}
		constexpr pop_demographics_key(pop_demographics_key const& v) noexcept = default;
		constexpr pop_demographics_key(pop_demographics_key&& v) noexcept = default;
		
		pop_demographics_key& operator=(pop_demographics_key const& v) noexcept = default;
		pop_demographics_key& operator=(pop_demographics_key&& v) noexcept = default;
		constexpr bool operator==(pop_demographics_key v) const noexcept { return value == v.value; }
		constexpr bool operator!=(pop_demographics_key v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint16_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class pop_demographics_key_pair {
		public:
		pop_demographics_key left;
		pop_demographics_key right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(pop_demographics_key id) { return bool(id); }
	
	//
	// definition of strongly typed index for demographics_key
	//
	class demographics_key {
		public:
		using value_base_t = uint16_t;
		using zero_is_null_t = std::true_type;
		
		uint16_t value = 0;
		
		constexpr demographics_key() noexcept = default;
		explicit constexpr demographics_key(uint16_t v) noexcept : value(v + 1) {}
		constexpr demographics_key(demographics_key const& v) noexcept = default;
		constexpr demographics_key(demographics_key&& v) noexcept = default;
		
		demographics_key& operator=(demographics_key const& v) noexcept = default;
		demographics_key& operator=(demographics_key&& v) noexcept = default;
		constexpr bool operator==(demographics_key v) const noexcept { return value == v.value; }
		constexpr bool operator!=(demographics_key v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint16_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class demographics_key_pair {
		public:
		demographics_key left;
		demographics_key right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(demographics_key id) { return bool(id); }
	
	//
	// definition of strongly typed index for provincial_modifier_value
	//
	class provincial_modifier_value {
		public:
		using value_base_t = uint8_t;
		using zero_is_null_t = std::true_type;
		
		uint8_t value = 0;
		
		constexpr provincial_modifier_value() noexcept = default;
		explicit constexpr provincial_modifier_value(uint8_t v) noexcept : value(v + 1) {}
		constexpr provincial_modifier_value(provincial_modifier_value const& v) noexcept = default;
		constexpr provincial_modifier_value(provincial_modifier_value&& v) noexcept = default;
		
		provincial_modifier_value& operator=(provincial_modifier_value const& v) noexcept = default;
		provincial_modifier_value& operator=(provincial_modifier_value&& v) noexcept = default;
		constexpr bool operator==(provincial_modifier_value v) const noexcept { return value == v.value; }
		constexpr bool operator!=(provincial_modifier_value v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint8_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class provincial_modifier_value_pair {
		public:
		provincial_modifier_value left;
		provincial_modifier_value right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(provincial_modifier_value id) { return bool(id); }
	
	//
	// definition of strongly typed index for national_modifier_value
	//
	class national_modifier_value {
		public:
		using value_base_t = uint8_t;
		using zero_is_null_t = std::true_type;
		
		uint8_t value = 0;
		
		constexpr national_modifier_value() noexcept = default;
		explicit constexpr national_modifier_value(uint8_t v) noexcept : value(v + 1) {}
		constexpr national_modifier_value(national_modifier_value const& v) noexcept = default;
		constexpr national_modifier_value(national_modifier_value&& v) noexcept = default;
		
		national_modifier_value& operator=(national_modifier_value const& v) noexcept = default;
		national_modifier_value& operator=(national_modifier_value&& v) noexcept = default;
		constexpr bool operator==(national_modifier_value v) const noexcept { return value == v.value; }
		constexpr bool operator!=(national_modifier_value v) const noexcept { return value != v.value; }
		explicit constexpr operator bool() const noexcept { return value != uint8_t(0); }
		constexpr DCON_RELEASE_INLINE int32_t index() const noexcept {
			return int32_t(value) - 1;
		}
	};
	
	class national_modifier_value_pair {
		public:
		national_modifier_value left;
		national_modifier_value right;
	};
	
	DCON_RELEASE_INLINE bool is_valid_index(national_modifier_value id) { return bool(id); }
	
}

#ifndef DCON_NO_VE
namespace ve {
	template<>
	struct value_to_vector_type_s<dcon::government_flag_id> {
		using type = ::ve::tagged_vector<dcon::government_flag_id>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::national_identity_id> {
		using type = ::ve::tagged_vector<dcon::national_identity_id>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::political_party_id> {
		using type = ::ve::tagged_vector<dcon::political_party_id>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::leader_images_id> {
		using type = ::ve::tagged_vector<dcon::leader_images_id>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::religion_id> {
		using type = ::ve::tagged_vector<dcon::religion_id>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::culture_group_id> {
		using type = ::ve::tagged_vector<dcon::culture_group_id>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::culture_id> {
		using type = ::ve::tagged_vector<dcon::culture_id>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::culture_group_membership_id> {
		using type = ::ve::tagged_vector<dcon::culture_group_membership_id>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::cultural_union_of_id> {
		using type = ::ve::tagged_vector<dcon::cultural_union_of_id>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::commodity_id> {
		using type = ::ve::tagged_vector<dcon::commodity_id>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::modifier_id> {
		using type = ::ve::tagged_vector<dcon::modifier_id>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::factory_type_id> {
		using type = ::ve::tagged_vector<dcon::factory_type_id>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::ideology_group_id> {
		using type = ::ve::tagged_vector<dcon::ideology_group_id>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::ideology_id> {
		using type = ::ve::tagged_vector<dcon::ideology_id>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::ideology_group_membership_id> {
		using type = ::ve::tagged_vector<dcon::ideology_group_membership_id>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::issue_id> {
		using type = ::ve::tagged_vector<dcon::issue_id>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::issue_option_id> {
		using type = ::ve::tagged_vector<dcon::issue_option_id>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::reform_id> {
		using type = ::ve::tagged_vector<dcon::reform_id>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::reform_option_id> {
		using type = ::ve::tagged_vector<dcon::reform_option_id>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::cb_type_id> {
		using type = ::ve::tagged_vector<dcon::cb_type_id>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::leader_trait_id> {
		using type = ::ve::tagged_vector<dcon::leader_trait_id>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::pop_type_id> {
		using type = ::ve::tagged_vector<dcon::pop_type_id>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::rebel_type_id> {
		using type = ::ve::tagged_vector<dcon::rebel_type_id>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::government_type_id> {
		using type = ::ve::tagged_vector<dcon::government_type_id>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::province_id> {
		using type = ::ve::tagged_vector<dcon::province_id>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::province_adjacency_id> {
		using type = ::ve::tagged_vector<dcon::province_adjacency_id>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::nation_adjacency_id> {
		using type = ::ve::tagged_vector<dcon::nation_adjacency_id>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::regiment_id> {
		using type = ::ve::tagged_vector<dcon::regiment_id>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::automated_army_group_id> {
		using type = ::ve::tagged_vector<dcon::automated_army_group_id>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::automated_army_group_membership_regiment_id> {
		using type = ::ve::tagged_vector<dcon::automated_army_group_membership_regiment_id>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::automated_army_group_membership_navy_id> {
		using type = ::ve::tagged_vector<dcon::automated_army_group_membership_navy_id>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::regiment_automation_data_id> {
		using type = ::ve::tagged_vector<dcon::regiment_automation_data_id>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::automation_id> {
		using type = ::ve::tagged_vector<dcon::automation_id>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::ship_id> {
		using type = ::ve::tagged_vector<dcon::ship_id>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::army_id> {
		using type = ::ve::tagged_vector<dcon::army_id>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::army_pursuit_id> {
		using type = ::ve::tagged_vector<dcon::army_pursuit_id>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::navy_id> {
		using type = ::ve::tagged_vector<dcon::navy_id>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::army_transport_id> {
		using type = ::ve::tagged_vector<dcon::army_transport_id>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::army_control_id> {
		using type = ::ve::tagged_vector<dcon::army_control_id>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::army_rebel_control_id> {
		using type = ::ve::tagged_vector<dcon::army_rebel_control_id>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::army_location_id> {
		using type = ::ve::tagged_vector<dcon::army_location_id>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::army_membership_id> {
		using type = ::ve::tagged_vector<dcon::army_membership_id>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::regiment_source_id> {
		using type = ::ve::tagged_vector<dcon::regiment_source_id>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::navy_control_id> {
		using type = ::ve::tagged_vector<dcon::navy_control_id>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::navy_location_id> {
		using type = ::ve::tagged_vector<dcon::navy_location_id>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::navy_membership_id> {
		using type = ::ve::tagged_vector<dcon::navy_membership_id>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::naval_battle_id> {
		using type = ::ve::tagged_vector<dcon::naval_battle_id>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::naval_battle_location_id> {
		using type = ::ve::tagged_vector<dcon::naval_battle_location_id>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::naval_battle_in_war_id> {
		using type = ::ve::tagged_vector<dcon::naval_battle_in_war_id>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::navy_battle_participation_id> {
		using type = ::ve::tagged_vector<dcon::navy_battle_participation_id>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::attacking_admiral_id> {
		using type = ::ve::tagged_vector<dcon::attacking_admiral_id>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::defending_admiral_id> {
		using type = ::ve::tagged_vector<dcon::defending_admiral_id>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::land_battle_id> {
		using type = ::ve::tagged_vector<dcon::land_battle_id>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::land_battle_location_id> {
		using type = ::ve::tagged_vector<dcon::land_battle_location_id>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::land_battle_in_war_id> {
		using type = ::ve::tagged_vector<dcon::land_battle_in_war_id>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::army_battle_participation_id> {
		using type = ::ve::tagged_vector<dcon::army_battle_participation_id>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::attacking_general_id> {
		using type = ::ve::tagged_vector<dcon::attacking_general_id>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::defending_general_id> {
		using type = ::ve::tagged_vector<dcon::defending_general_id>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::leader_id> {
		using type = ::ve::tagged_vector<dcon::leader_id>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::army_leadership_id> {
		using type = ::ve::tagged_vector<dcon::army_leadership_id>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::navy_leadership_id> {
		using type = ::ve::tagged_vector<dcon::navy_leadership_id>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::leader_loyalty_id> {
		using type = ::ve::tagged_vector<dcon::leader_loyalty_id>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::war_id> {
		using type = ::ve::tagged_vector<dcon::war_id>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::peace_offer_id> {
		using type = ::ve::tagged_vector<dcon::peace_offer_id>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::pending_peace_offer_id> {
		using type = ::ve::tagged_vector<dcon::pending_peace_offer_id>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::war_settlement_id> {
		using type = ::ve::tagged_vector<dcon::war_settlement_id>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::wargoal_id> {
		using type = ::ve::tagged_vector<dcon::wargoal_id>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::war_participant_id> {
		using type = ::ve::tagged_vector<dcon::war_participant_id>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::wargoals_attached_id> {
		using type = ::ve::tagged_vector<dcon::wargoals_attached_id>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::peace_offer_item_id> {
		using type = ::ve::tagged_vector<dcon::peace_offer_item_id>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::state_definition_id> {
		using type = ::ve::tagged_vector<dcon::state_definition_id>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::administration_id> {
		using type = ::ve::tagged_vector<dcon::administration_id>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::nation_administration_id> {
		using type = ::ve::tagged_vector<dcon::nation_administration_id>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::state_instance_id> {
		using type = ::ve::tagged_vector<dcon::state_instance_id>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::colonization_id> {
		using type = ::ve::tagged_vector<dcon::colonization_id>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::state_ownership_id> {
		using type = ::ve::tagged_vector<dcon::state_ownership_id>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::flashpoint_focus_id> {
		using type = ::ve::tagged_vector<dcon::flashpoint_focus_id>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::abstract_state_membership_id> {
		using type = ::ve::tagged_vector<dcon::abstract_state_membership_id>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::region_id> {
		using type = ::ve::tagged_vector<dcon::region_id>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::region_membership_id> {
		using type = ::ve::tagged_vector<dcon::region_membership_id>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::core_id> {
		using type = ::ve::tagged_vector<dcon::core_id>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::identity_holder_id> {
		using type = ::ve::tagged_vector<dcon::identity_holder_id>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::technology_id> {
		using type = ::ve::tagged_vector<dcon::technology_id>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::invention_id> {
		using type = ::ve::tagged_vector<dcon::invention_id>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::nation_id> {
		using type = ::ve::tagged_vector<dcon::nation_id>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::market_id> {
		using type = ::ve::tagged_vector<dcon::market_id>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::local_market_id> {
		using type = ::ve::tagged_vector<dcon::local_market_id>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::trade_route_id> {
		using type = ::ve::tagged_vector<dcon::trade_route_id>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::diplomatic_relation_id> {
		using type = ::ve::tagged_vector<dcon::diplomatic_relation_id>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::unilateral_relationship_id> {
		using type = ::ve::tagged_vector<dcon::unilateral_relationship_id>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::gp_relationship_id> {
		using type = ::ve::tagged_vector<dcon::gp_relationship_id>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::factory_id> {
		using type = ::ve::tagged_vector<dcon::factory_id>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::factory_location_id> {
		using type = ::ve::tagged_vector<dcon::factory_location_id>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::province_ownership_id> {
		using type = ::ve::tagged_vector<dcon::province_ownership_id>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::province_control_id> {
		using type = ::ve::tagged_vector<dcon::province_control_id>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::province_rebel_control_id> {
		using type = ::ve::tagged_vector<dcon::province_rebel_control_id>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::province_land_construction_id> {
		using type = ::ve::tagged_vector<dcon::province_land_construction_id>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::province_naval_construction_id> {
		using type = ::ve::tagged_vector<dcon::province_naval_construction_id>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::province_building_construction_id> {
		using type = ::ve::tagged_vector<dcon::province_building_construction_id>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::factory_construction_id> {
		using type = ::ve::tagged_vector<dcon::factory_construction_id>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::overlord_id> {
		using type = ::ve::tagged_vector<dcon::overlord_id>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::rebel_faction_id> {
		using type = ::ve::tagged_vector<dcon::rebel_faction_id>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::rebellion_within_id> {
		using type = ::ve::tagged_vector<dcon::rebellion_within_id>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::movement_id> {
		using type = ::ve::tagged_vector<dcon::movement_id>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::movement_within_id> {
		using type = ::ve::tagged_vector<dcon::movement_within_id>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::pop_movement_membership_id> {
		using type = ::ve::tagged_vector<dcon::pop_movement_membership_id>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::pop_rebellion_membership_id> {
		using type = ::ve::tagged_vector<dcon::pop_rebellion_membership_id>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::pop_id> {
		using type = ::ve::tagged_vector<dcon::pop_id>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::pop_location_id> {
		using type = ::ve::tagged_vector<dcon::pop_location_id>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::national_event_id> {
		using type = ::ve::tagged_vector<dcon::national_event_id>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::provincial_event_id> {
		using type = ::ve::tagged_vector<dcon::provincial_event_id>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::free_national_event_id> {
		using type = ::ve::tagged_vector<dcon::free_national_event_id>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::free_provincial_event_id> {
		using type = ::ve::tagged_vector<dcon::free_provincial_event_id>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::national_focus_id> {
		using type = ::ve::tagged_vector<dcon::national_focus_id>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::stored_trigger_id> {
		using type = ::ve::tagged_vector<dcon::stored_trigger_id>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::gamerule_id> {
		using type = ::ve::tagged_vector<dcon::gamerule_id>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::decision_id> {
		using type = ::ve::tagged_vector<dcon::decision_id>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::locale_id> {
		using type = ::ve::tagged_vector<dcon::locale_id>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::mp_player_id> {
		using type = ::ve::tagged_vector<dcon::mp_player_id>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::player_nation_id> {
		using type = ::ve::tagged_vector<dcon::player_nation_id>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::scripted_interaction_id> {
		using type = ::ve::tagged_vector<dcon::scripted_interaction_id>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::ui_variable_id> {
		using type = ::ve::tagged_vector<dcon::ui_variable_id>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::text_key> {
		using type = ::ve::tagged_vector<dcon::text_key>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::texture_id> {
		using type = ::ve::tagged_vector<dcon::texture_id>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::gfx_object_id> {
		using type = ::ve::tagged_vector<dcon::gfx_object_id>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::gui_def_id> {
		using type = ::ve::tagged_vector<dcon::gui_def_id>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::crime_id> {
		using type = ::ve::tagged_vector<dcon::crime_id>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::unit_type_id> {
		using type = ::ve::tagged_vector<dcon::unit_type_id>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::unit_name_id> {
		using type = ::ve::tagged_vector<dcon::unit_name_id>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::national_variable_id> {
		using type = ::ve::tagged_vector<dcon::national_variable_id>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::national_flag_id> {
		using type = ::ve::tagged_vector<dcon::national_flag_id>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::global_flag_id> {
		using type = ::ve::tagged_vector<dcon::global_flag_id>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::trigger_key> {
		using type = ::ve::tagged_vector<dcon::trigger_key>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::effect_key> {
		using type = ::ve::tagged_vector<dcon::effect_key>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::value_modifier_key> {
		using type = ::ve::tagged_vector<dcon::value_modifier_key>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::pop_demographics_key> {
		using type = ::ve::tagged_vector<dcon::pop_demographics_key>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::demographics_key> {
		using type = ::ve::tagged_vector<dcon::demographics_key>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::provincial_modifier_value> {
		using type = ::ve::tagged_vector<dcon::provincial_modifier_value>;
	};
	
	template<>
	struct value_to_vector_type_s<dcon::national_modifier_value> {
		using type = ::ve::tagged_vector<dcon::national_modifier_value>;
	};
	
}

#endif
#ifndef DCON_RELEASE_INLINE_DEFINED
#undef DCON_RELEASE_INLINE
#ifdef _MSC_VER
#pragma warning( pop )
#endif
#else
#undef DCON_RELEASE_INLINE_DEFINED
#endif

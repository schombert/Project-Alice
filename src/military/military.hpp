#pragma once

namespace military {
namespace cb_flag {

inline constexpr uint32_t is_civil_war = 0x00000001;
inline constexpr uint32_t always = 0x00000002;
inline constexpr uint32_t is_not_triggered_only = 0x00000004;
inline constexpr uint32_t is_not_constructing_cb = 0x00000008;
inline constexpr uint32_t great_war_obligatory = 0x00000010;
inline constexpr uint32_t all_allowed_states = 0x00000020;
inline constexpr uint32_t not_in_crisis = 0x00000040;
inline constexpr uint32_t po_clear_union_sphere = 0x00000080;
inline constexpr uint32_t po_gunboat = 0x00000100;
inline constexpr uint32_t po_annex = 0x00000200;
inline constexpr uint32_t po_demand_state = 0x00000400;
inline constexpr uint32_t po_add_to_sphere = 0x00000800;
inline constexpr uint32_t po_disarmament = 0x00001000;
inline constexpr uint32_t po_reparations = 0x00002000;
inline constexpr uint32_t po_transfer_provinces = 0x00004000;
inline constexpr uint32_t po_remove_prestige = 0x00008000;
inline constexpr uint32_t po_make_puppet = 0x00010000;
inline constexpr uint32_t po_release_puppet = 0x00020000;
inline constexpr uint32_t po_status_quo = 0x00040000;
inline constexpr uint32_t po_install_communist_gov_type = 0x00080000;
inline constexpr uint32_t po_uninstall_communist_gov_type = 0x00100000;
inline constexpr uint32_t po_remove_cores = 0x00200000;
inline constexpr uint32_t po_colony = 0x00400000;
inline constexpr uint32_t po_destroy_forts = 0x00800000;
inline constexpr uint32_t po_destroy_naval_bases = 0x01000000;
inline constexpr uint32_t po_liberate = 0x02000000;
inline constexpr uint32_t po_take_from_sphere = 0x04000000;

}
}

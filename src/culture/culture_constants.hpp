#pragma once

namespace issue_rule {

constexpr inline uint32_t build_factory = 0x00000001;
constexpr inline uint32_t expand_factory = 0x00000002;
constexpr inline uint32_t open_factory = 0x00000004;
constexpr inline uint32_t destroy_factory = 0x00000008;
constexpr inline uint32_t factory_priority = 0x00000010;
constexpr inline uint32_t can_subsidise = 0x00000020;
constexpr inline uint32_t pop_build_factory = 0x00000040;
constexpr inline uint32_t pop_expand_factory = 0x00000080;
constexpr inline uint32_t pop_open_factory = 0x00000100;
constexpr inline uint32_t delete_factory_if_no_input = 0x00000200;
constexpr inline uint32_t build_factory_invest = 0x00000400;
constexpr inline uint32_t expand_factory_invest = 0x00000800;
constexpr inline uint32_t open_factory_invest = 0x00001000;
constexpr inline uint32_t build_railway_invest = 0x00002000;
constexpr inline uint32_t can_invest_in_pop_projects = 0x00004000;
constexpr inline uint32_t pop_build_factory_invest = 0x00008000;
constexpr inline uint32_t pop_expand_factory_invest = 0x00010000;
constexpr inline uint32_t pop_open_factory_invest = 0x00020000;
constexpr inline uint32_t allow_foreign_investment = 0x00040000;
constexpr inline uint32_t slavery_allowed = 0x00080000;
constexpr inline uint32_t primary_culture_voting = 0x00100000;
constexpr inline uint32_t culture_voting = 0x00200000;
constexpr inline uint32_t all_voting = 0x00400000;
constexpr inline uint32_t largest_share = 0x00800000;
constexpr inline uint32_t dhont = 0x01000000;
constexpr inline uint32_t sainte_laque = 0x02000000;
constexpr inline uint32_t same_as_ruling_party = 0x04000000;
constexpr inline uint32_t rich_only = 0x08000000;
constexpr inline uint32_t state_vote = 0x10000000;
constexpr inline uint32_t population_vote = 0x20000000;
constexpr inline uint32_t build_railway = 0x40000000;
// TODO: Add more bits to issue rules so we can accomodate both banks and universities SEPARATEDLY
// "Well the idea we had in mind is that universities are more of a state thing that you can build regardless of party, while banks of course are more tied to party policies"
constexpr inline uint32_t build_bank = 0x40000000; // tied to building railroads
constexpr inline uint32_t build_university = 0x80000000; // independent

} // namespace issue_rule

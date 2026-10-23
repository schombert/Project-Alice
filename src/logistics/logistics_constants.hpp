#pragma once

namespace logistics {


constexpr float construction_route_transport_leeway = 0.001f; // The minimum % of the total cost a route will attempt to consume and transport per day as extra leeway. This makes it so that constructions arent "stuck" for abit due to supply loss. Should be low enough that it isn't noticable.

constexpr float min_route_desired_goods_threshold = 0.5f; // The minimum % of the total desired goods a stockpile must be able to fufill to use/create a route there. For military routes, supply and reinforcements desired goods are taken into account seperately

constexpr uint8_t ineffective_supply_path_update_interval = 32; // Days interval between ineffective path updates in days. Must be active for atleast this amount of (non-consecutive) days to receive such an update
constexpr float ineffective_supply_path_throughput_cutoff = 0.85f; // Must be below this throughput to receive an ineffective path update, or fufill the loss requirement
constexpr float ineffective_supply_path_loss_cutoff = 0.35f;// Must be above this loss rate to receive an ineffective path update, or fufill the throughput requirement

constexpr uint32_t supply_route_pathfinding_batches = 30; // The desired amount of batches for processing pathfinding in parallel. Less batches = better performance when under heavy load, but the pathfinding is not able to take other potentially overlapping paths into account in the pathfinding logic.

constexpr uint32_t supply_route_inactive_days_before_deletion = 10; // consectutive inactive days before a supply route is deleted
constexpr uint32_t supply_path_inactive_days_before_deletion = 50; // consectutive inactive days before a supply path is deleted

}

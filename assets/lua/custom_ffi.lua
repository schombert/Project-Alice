local ffi = require("ffi")

---@class unit_definition
---@field build_time number
---@field default_organisation number
---@field maximum_speed number
---@field defence_or_hull number
---@field attack_or_gun_power number
---@field supply_consumption number
---@field support number
---@field siege_or_torpedo_attack number
---@field reconnaissance_or_fire_range number
---@field discipline_or_evasion number
---@field maneuver number

ffi.cdef[[
	bool alice_at_war(int32_t nation_a, int32_t nation_b);

	typedef struct {
        int32_t good;
        float amount;
    } trade_good_container;

    typedef struct {
        int32_t build_time;
        int32_t default_organisation;
        float maximum_speed;
        float defence_or_hull;
        float attack_or_gun_power;
        float supply_consumption;
        float support;
        float siege_or_torpedo_attack;
        float reconnaissance_or_fire_range;
        float discipline_or_evasion;
        float maneuver;
    } unit_variable_stats;

    unit_variable_stats* alice_get_unit_stats(int32_t unit_id);

    void call_daily(const char function_name[]);
    void remove_daily(const char function_name[]);
]]

MILITARY = {}

---@param a nation_id
---@param b nation_id
---@returns boolean
function MILITARY.at_war(a, b)
	return ffi.C.alice_at_war(a, b)
end

---returns the pointer to unit definition
---you can access it via stats[0].field
---@param a unit_type_id
---@return table<number, unit_definition>
function MILITARY.get_stats(a)
	return ffi.C.alice_get_unit_stats(a)
end

ON_ACTION = {}

---@param name string
function ON_ACTION.add_daily_call(name)
    ffi.C.call_daily(name)
end

---@param name string
function ON_ACTION.remove_daily_call(name)
    ffi.C.call_daily(name)
end
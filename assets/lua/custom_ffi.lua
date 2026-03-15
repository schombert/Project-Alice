-- local ffi = require("ffi")

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

-- These functions exposed through FFI interface should be called as ffi.C.XXX unless wrapped in a function like ones below
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

    void call_daily_battle(const char function_name[]);
    void remove_daily_battle(const char function_name[]);

    void call_battle_end(const char function_name[]);
    void remove_battle_end(const char function_name[]);

    void set_text(const char text[]);
    int32_t local_player_nation();

    void console_log(const char text[]);

    int32_t lua_get_gamerule_id_by_name(const char gamerule_name[]);
	int32_t lua_get_gamerule_option_id_by_name(const char gamerule_option_name[]);
	bool lua_check_gamerule(int32_t gamerule_id, uint8_t opt_id);
	int32_t lua_get_active_gamerule_option(int32_t gamerule_id);

	
	
	
]]


STATE = {}

---@return nation_id
function STATE.player()
    return ffi.C.local_player_nation()
end

---@param text string
function STATE.set_text(text)
    return ffi.C.set_text(text)
end


MILITARY = {}

---@param a nation_id
---@param b nation_id
---@return boolean
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


GAMERULE = {}

---returns the gamerule ID for the gamerule with the supplied name
---@param the text name of a gamerule
---@return a gamerule ID, or -1 if there is no gamerule with that name
function GAMERULE.get_gamerule_id_by_name(gamerule_name)
	return ffi.C.lua_get_gamerule_id_by_name(gamerule_name)
end


---returns a numeric ID for the gamerule option with the supplied name
---@param the text name of a gamerule option
---@return a numeric ID for the a matching gamerule option
function GAMERULE.get_gamerule_option_id_by_name(gamerule_option_name)
	return ffi.C.check_gamerule_option_by_name(gamerule_option_name)
end

---returns true if the given gamerule has the specified option ID selected, false otherwise
---@param the numeric ID of the gamerule
---@param the numeric ID of the gamerule option to check.
---@return a boolean representing if the specified gamerule option is selected
function GAMERULE.check_gamerule(gamerule_id, opt_id)
	return ffi.C.lua_check_gamerule(gamerule_id, opt_id)
end



---returns the currently active gamerule option ID for a gamerule ID
---@param the numeric ID of the gamerule
---@return a numeric ID representing the active gamerule option
function GAMERULE.get_active_gamerule_option(gamerule_id)
	return ffi.C.lua_get_active_gamerule_option(gamerule_id)
end


ON_ACTION = {}

---@param name string
function ON_ACTION.add_daily_call(name)
    ffi.C.call_daily(name)
end
---@param name string
function ON_ACTION.remove_daily_call(name)
    ffi.C.remove_daily(name)
end
---@param name string
function ON_ACTION.add_battle_call(name)
    ffi.C.call_daily_battle(name)
end
---@param name string
function ON_ACTION.remove_battle_call(name)
    ffi.C.remove_daily_battle(name)
end
---@param name string
function ON_ACTION.add_battle_end_call(name)
    ffi.C.call_battle_end(name)
end
---@param name string
function ON_ACTION.remove_battle_end_call(name)
    ffi.C.remove_battle_end(name)
end
---@param name string
function console_log(message)
    ffi.C.console_log(message)
end

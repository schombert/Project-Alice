function LOAD_SCENARIO_DATA()

end

--[[
Usage example:
(DO NOT FORGET TO DELETE ORIGINAL BUILDINGS SO YOU DON'T GET DUPLICATES!!!)

---@enum COMMODITY_GROUP
COMMODITY_GROUP  = {
	MILITARY = 0,
	RAW = 1,
	INDUSTRY = 2,
	CONSUMER = 3,
	INDUSTRIAL_CONSUMER = 4
}

---comment
---@param name string
---@param icon number
---@param color number[]
---@return commodity_id
local function load_commodity(name, icon, color)
	local item = COMMODITY.create()
	local name_key = TEXT.create_key(name)
	COMMODITY.set_name(item, name_key)
	COMMODITY.set_commodity_group(item, COMMODITY_GROUP.CONSUMER)
	COMMODITY.set_cost(item, 1)
	COMMODITY.set_median_price(item, 1)
	COMMODITY.set_is_available_from_start(item, true)
	COMMODITY.set_is_local(item, false)
	COMMODITY.set_overseas_penalty(item, false)
	COMMODITY.set_uses_potentials(item, false)
	COMMODITY.set_money_rgo(item, false)
	COMMODITY.set_icon(item, icon)
	-- ABGR
	COMMODITY.set_color(item, color[3] * (2 ^ 16) + color[2] * (2 ^ 8) + color[1])
	return item
end
---@alias Container {[1] : commodity_id, [2] : number}

---@param name string
---@param inputs Container[]
---@param efficiency_inputs Container[]
---@param output Container
---@param construction_cost Container[]
---@param construction_time number?
local function load_factory(name, inputs, efficiency_inputs, output, construction_cost, construction_time)
	local item = FACTORY_TYPE.create()
	local name_key = TEXT.create_key(name)
	local description_key = TEXT.create_key(name .. "_desc")
	FACTORY_TYPE.set_name(item, name_key)
	FACTORY_TYPE.set_description(item, description_key)
	if construction_time == nil then
		FACTORY_TYPE.set_construction_time(item, 360)
	else
		FACTORY_TYPE.set_construction_time(item, construction_time)
	end
	FACTORY_TYPE.set_is_available_from_start(item, true)
	FACTORY_TYPE.set_can_be_built_in_colonies(item, false)
	FACTORY_TYPE.set_factory_tier(item, 0)
	FACTORY_TYPE.set_is_coastal(item, false)

	for _, value in ipairs(construction_cost) do
		FACTORY_TYPE_DATA.COST.add(item, value[1], value[2])
	end
	for _, value in ipairs(inputs) do
		FACTORY_TYPE_DATA.INPUT.add(item, value[1], value[2])
	end
	for _, value in ipairs(efficiency_inputs) do
		FACTORY_TYPE_DATA.EFFICIENCY.add(item, value[1], value[2])
	end

	FACTORY_TYPE.set_output(item, output[1])
	FACTORY_TYPE.set_output_amount(item, output[2])
	FACTORY_TYPE.set_base_workforce(item, 1000)
end

---comment
---@param goods Container[]
---@return consumption_category_id
local function load_consumption_category(weight, depends_on, scales_with, goods)
	local item = CONSUMPTION_CATEGORY.create()
	if (depends_on) then
		-- Consumption is discouraged unless pop is able to buy this category
		CONSUMPTION_CATEGORY.set_buy_after(item, depends_on)
	end
	if (scales_with) then
		-- Consumption is discouraged unless pop is able to buy this good from previously buy_after category
		-- For example, it could be automobiles from transportation need which discourage consumption of fuel
		CONSUMPTION_CATEGORY.set_scale_with(item, scales_with)
	end
	for index, value in ipairs(goods) do
		CONSUMPTION_CATEGORY.set_weights(item, value[1], value[2])
	end
	CONSUMPTION_CATEGORY.set_satisfaction_score(item, weight)

	-- Pops would prefer to buy goods with actual availability
	CONSUMPTION_CATEGORY.set_availability_pressure(item, 1)

	-- When negative, pops will try to avoid buying expensive goods from this category
	-- When positive, they will be encouraged to buy expensive goods rather than cheap
	CONSUMPTION_CATEGORY.set_price_pressure(item, -10)

	-- Base pressure to buy goods from this category.
	-- Set high to get more uniform weights.
	CONSUMPTION_CATEGORY.set_base_pressure(item, 0.3)
	return item
end

---comment
---@param item consumption_category_id
local function set_as_luxury(item)
	CONSUMPTION_CATEGORY.set_price_pressure(item, 1)
	CONSUMPTION_CATEGORY.set_base_pressure(item, 1.0)
end


function LOAD_SCENARIO_DATA()
	-- OBLIGATORY FOR NOW
	local money = load_commodity("money", 0, {50, 50, 50})

	-- Load your goods

	local raw_ammunition = load_commodity("raw_ammunition", 2, {100, 100, 100})
	local alloys = load_commodity("alloys", 3, {0, 100, 100})
	local super_alloys = load_commodity("super_alloys", 4, {100, 0, 100})
	local ammunition = load_commodity("ammunition", 1, {200, 200, 100})

	-- Create convenience tables
	local ammunition_RGO = {raw_ammunition}
	local ammunition_needs_desc = {{ammunition, 1}}
	local super_alloys_needs_desc = {{super_alloys, 1}}

	-- Set RGO data
	for _, item in ipairs(ammunition_RGO) do
		EFFICIENCY.RGO.add(item, fertilizer, 0.1)
		EFFICIENCY.RGO.add(item, steel, 0.1)
		EFFICIENCY.RGO.add(item, automobiles, 0.1)
		COMMODITY.set_rgo_amount(item, 10)		
		COMMODITY.set_rgo_efficiency_inputs_are_defined_in_content(item, true)
	end

	-- Create factories.

	local default_efficiency = {{alloys, 0.1}}
	local default_cost = {{alloys, 100}, {super_alloys, 100}}

	load_factory("ammunition_factory", { {raw_ammunition, 2} }, default_efficiency, {ammunition, 1}, default_cost)

	-- Create categories of needs

	local basic_food = load_consumption_category(0.05, nil, nil, ammunition_needs_desc)
	local cool_food = load_consumption_category(0.1, basic_food, nil, super_alloys_needs_desc)	
	set_as_luxury(cool_food)
--]]

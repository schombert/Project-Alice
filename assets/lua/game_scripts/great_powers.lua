-- Example of modding great powers ranking and selection


-- local total_demographics_key_raw = 0
-- local total_demographics_key = total_demographics_key_raw --[[@as demographics_key]]

-- ---@param nation nation_id
-- local function exists(nation)
-- 	return NATION.get_owned_province_count(nation) > 0
-- end

-- ---@param nation nation_id
-- local function eligible(nation)
-- 	-- if not NATION.get_is_civilized(nation) then
-- 		-- return false
-- 	-- end

-- 	if NATION.get_flag_variables(nation, country_flag.serfdom_not_abolished) then
-- 		return false
-- 	end

-- 	local overlord_rel = NATION.get_overlord_as_subject(nation)
-- 	local overlord = OVERLORD.get_ruler(overlord_rel)
-- 	if (overlord_rel ~= -1) and (overlord ~= -1) then
-- 		return false
-- 	end

-- 	if NATION.get_owned_province_count(nation) < 30 then
-- 		return false
-- 	end

-- 	local non_colonial_state_count = 0
-- 	for state_index = 0, NATION.get_range_length_state_ownership(nation) - 1 do
-- 		local state_ownership = NATION.get_item_from_range_state_ownership(nation, state_index)
-- 		local state = STATE_OWNERSHIP.get_state(state_ownership)
-- 		local capital = STATE_INSTANCE.get_capital(state)
-- 		local is_colonial = PROVINCE.get_is_colonial(capital)
-- 		if not is_colonial then
-- 			non_colonial_state_count = non_colonial_state_count + 1
-- 		end
-- 	end

-- 	if non_colonial_state_count < 4 then
-- 		return false
-- 	end


-- 	return true
-- end


-- local function relatively_high_population(nation, max_population)
-- 	local population = NATION.get_demographics(nation, total_demographics_key)
-- 	if population < max_population / 20 then
-- 		return false
-- 	end
-- 	return true
-- end

-- local function not_behind_in_tech(nation, tech_cache, max_tech)
-- 	if tech_cache[nation] > max_tech - 20 then
-- 		return true
-- 	end
-- 	return false
-- end


-- ---@class RankedNation
-- ---@field nation nation_id
-- ---@field total_score number
-- ---@field legit_great_power_candidate boolean
-- ---@field exists boolean

-- ---@param max_great_powers number
-- function UPDATE_GREAT_POWERS(max_great_powers)
-- 	local max_population = 0
-- 	local max_technology = 0

-- 	---@type table<number, number>
-- 	local tech_cache = {}

-- 	for nation = 0, NATION.size() - 1 do
-- 		local nid = nation --[[@as nation_id --]]

-- 		if eligible(nid) then
-- 			local pop = NATION.get_demographics(nid, total_demographics_key)
-- 			if pop > max_population then
-- 				max_population = pop
-- 			end

-- 			local national_tech = 0

-- 			for tech = 0, TECHNOLOGY.size() - 1 do
-- 				if NATION.get_active_technologies(nid, tech) then
-- 					national_tech = national_tech + 1
-- 				end
-- 			end

-- 			tech_cache[nation] = national_tech

-- 			if national_tech > max_technology then
-- 				max_technology = national_tech
-- 			end
-- 		end
-- 	end

-- 	console_log("MAX_POP" .. max_population)

-- 	for nation = 0, NATION.size() - 1 do
-- 		NATION.set_is_great_power_candidate(nation --[[@as nation_id --]], false)
-- 	end

-- 	---@type RankedNation[]
-- 	local nations_by_rank = {}
-- 	for nation = 0, NATION.size() - 1 do
-- 		local nid = nation --[[@as nation_id]]
-- 		---@type RankedNation
-- 		local to_add = {
-- 			nation = nid,
-- 			total_score = NATION.get_industrial_score(nid) + NATION.get_military_score(nid),
-- 			legit_great_power_candidate =
-- 				eligible(nid)
-- 				and relatively_high_population(nid, max_population)
-- 				and exists(nid)
-- 				and not_behind_in_tech(nid, tech_cache, max_technology),
-- 			exists = exists(nid)
-- 		}
-- 		table.insert(nations_by_rank, to_add)
-- 	end

-- 	table.sort(nations_by_rank, function (a, b)
-- 		if a.exists ~= b.exists then
-- 			return a.exists
-- 		end
-- 		if a.legit_great_power_candidate ~= b.legit_great_power_candidate then
-- 			return a.legit_great_power_candidate
-- 		end
-- 		return a.total_score > b.total_score
-- 	end)

-- 	for index, value in ipairs(nations_by_rank) do
-- 		NATION.set_rank(value.nation, index)
-- 	end


-- 	local greatpowersfound = 0
-- 	local i = 1
-- 	local i_max = #nations_by_rank
-- 	while greatpowersfound < max_great_powers do
-- 		if (i > i_max) then
-- 			break
-- 		end
-- 		if nations_by_rank[i].legit_great_power_candidate then

-- 		console_log("GREAT POWER: " .. nations_by_rank[i].nation)
-- 			NATION.set_is_great_power_candidate(nations_by_rank[i].nation, true)
-- 			greatpowersfound = greatpowersfound + 1
-- 		else
-- 			break
-- 		end
-- 		i = i + 1
-- 	end

-- 	console_log("GREAT POWERS FOUND: " .. greatpowersfound)
-- end
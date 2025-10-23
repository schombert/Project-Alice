io.write("LUA LOADED");

TIMER = 0

function alice.on_ui_thread_update(dt_microseconds, window_width, window_height)
	TIMER = TIMER + dt_microseconds / 1000 / 500
	alice.graphics.rect(window_width / 2 + math.cos(TIMER) * 100 - 10, window_height / 2 + math.sin(TIMER) * 100 - 10, 20, 20)
end

function alice.update_control_element()
	local nation = STATE.player()
	local control = 1

	if nation ~= -1 then
		local total = 0
		local provinces = NATION.get_range_length_province_ownership(nation)
		for i = 0, provinces - 1 do
			local ownership = NATION.get_item_from_range_province_ownership(nation, i)
			local province = PROVINCE_OWNERSHIP.get_province(ownership)
			local control_prov = PROVINCE.get_control_ratio(province)
			total = total + control_prov ---@type number
		end
		if provinces > 0 then
			control = total / provinces
		else
			control = 1
		end
	end
	STATE.set_text("Avg. Control: " .. tostring(math.floor(100 * control)) .. "%")
end

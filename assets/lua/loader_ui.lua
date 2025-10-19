io.write("LUA LOADED");

TIMER = 0

function alice_ui.on_ui_thread_update(dt_microseconds, window_width, window_height)
	TIMER = TIMER + dt_microseconds / 1000 / 500
	alice_ui.graphics.rect(window_width / 2 + math.cos(TIMER) * 100 - 10, window_height / 2 + math.sin(TIMER) * 100 - 10, 20, 20)
end

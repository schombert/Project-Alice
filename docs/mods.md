## Modding support

As of writing, Alice doesn't properly support most of the popular mods without a patch (which shouldn't be the case once we get near to 1.0).

If you're impertinent to try out the mods, you may be able to apply the following diff patch to your Alice codebase:

```diff
diff --git a/src/common_types/container_types.hpp b/src/common_types/container_types.hpp
index 9b7801f9..d312692b 100644
--- a/src/common_types/container_types.hpp
+++ b/src/common_types/container_types.hpp
@@ -48,7 +48,7 @@ struct event_option {
 	dcon::effect_key effect;
 };
 
-constexpr int32_t max_event_options = 6;
+constexpr int32_t max_event_options = 8;
 
 struct modifier_hash {
 	using is_avalanching = void;
diff --git a/src/entry_point_nix.cpp b/src/entry_point_nix.cpp
index da0afa0f..78aca3cd 100644
--- a/src/entry_point_nix.cpp
+++ b/src/entry_point_nix.cpp
@@ -1,6 +1,6 @@
 #include "system_state.hpp"
 
-int main() {
+int main(int argc, char **argv) {
 	std::unique_ptr<sys::state> game_state = std::make_unique<sys::state>(); // too big for the stack
 
 	assert(
@@ -10,6 +10,23 @@ int main() {
 	add_root(game_state->common_fs,
 			NATIVE(".")); // will add the working directory as first root -- for the moment this lets us find the shader files
 
+	{
+		std::vector<std::string> cmd_args;
+		for(int i = 1; i < argc; ++i)
+			cmd_args.push_back(std::string{ argv[i] });
+		parsers::error_handler err("");
+		parsers::scenario_building_context context(*game_state);
+		auto root = get_root(game_state->common_fs);
+		for(const auto& mod_filename : cmd_args) {
+			auto mod_file = open_file(root, mod_filename);
+			auto content = view_contents(*mod_file);
+			err.file_name = mod_filename;
+			parsers::token_generator gen(content.data, content.data + content.file_size);
+			parsers::mod_file_context mod_file_context(context);
+			parsers::parse_mod_file(gen, err, mod_file_context);
+		}
+	}
+
 	if(!sys::try_read_scenario_and_save_file(*game_state, NATIVE("development_test_file.bin"))) {
 		// scenario making functions
 		game_state->load_scenario_data();
diff --git a/src/entry_point_win.cpp b/src/entry_point_win.cpp
index 606db1ac..e3642da8 100644
--- a/src/entry_point_win.cpp
+++ b/src/entry_point_win.cpp
@@ -12,7 +12,7 @@
 
 #pragma comment(lib, "Ole32.lib")
 
-int WINAPI wWinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, LPWSTR /*lpCmdLine*/, int /*nCmdShow*/
+int WINAPI wWinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, LPWSTR lpCmdLine, int /*nCmdShow*/
 ) {
 
 #ifdef _DEBUG
@@ -47,6 +47,18 @@ int WINAPI wWinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, LPWSTR
 			RegCloseKey(hKey);
 		}
 
+		if(lpCmdLine) {
+			parsers::error_handler err("");
+			parsers::scenario_building_context context(*game_state);
+			auto root = get_root(game_state->common_fs);
+			auto mod_file = open_file(root, lpCmdLine);
+			auto content = view_contents(*mod_file);
+			err.file_name = lpCmdLine;
+			parsers::token_generator gen(content.data, content.data + content.file_size);
+			parsers::mod_file_context mod_file_context(context);
+			parsers::parse_mod_file(gen, err, mod_file_context);
+		}
+
 		if(!sys::try_read_scenario_and_save_file(*game_state, NATIVE("development_test_file.bin"))) {
 			// scenario making functions
 			game_state->load_scenario_data();
diff --git a/src/gamestate/modifiers.hpp b/src/gamestate/modifiers.hpp
index a8c06cd2..ae028f93 100644
--- a/src/gamestate/modifiers.hpp
+++ b/src/gamestate/modifiers.hpp
@@ -206,14 +206,14 @@ constexpr inline uint32_t count = MOD_NAT_LIST_COUNT;
 } // namespace national_mod_offsets
 
 struct provincial_modifier_definition {
-	static constexpr uint32_t modifier_definition_size = 10;
+	static constexpr uint32_t modifier_definition_size = 25;
 
 	float values[modifier_definition_size] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
 	dcon::provincial_modifier_value offsets[modifier_definition_size] = {dcon::provincial_modifier_value{}};
 };
 
 struct national_modifier_definition {
-	static constexpr uint32_t modifier_definition_size = 10;
+	static constexpr uint32_t modifier_definition_size = 25;
 
 	float values[modifier_definition_size] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
 	dcon::national_modifier_value offsets[modifier_definition_size] = {dcon::national_modifier_value{}};
diff --git a/src/gamestate/system_state.cpp b/src/gamestate/system_state.cpp
index 605cc42a..1691bea1 100644
--- a/src/gamestate/system_state.cpp
+++ b/src/gamestate/system_state.cpp
@@ -926,9 +926,14 @@ std::string_view state::to_string_view(dcon::unit_name_id tag) const {
 }
 
 dcon::trigger_key state::commit_trigger_data(std::vector<uint16_t> data) {
+	if(trigger_data_indices.empty()) { //placeholder for invalid triggers
+		trigger_data_indices.push_back(0);
+		trigger_data.push_back(uint16_t(trigger::integer_scope));
+		trigger_data.push_back(uint16_t(2));
+		trigger_data.push_back(uint16_t(1));
+	}
+	
 	if(data.size() == 0) {
-		if(trigger_data_indices.empty())
-			trigger_data_indices.push_back(int32_t(0));
 		return dcon::trigger_key();
 	}
 
@@ -957,8 +962,15 @@ dcon::trigger_key state::commit_trigger_data(std::vector<uint16_t> data) {
 }
 
 dcon::effect_key state::commit_effect_data(std::vector<uint16_t> data) {
-	if(data.size() == 0)
+	if(effect_data.empty()) { //placeholder for invalid effects
+		effect_data.push_back(uint16_t(trigger::integer_scope));
+		effect_data.push_back(uint16_t(2));
+		effect_data.push_back(uint16_t(1));
+	}
+
+	if(data.size() == 0) {
 		return dcon::effect_key();
+	}
 
 	auto start = effect_data.size();
 	auto size = data.size();
diff --git a/src/map/map.cpp b/src/map/map.cpp
index c36d6460..5e2f41ea 100644
--- a/src/map/map.cpp
+++ b/src/map/map.cpp
@@ -666,6 +666,8 @@ void display_data::set_unit_arrows(std::vector<std::vector<glm::vec2>> const& ar
 
 GLuint load_dds_texture(simple_fs::directory const& dir, native_string_view file_name) {
 	auto file = simple_fs::open_file(dir, file_name);
+	if(!bool(file))
+		return 0;
 	auto content = simple_fs::view_contents(*file);
 	uint32_t size_x, size_y;
 	uint8_t const* data = (uint8_t const*)(content.data);
diff --git a/src/map/map_data_loading.cpp b/src/map/map_data_loading.cpp
index 94efdd6c..fa500580 100644
--- a/src/map/map_data_loading.cpp
+++ b/src/map/map_data_loading.cpp
@@ -104,7 +104,7 @@ void display_data::load_provinces_mid_point(parsers::scenario_building_context&
 
 		glm::vec2 tile_pos;
 
-		assert(tiles_number[i] > 0); // yeah but a province without tiles is no bueno
+		//assert(tiles_number[i] > 0); // yeah but a province without tiles is no bueno
 
 		if(tiles_number[i] == 0) {
 			tile_pos = glm::vec2(0, 0);
diff --git a/src/parsing/effect_parsing.cpp b/src/parsing/effect_parsing.cpp
index 87131ed7..071fea76 100644
--- a/src/parsing/effect_parsing.cpp
+++ b/src/parsing/effect_parsing.cpp
@@ -1102,9 +1102,12 @@ dcon::effect_key make_effect(token_generator& gen, error_handler& err, effect_bu
 		return dcon::effect_key{0};
 	}
 
-	auto const new_size = simplify_effect(context.compiled_effect.data());
-	context.compiled_effect.resize(static_cast<size_t>(new_size));
-
+	if(err.accumulated_errors.empty()) {
+		auto const new_size = simplify_effect(context.compiled_effect.data());
+		context.compiled_effect.resize(static_cast<size_t>(new_size));
+	} else {
+		context.compiled_effect.clear();
+	}
 	return context.outer_context.state.commit_effect_data(context.compiled_effect);
 }
 
diff --git a/src/parsing/nations_parsing.cpp b/src/parsing/nations_parsing.cpp
index 3f53e7d8..4f970ea9 100644
--- a/src/parsing/nations_parsing.cpp
+++ b/src/parsing/nations_parsing.cpp
@@ -978,8 +978,13 @@ sys::event_option make_event_option(token_generator& gen, error_handler& err, ev
 															" cells big, which exceeds 64 KB bytecode limit (" + err.file_name + ")";
 		return sys::event_option{opt_result.name_, opt_result.ai_chance, dcon::effect_key{0}};
 	}
-	auto const new_size = simplify_effect(e_context.compiled_effect.data());
-	e_context.compiled_effect.resize(static_cast<size_t>(new_size));
+
+	if(err.accumulated_errors.empty()) {
+		auto const new_size = simplify_effect(e_context.compiled_effect.data());
+		e_context.compiled_effect.resize(static_cast<size_t>(new_size));
+	} else {
+		e_context.compiled_effect.clear();
+	}
 
 	auto effect_id = context.outer_context.state.commit_effect_data(e_context.compiled_effect);
 
diff --git a/src/parsing/parsers_declarations.cpp b/src/parsing/parsers_declarations.cpp
index 1e8ce7f0..8d539ddf 100644
--- a/src/parsing/parsers_declarations.cpp
+++ b/src/parsing/parsers_declarations.cpp
@@ -2954,24 +2954,25 @@ void mod_file::finish(mod_file_context& context) {
 	// If there isn't any path then we aren't required to do anything
 	if(context.path.empty())
 		return;
-
+	
 	auto& fs = context.outer_context.state.common_fs;
-
-	// Add root of mod_path
-	for(auto replace_path : context.replace_paths) {
-		native_string path_block = simple_fs::list_roots(fs)[0];
-		path_block += NATIVE_DIR_SEPARATOR;
-		path_block += simple_fs::correct_slashes(simple_fs::utf8_to_native(replace_path));
-		if(path_block.back() != NATIVE_DIR_SEPARATOR)
+	const auto roots = simple_fs::list_roots(fs);
+	for(const auto& root : roots) {
+		// Add root of mod_path
+		for(auto replace_path : context.replace_paths) {
+			native_string path_block = root;
 			path_block += NATIVE_DIR_SEPARATOR;
+			path_block += simple_fs::correct_slashes(simple_fs::utf8_to_native(replace_path));
+			if(path_block.back() != NATIVE_DIR_SEPARATOR)
+				path_block += NATIVE_DIR_SEPARATOR;
 
-		simple_fs::add_ignore_path(fs, path_block);
+			simple_fs::add_ignore_path(fs, path_block);
+		}
+		native_string mod_path = root;
+		mod_path += NATIVE_DIR_SEPARATOR;
+		mod_path += simple_fs::correct_slashes(simple_fs::utf8_to_native(context.path));
+		add_root(fs, mod_path);
 	}
-
-	native_string mod_path = simple_fs::list_roots(fs)[0];
-	mod_path += NATIVE_DIR_SEPARATOR;
-	mod_path += simple_fs::correct_slashes(simple_fs::utf8_to_native(context.path));
-	add_root(fs, mod_path);
 }
 
 } // namespace parsers
diff --git a/src/parsing/trigger_parsing.cpp b/src/parsing/trigger_parsing.cpp
index daa864f9..fac1d821 100644
--- a/src/parsing/trigger_parsing.cpp
+++ b/src/parsing/trigger_parsing.cpp
@@ -692,9 +692,13 @@ int32_t simplify_trigger(uint16_t* source) {
 
 dcon::trigger_key make_trigger(token_generator& gen, error_handler& err, trigger_building_context& context) {
 	tr_scope_and(gen, err, context);
-	auto const new_size = simplify_trigger(context.compiled_trigger.data());
-	context.compiled_trigger.resize(static_cast<size_t>(new_size));
 
+	if(err.accumulated_errors.empty()) {
+		auto const new_size = simplify_trigger(context.compiled_trigger.data());
+		context.compiled_trigger.resize(static_cast<size_t>(new_size));
+	} else {
+		context.compiled_trigger.clear();
+	}
 	return context.outer_context.state.commit_trigger_data(context.compiled_trigger);
 }
 
@@ -705,9 +709,12 @@ void make_value_modifier_segment(token_generator& gen, error_handler& err, trigg
 	auto new_factor = context.factor;
 	context.factor = old_factor;
 
-	auto const new_size = simplify_trigger(context.compiled_trigger.data());
-	context.compiled_trigger.resize(static_cast<size_t>(new_size));
-
+	if(err.accumulated_errors.empty()) {
+		auto const new_size = simplify_trigger(context.compiled_trigger.data());
+		context.compiled_trigger.resize(static_cast<size_t>(new_size));
+	} else {
+		context.compiled_trigger.clear();
+	}
 	auto tkey = context.outer_context.state.commit_trigger_data(context.compiled_trigger);
 	context.compiled_trigger.clear();
 

```

What this patch does is that it increments the size of possible modifiers (thus allowing mods to add as many modifiers as they wish, more than it's normally allowed) - this however, incurs a huge penalty on memory size and speed.
Additionally, all erroneous triggers are discarded and set into a "dummy trigger". This is a bad idea to push upstream because it is only a band-aid to a deeper problem.
And the roots are added so you can have mods alongside Alice's exe. This is also a bad idea due to the fact it exponentially adds roots, causing worse loading times.
We also add command line handling to specify your own `.mod` files via the command line, this isn't accepted due to the fact it's an ugly way to skirt around - and should probably be replaced with a more elegant solution.
So be cautious, this patch isn't on upstream for various good reasons, and should only be used to improve mod compatibility and not to play the mods themselves.

## Identified issues

Some issues are solved by Project Alice already, some aren't, feel free to add your bitter experiences below for consideration post-1.0:

- No easy "if-else" statment chains on triggers
- No first-hand support for dynamic localisation (without using change_region_name)
- Trade window only supports N number of goods before it overflows
- Production goods tab only supports N number of goods, perhaps a horizontal scrollbar would fix it?
- Undocumented and inconsistent modifiers (some that can only be used on technologies and inventions)
- Have to add 1 technology for each folder so the UI doesn't break
- No unicode support
- Several bugs with investment and the economy overall, making it very broken on some mods
- Hidden AI weights
- Selecting which wars to call an ally into
- "Transfer land/states" diplomacy action

## Mod support

This list may or may not be out of date, it's wise to check the mods yourself for verifying (and updating this list as needed). And here they be, in no particular order:

| Mod | Works w/o patch | Works with patch | Notes |
|---|---|---|---|
| [GFM - Greater Flavour Mod](https://github.com/Historical-Expansion-Mod/Greater-Flavor-Mod/releases/tag/v2.1) | No | No | Crashes when loading |
| [TGC - The Great Combination](https://github.com/rderekp/The-Grand-Combo/releases/tag/v0.96.4) | No | Somewhat | Loads, crashes when running, requires intricate work to make Universities and Banks work |
| [Divergences of Darkness](https://www.moddb.com/mods/divergences-of-darkness/downloads/divergences-of-darkness-hungary-patch) | No | Somewhat | Loads, crashes when running |
| [HPM - Historical Project Mod](https://www.moddb.com/mods/historical-project-mod/downloads/hpm-0463) | No | Somewhat | Loads, crashes when running |
| [CoE RoI - Concert of Europe: Roar of Industry](https://www.moddb.com/mods/the-concert-of-europe-roi/downloads/coeroi-2-0-1) | No | Somewhat | Loads, crashes when running |
| [BAI - Blood And Iron](https://github.com/antonius117/BAI/releases/tag/Patch_06_11) | No | Somewhat | Loads, crashes when running |
| [UPM - Universal Project Mod](https://www.moddb.com/mods/universal-project-mod/downloads/universal-project-mod-1-0-first-release) | No | Somewhat | Loads, crashes when running |
| [AoE Remake - Age of Enlightment Remake](https://www.moddb.com/mods/age-of-enlightenment/downloads/age-of-enlightenment-sp-remake-version-10) | No | Somewhat | Loads, crashes when running |
| [crimeamod](https://www.moddb.com/mods/crimeamod/downloads/crimeamod-141-unciv-edition) | No | Yes |  |
| [The Northern Struggle](https://www.moddb.com/mods/the-northern-struggle/downloads/the-northern-struggle-234-the-african-update) | No | Yes |  |
| [RLWAD - Real Life was a Dream](https://www.moddb.com/mods/real-life-was-a-dream-rlwad/downloads/rlwad-mod) | No | Yes |  |
| [Victoria Universalis](https://www.moddb.com/mods/victoria-universalis/downloads/victoria-universalis-v077) | No | Somewhat | Loads, crashes when running |
| [Wackria](https://www.moddb.com/mods/wakracia-steel-and-coal/downloads/wakracia-13-brazil-update) | No | Somewhat | Loads, crashes when running |
| [Supernations Mod](https://www.moddb.com/mods/victoria-ii-supernations-mod/downloads/victoria-ii-supernations-mod-v151-the-disun) | No | Somewhat | Loads, crashes when running |
| [Harolds Triumph](https://github.com/FeelsAtlasMan/Harald-s-Triumph) | No | Somewhat | Loads, crashes when running |
| [Divergences of Darkness Expanded](https://github.com/rileyo92/Divergence-Expanded-MP/releases/tag/2.3.2) | No | Somewhat | Loads, crashes when running |

(The links provided are the exact versions being tested).

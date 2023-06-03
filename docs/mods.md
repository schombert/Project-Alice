# Modding support

As of writing, Alice doesn't properly support most of the popular mods without a patch (which shouldn't be the case once we get near to 1.0).

If you're impertinent to try out the mods, you may be able to apply the following diff patch to your Alice codebase:

```diff
diff --git a/src/entry_point_nix.cpp b/src/entry_point_nix.cpp
index 2dc422d..f5c0f44 100644
--- a/src/entry_point_nix.cpp
+++ b/src/entry_point_nix.cpp
@@ -7,6 +7,23 @@ int main() {
 	add_root(game_state->common_fs, NATIVE_M(GAME_DIR)); // game files directory is overlaid on top of that
 	add_root(game_state->common_fs, NATIVE("."));        // will add the working directory as first root -- for the moment this lets us find the shader files
 
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
index 3144322..ba5adbf 100644
--- a/src/entry_point_win.cpp
+++ b/src/entry_point_win.cpp
@@ -15,7 +15,7 @@
 int WINAPI wWinMain(
     HINSTANCE /*hInstance*/,
     HINSTANCE /*hPrevInstance*/,
-    LPWSTR /*lpCmdLine*/,
+    LPWSTR lpCmdLine,
     int /*nCmdShow*/
 ) {
 
@@ -50,6 +50,18 @@ int WINAPI wWinMain(
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
index 794a61c..0f89293 100644
--- a/src/gamestate/modifiers.hpp
+++ b/src/gamestate/modifiers.hpp
@@ -197,14 +197,14 @@ constexpr inline uint32_t count = MOD_NAT_LIST_COUNT;
 } // namespace national_mod_offsets
 
 struct provincial_modifier_definition {
-	static constexpr uint32_t modifier_definition_size = 10;
+	static constexpr uint32_t modifier_definition_size = 24;
 
 	float values[modifier_definition_size] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
 	dcon::provincial_modifier_value offsets[modifier_definition_size] = {dcon::provincial_modifier_value{}};
 };
 
 struct national_modifier_definition {
-	static constexpr uint32_t modifier_definition_size = 10;
+	static constexpr uint32_t modifier_definition_size = 24;
 
 	float values[modifier_definition_size] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
 	dcon::national_modifier_value offsets[modifier_definition_size] = {dcon::national_modifier_value{}};
diff --git a/src/map/map.cpp b/src/map/map.cpp
index c0d9e61..298f79f 100644
--- a/src/map/map.cpp
+++ b/src/map/map.cpp
@@ -876,6 +876,9 @@ void display_data::load_map_data(parsers::scenario_building_context& context) {
 }
 
 GLuint load_dds_texture(simple_fs::directory const & dir, native_string_view file_name) {
+	if(!bool(file))
+		return 0;
+
 	auto file = simple_fs::open_file(dir, file_name);
 	auto content = simple_fs::view_contents(*file);
 	uint32_t size_x, size_y;
diff --git a/src/parsing/parsers_declarations.cpp b/src/parsing/parsers_declarations.cpp
index 270a4d8..bd8fef0 100644
--- a/src/parsing/parsers_declarations.cpp
+++ b/src/parsing/parsers_declarations.cpp
@@ -2530,13 +2530,16 @@ void mod_file::finish(mod_file_context& context) {
 	auto& fs = context.outer_context.state.common_fs;
 
 	// Add root of mod_path
-	for(auto replace_path : context.replace_paths) {
-		native_string path_block = simple_fs::list_roots(fs)[0];
-		path_block += NATIVE_DIR_SEPARATOR;
-		path_block += simple_fs::correct_slashes(simple_fs::utf8_to_native(replace_path));
-		if(path_block.back() != NATIVE_DIR_SEPARATOR)
-			path_block += NATIVE_DIR_SEPARATOR;
-
+ 	auto& fs = context.outer_context.state.common_fs;
+	const auto roots = simple_fs::list_roots(fs);
+	for(const auto& root : roots) {
+		for(auto replace_path : context.replace_paths) {
+			native_string path_block = root;
+ 			path_block += NATIVE_DIR_SEPARATOR;
+			path_block += simple_fs::correct_slashes(simple_fs::utf8_to_native(replace_path));
+			if(path_block.back() != NATIVE_DIR_SEPARATOR)
+				path_block += NATIVE_DIR_SEPARATOR;
+		}
 		simple_fs::add_ignore_path(fs, path_block);
 	}
 
diff --git a/src/parsing/trigger_parsing.cpp b/src/parsing/trigger_parsing.cpp
index 5a3fb74..440ea83 100644
--- a/src/parsing/trigger_parsing.cpp
+++ b/src/parsing/trigger_parsing.cpp
@@ -670,6 +670,8 @@ int32_t simplify_trigger(uint16_t* source) {
 dcon::trigger_key make_trigger(token_generator& gen, error_handler& err, trigger_building_context& context) {
 	tr_scope_and(gen, err, context);
 	assert(context.compiled_trigger.size() <= std::numeric_limits<uint16_t>::max());
+	if(!err.accumulated_errors.empty())
+		return dcon::trigger_key{1}; // Can't rely on a trigger with errors!
 
 	const auto new_size = simplify_trigger(context.compiled_trigger.data());
 	context.compiled_trigger.resize(static_cast<size_t>(new_size));

diff --git a/src/common_types/container_types.hpp b/src/common_types/container_types.hpp
index 5505214..b9d085a 100644
--- a/src/common_types/container_types.hpp
+++ b/src/common_types/container_types.hpp
@@ -48,7 +48,7 @@ struct event_option {
 	dcon::effect_key effect;
 };
 
-constexpr int32_t max_event_options = 6;
+constexpr int32_t max_event_options = 8;
 
 struct modifier_hash {
 	using is_avalanching = void;
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

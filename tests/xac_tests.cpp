#include "xac.hpp"

#ifndef IGNORE_REAL_FILES_TESTS
TEST_CASE("xac reading", "[xac tests]") {
	parsers::error_handler err("");
	REQUIRE(std::string("NONE") != GAME_DIR); // If this fails, then you have not created a local_user_settings.hpp (read the documentation for contributors)

	std::unique_ptr<sys::state> state = std::make_unique<sys::state>();
	add_root(state->common_fs, NATIVE_M(GAME_DIR));
	auto root = get_root(state->common_fs);
	auto gfx = open_directory(root, NATIVE("gfx"));
	auto anims = open_directory(gfx, NATIVE("anims"));

	auto xac_file = open_file(anims, NATIVE("British_Helmet21.xac"));
	REQUIRE(bool(xac_file));

	auto content = view_contents(*xac_file);
	ogl::parse_xac(content.data, content.data + content.file_size, err);
	REQUIRE(err.accumulated_errors == "");
	REQUIRE(err.accumulated_warnings == "");
}
#endif

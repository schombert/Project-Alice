#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include "parsers.hpp"

struct basic_object_a {
    int32_t int_value = 0;
    float float_value = 0.0f;
    int32_t key_A = 0;
    std::string_view stored_text;

    void any(parsers::token_and_type left, parsers::association_type assoc, std::string_view txt, parsers::error_handler& err) {
        stored_text = txt;
    }

    void finish() { }
};

#include "test_parsers_generated.hpp"

TEST_CASE("Dummy test", "[dummy test instance]") {
    REQUIRE(1 + 1 == 2); 
}

/*
* basic_object_a
	#value: int, member, (int_value)
	#set: discard, none, group
	#any: value member_fn text
	key_A: value member int
	key_C: value member float (float_value)
	key_B: group, none, discard
*/

TEST_CASE("Generated Parsers Tests", "[trivial parser]") {
    char file_data[] = "{ a b\nc }\nunk_key = free_text 11 key_A = 3\nkey_C = 2.5 key_B  = { 1 2 3}";

    parsers::error_handler err("no file");
    parsers::token_generator gen(file_data, file_data + strlen(file_data));

    auto created_object = parsers::parse_basic_object_a(gen, err, 0);

    REQUIRE(created_object.key_A == 3);
    REQUIRE(created_object.int_value == 11);
    REQUIRE(created_object.float_value == 2.5f);
    REQUIRE(created_object.stored_text == "free_text");
    REQUIRE(err.accumulated_errors.length() == 0);
}
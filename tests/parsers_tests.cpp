#include "catch2/catch.hpp"
#include "parsers.hpp"

struct basic_object_a {
    int32_t int_value = 0;
    float float_value = 0.0f;
    int32_t key_A = 0;
    std::string_view stored_text;

    void any(parsers::token_and_type left, parsers::association_type assoc, std::string_view txt, parsers::error_handler& err, int32_t line, int32_t context) {
        stored_text = txt;
    }

    void finish(int32_t) {
    }
};

/*
* exercising_combinations
    aaa       value    int    member
    bbbb      value    int    member_fn
    ccc       value    int    function   (free_int_fn)
    ddd       value    none   discard
*/

struct exercising_combinations {
    int32_t aaa = 0;
    int32_t stored_int = 0;
    int32_t free_int = 0;

    void bbbb(parsers::association_type, int32_t value, parsers::error_handler& err, int32_t line, int32_t context) {
        stored_int = value;
    }

    void finish(int32_t) {

    }
};

void free_int_fn(exercising_combinations& o, parsers::association_type, int32_t value, parsers::error_handler& err, int32_t line, int32_t context) {
    o.free_int = value;
}


#include "test_parsers_generated.hpp"


TEST_CASE("Generated Parsers Tests", "[parsers]") {
    SECTION("trivial cases") {
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
    SECTION("value exercises") {
        char file_data[] = "# comment\nddd = 1\n\tccc = 2\nbbbb != 3\n# comment 2\r\naaa = 4";

        parsers::error_handler err("no file");
        parsers::token_generator gen(file_data, file_data + strlen(file_data));

        auto created_object = parsers::parse_exercising_combinations(gen, err, 0);

        REQUIRE(created_object.free_int == 2);
        REQUIRE(created_object.stored_int == 3);
        REQUIRE(created_object.aaa == 4);
        REQUIRE(err.accumulated_errors.length() == 0);
    }
}
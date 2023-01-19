#include "catch2/catch.hpp"
#include "parsers.hpp"

struct basic_object_a {
    int32_t int_value = 0;
    float float_value = 0.0f;
    int32_t key_A = 0;
    std::string_view stored_text;
    std::string_view left_free_text;

    void any_value(std::string_view left, parsers::association_type assoc, std::string_view txt, parsers::error_handler& err, int32_t line, int32_t context) {
        stored_text = txt;
        left_free_text = left;
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


struct direct_group {
    exercising_combinations aaa;
    exercising_combinations val2;
    exercising_combinations val3;
    exercising_combinations val4;
    std::string_view left_free_text;

    void bbb(exercising_combinations value, parsers::error_handler& err, int32_t line, int32_t context) {
        val2 = value;
    }

    void any_group(std::string_view left, exercising_combinations value, parsers::error_handler& err, int32_t line, int32_t context) {
        val4 = value;
        left_free_text = left;
    }

    void finish(int32_t) {

    }
};

void free_ec_fn(direct_group& o, exercising_combinations value, parsers::error_handler& err, int32_t line, int32_t context) {
    o.val3 = value;
}


struct indirect_group {
    exercising_combinations aaa;
    exercising_combinations val2;
    exercising_combinations val3;
    exercising_combinations val4;
    std::string_view left_free_text;

    void bbb(exercising_combinations value, parsers::error_handler& err, int32_t line, int32_t context) {
        val2 = value;
    }

    void any_group(std::string_view left, exercising_combinations value, parsers::error_handler& err, int32_t line, int32_t context) {
        val4 = value;
        left_free_text = left;
    }

    void finish(int32_t) {

    }
};

void free_ec_fn(indirect_group& o, exercising_combinations value, parsers::error_handler& err, int32_t line, int32_t context) {
    o.val3 = value;
}

exercising_combinations ec_stub(parsers::token_generator& gen, parsers::error_handler& err, int32_t context);
exercising_combinations ec_sv_stub(std::string_view sv, parsers::token_generator& gen, parsers::error_handler& err, int32_t context);

struct free_value_mem_fn {
    int32_t sum = 0;
    void free_value(int32_t value, parsers::error_handler& err, int32_t line, int32_t context) {
        sum += value;
    }
    void finish(int32_t) {

    }
};

struct free_value_fn {
    int32_t sum = 0;
    void finish(int32_t) {

    }
};

void free_value(free_value_fn& obj, int32_t value, parsers::error_handler& err, int32_t line, int32_t context) {
    obj.sum += value;
}

struct free_group_g_mem_fn {
    int32_t sum = 0;
    void free_group(exercising_combinations value, parsers::error_handler& err, int32_t line, int32_t context) {
        sum += value.aaa;
    }
    void finish(int32_t) {

    }
};
struct free_group_g_mem {
    exercising_combinations free_group;
    void finish(int32_t) {

    }
};
struct free_group_g_fn {
    int32_t sum = 0;
    void finish(int32_t) {

    }
};

void free_group(free_group_g_fn& o, exercising_combinations value, parsers::error_handler& err, int32_t line, int32_t context) {
    o.sum += value.aaa;
}

struct free_group_e_mem_fn {
    int32_t sum = 0;
    void free_group(exercising_combinations value, parsers::error_handler& err, int32_t line, int32_t context) {
        sum += value.aaa;
    }
    void finish(int32_t) {

    }
};
struct free_group_e_mem {
    exercising_combinations free_group;
    void finish(int32_t) {

    }
};
struct free_group_e_fn {
    int32_t sum = 0;
    void finish(int32_t) {

    }
};

void free_group(free_group_e_fn& o, exercising_combinations value, parsers::error_handler& err, int32_t line, int32_t context) {
    o.sum += value.aaa;
}

struct basic_copy : public basic_object_a { };

#include "test_parsers_generated.hpp"

inline exercising_combinations ec_stub(parsers::token_generator& gen, parsers::error_handler& err, int32_t context) {
    auto tmp = parsers::parse_exercising_combinations(gen, err, context);
    tmp.stored_int = 5;
    return tmp;
}

inline exercising_combinations ec_sv_stub(std::string_view sv, parsers::token_generator& gen, parsers::error_handler& err, int32_t context) {
    auto tmp = parsers::parse_exercising_combinations(gen, err, context);
    tmp.stored_int = 5;
    REQUIRE(sv == "other");
    return tmp;
}


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
        REQUIRE(created_object.left_free_text == "unk_key");
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
    SECTION("group exercises") {
        char file_data[] = "aaa = { aaa = 40 } ccc = { aaa = 400 } bbb = { aaa = 4000 } other = { aaa = 4 }";

        parsers::error_handler err("no file");
        parsers::token_generator gen(file_data, file_data + strlen(file_data));

        direct_group created_object = parsers::parse_direct_group(gen, err, 0);

        REQUIRE(created_object.aaa.aaa == 40);
        REQUIRE(created_object.val2.aaa == 4000);
        REQUIRE(created_object.val3.aaa == 400);
        REQUIRE(created_object.val4.aaa == 4);
        REQUIRE(created_object.left_free_text == "other");

        REQUIRE(err.accumulated_errors.length() == 0);
    }
    SECTION("extern exercises") {
        char file_data[] = "aaa = { aaa = 40 } ccc = { aaa = 400 } bbb = { aaa = 4000 } other = { aaa = 4 }";

        parsers::error_handler err("no file");
        parsers::token_generator gen(file_data, file_data + strlen(file_data));

        auto created_object = parsers::parse_indirect_group(gen, err, 0);

        REQUIRE(created_object.aaa.aaa == 40);
        REQUIRE(created_object.aaa.stored_int == 5);
        REQUIRE(created_object.val2.aaa == 4000);
        REQUIRE(created_object.val2.stored_int == 5);
        REQUIRE(created_object.val3.aaa == 400);
        REQUIRE(created_object.val3.stored_int == 5);
        REQUIRE(created_object.val4.aaa == 4);
        REQUIRE(created_object.val4.stored_int == 5);
        REQUIRE(created_object.left_free_text == "other");

        REQUIRE(err.accumulated_errors.length() == 0);
    }
    SECTION("free value exercises") {
        {
            char file_data[] = "1 2 3";

            parsers::error_handler err("no file");
            parsers::token_generator gen(file_data, file_data + strlen(file_data));

            auto created_object = parsers::parse_free_value_mem_fn(gen, err, 0);

            REQUIRE(created_object.sum == 6);
            REQUIRE(err.accumulated_errors.length() == 0);
        }
        {
            char file_data[] = "8 2 3";

            parsers::error_handler err("no file");
            parsers::token_generator gen(file_data, file_data + strlen(file_data));

            auto created_object = parsers::parse_free_value_fn(gen, err, 0);

            REQUIRE(created_object.sum == 13);
            REQUIRE(err.accumulated_errors.length() == 0);
        }
    }
    SECTION("free group exercises") {
        {
            char file_data[] = "{ aaa = 40 } { aaa = 400 } { aaa = 4 }";
            parsers::error_handler err("no file");
            parsers::token_generator gen(file_data, file_data + strlen(file_data));

            auto created_object = parsers::parse_free_group_g_fn(gen, err, 0);

            REQUIRE(created_object.sum == 444);
            REQUIRE(err.accumulated_errors.length() == 0);
        }
        {
            char file_data[] = "{ aaa = 40 } { aaa = 400 } { aaa = 4 }";
            parsers::error_handler err("no file");
            parsers::token_generator gen(file_data, file_data + strlen(file_data));

            auto created_object = parsers::parse_free_group_g_mem_fn(gen, err, 0);

            REQUIRE(created_object.sum == 444);
            REQUIRE(err.accumulated_errors.length() == 0);
        }
        {
            char file_data[] = "{ aaa = 40 } { aaa = 400 } { aaa = 4 }";
            parsers::error_handler err("no file");
            parsers::token_generator gen(file_data, file_data + strlen(file_data));

            auto created_object = parsers::parse_free_group_g_mem(gen, err, 0);

            REQUIRE(created_object.free_group.aaa == 4);
            REQUIRE(err.accumulated_errors.length() == 0);
        }
        {
            char file_data[] = "{ aaa = 40 } { aaa = 400 } { aaa = 4 }";
            parsers::error_handler err("no file");
            parsers::token_generator gen(file_data, file_data + strlen(file_data));

            auto created_object = parsers::parse_free_group_e_fn(gen, err, 0);

            REQUIRE(created_object.sum == 444);
            REQUIRE(err.accumulated_errors.length() == 0);
        }
        {
            char file_data[] = "{ aaa = 40 } { aaa = 400 } { aaa = 4 }";
            parsers::error_handler err("no file");
            parsers::token_generator gen(file_data, file_data + strlen(file_data));

            auto created_object = parsers::parse_free_group_e_mem_fn(gen, err, 0);

            REQUIRE(created_object.sum == 444);
            REQUIRE(err.accumulated_errors.length() == 0);
        }
        {
            char file_data[] = "{ aaa = 40 } { aaa = 400 } { aaa = 4 }";
            parsers::error_handler err("no file");
            parsers::token_generator gen(file_data, file_data + strlen(file_data));

            auto created_object = parsers::parse_free_group_e_mem(gen, err, 0);

            REQUIRE(created_object.free_group.aaa == 4);
            REQUIRE(err.accumulated_errors.length() == 0);
        }
    }
    SECTION("inheritance and error test") {
        char file_data[] = "{ a b\nc }\nunk_key = free_text 11 bad_key = { 10 } key_A = 3\nkey_C = 2.5 key_B  = { 1 2 3}";

        parsers::error_handler err("no file");
        parsers::token_generator gen(file_data, file_data + strlen(file_data));

        auto created_object = parsers::parse_basic_copy(gen, err, 0);

        REQUIRE(created_object.key_A == 3);
        REQUIRE(created_object.int_value == 11);
        REQUIRE(created_object.float_value == 2.5f);
        REQUIRE(created_object.stored_text == "free_text");
        REQUIRE(created_object.left_free_text == "unk_key");
        REQUIRE(err.accumulated_errors.length() != 0);
    }
}
#include "catch2/catch.hpp"
#include "parsers.hpp"
#include <cstring>

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


TEST_CASE("Generated parsers tests", "[parsers]") {
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
        REQUIRE(err.accumulated_errors.length() == size_t(0));
    }
	SECTION("trim a float") {
		parsers::error_handler err("no file");
		auto result = parsers::parse_int(std::string_view("2.0"), 0, err);
		REQUIRE(result == 2);
		REQUIRE(err.accumulated_errors.length() == size_t(0));
	}
    SECTION("value exercises") {
        char file_data[] = "# comment\nddd = 1\n\tccc = 2\nbbbb != 3\n# comment 2\r\naaa = 4";

        parsers::error_handler err("no file");
        parsers::token_generator gen(file_data, file_data + strlen(file_data));

        auto created_object = parsers::parse_exercising_combinations(gen, err, 0);

        REQUIRE(created_object.free_int == 2);
        REQUIRE(created_object.stored_int == 3);
        REQUIRE(created_object.aaa == 4);
        REQUIRE(err.accumulated_errors.length() == size_t(0));
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

        REQUIRE(err.accumulated_errors.length() == size_t(0));
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

        REQUIRE(err.accumulated_errors.length() == size_t(0));
    }
    SECTION("free value exercises") {
        {
            char file_data[] = "1 2 3";

            parsers::error_handler err("no file");
            parsers::token_generator gen(file_data, file_data + strlen(file_data));

            auto created_object = parsers::parse_free_value_mem_fn(gen, err, 0);

            REQUIRE(created_object.sum == 6);
            REQUIRE(err.accumulated_errors.length() == size_t(0));
        }
        {
            char file_data[] = "8 2 3";

            parsers::error_handler err("no file");
            parsers::token_generator gen(file_data, file_data + strlen(file_data));

            auto created_object = parsers::parse_free_value_fn(gen, err, 0);

            REQUIRE(created_object.sum == 13);
            REQUIRE(err.accumulated_errors.length() == size_t(0));
        }
    }
    SECTION("free group exercises") {
        {
            char file_data[] = "{ aaa = 40 } { aaa = 400 } { aaa = 4 }";
            parsers::error_handler err("no file");
            parsers::token_generator gen(file_data, file_data + strlen(file_data));

            auto created_object = parsers::parse_free_group_g_fn(gen, err, 0);

            REQUIRE(created_object.sum == 444);
            REQUIRE(err.accumulated_errors.length() == size_t(0));
        }
        {
            char file_data[] = "{ aaa = 40 } { aaa = 400 } { aaa = 4 }";
            parsers::error_handler err("no file");
            parsers::token_generator gen(file_data, file_data + strlen(file_data));

            auto created_object = parsers::parse_free_group_g_mem_fn(gen, err, 0);

            REQUIRE(created_object.sum == 444);
            REQUIRE(err.accumulated_errors.length() == size_t(0));
        }
        {
            char file_data[] = "{ aaa = 40 } { aaa = 400 } { aaa = 4 }";
            parsers::error_handler err("no file");
            parsers::token_generator gen(file_data, file_data + strlen(file_data));

            auto created_object = parsers::parse_free_group_g_mem(gen, err, 0);

            REQUIRE(created_object.free_group.aaa == 4);
            REQUIRE(err.accumulated_errors.length() == size_t(0));
        }
        {
            char file_data[] = "{ aaa = 40 } { aaa = 400 } { aaa = 4 }";
            parsers::error_handler err("no file");
            parsers::token_generator gen(file_data, file_data + strlen(file_data));

            auto created_object = parsers::parse_free_group_e_fn(gen, err, 0);

            REQUIRE(created_object.sum == 444);
            REQUIRE(err.accumulated_errors.length() == size_t(0));
        }
        {
            char file_data[] = "{ aaa = 40 } { aaa = 400 } { aaa = 4 }";
            parsers::error_handler err("no file");
            parsers::token_generator gen(file_data, file_data + strlen(file_data));

            auto created_object = parsers::parse_free_group_e_mem_fn(gen, err, 0);

            REQUIRE(created_object.sum == 444);
            REQUIRE(err.accumulated_errors.length() == size_t(0));
        }
        {
            char file_data[] = "{ aaa = 40 } { aaa = 400 } { aaa = 4 }";
            parsers::error_handler err("no file");
            parsers::token_generator gen(file_data, file_data + strlen(file_data));

            auto created_object = parsers::parse_free_group_e_mem(gen, err, 0);

            REQUIRE(created_object.free_group.aaa == 4);
            REQUIRE(err.accumulated_errors.length() == size_t(0));
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
        REQUIRE(err.accumulated_errors.length() != size_t(0));
    }
}

TEST_CASE("csv parser tests", "[parsers]") {
    SECTION("parse 4 things from a csv") {
        char file_data[] = "name;1; 23; 5\r\n#name2; 2; 3; 4; 5; 6;\nname2; 2; 3; 4; 5; 6;\n\nname3;7;8;9;10";
        auto sz = strlen(file_data);
        char const* cpos = file_data;

        // NOTE: this line is unnecesasry here, but is needed for real files to discard any initial comments
        if(sz != 0 && cpos[0] == '#')
            cpos = parsers::csv_advance_to_next_line(cpos, file_data + sz);
        int32_t col2_sum = 0;
        int32_t col3_sum = 0;
        int32_t col4_sum = 0;
        std::string names;
        parsers::error_handler err("no_file");
        while(cpos < file_data + sz) {
            cpos = parsers::parse_fixed_amount_csv_values<4>(cpos, file_data + sz, ';',
                [&](std::string_view const* values) {
                    names += values[0];
                    col2_sum += parsers::parse_int(parsers::remove_surrounding_whitespace(values[1]), 0, err);
                    col3_sum += parsers::parse_int(parsers::remove_surrounding_whitespace(values[2]), 0, err);
                    col4_sum += parsers::parse_int(parsers::remove_surrounding_whitespace(values[3]), 0, err);
                });
        }

        REQUIRE(col2_sum == 10);
        REQUIRE(col3_sum == 34);
        REQUIRE(col4_sum == 18);
        REQUIRE(names == "namename2name3");
    }
    SECTION("parse 1st and 3rd things from a csv") {
        char file_data[] = "name;1; 23; 5\r\n#name2; 2; 3; 4; 5; 6;\nname2; 2; 3; 4; 5; 6;\n\nname3;7;8;9;10";
        auto sz = strlen(file_data);
        char const* cpos = file_data;

        if(sz != 0 && cpos[0] == '#')
            cpos = parsers::csv_advance_to_next_line(cpos, file_data + sz);
        int32_t col3_sum = 0;
        std::string names;
        parsers::error_handler err("no_file");
        while(cpos < file_data + sz) {
            cpos = parsers::parse_first_and_nth_csv_values(3, cpos, file_data + sz, ';',
                [&](std::string_view first, std::string_view third) {
                    names += first;
                    col3_sum += parsers::parse_int(parsers::remove_surrounding_whitespace(third), 0, err);
                });
        }

        REQUIRE(col3_sum == 34);
        REQUIRE(names == "namename2name3");
    }
}

TEST_CASE("Floating point parser tests", "[parsers]") {

    {
        float val = 0.0f;
        char const* txt = "0.0";
        REQUIRE(parsers::float_from_chars(txt, txt + 3, val) == true);
        REQUIRE(val == 0.0f);
    }
    {
        float val = 0.0f;
        char const* txt = "123";
        REQUIRE(parsers::float_from_chars(txt, txt + 3, val) == true);
        REQUIRE(val == 123.0f);
    }
    {
        float val = 0.0f;
        char const* txt = "0.25";
        REQUIRE(parsers::float_from_chars(txt, txt + 4, val) == true);
        REQUIRE(val == 0.25f);
    }
    {
        float val = 0.0f;
        char const* txt = "-10";
        REQUIRE(parsers::float_from_chars(txt, txt + 3, val) == true);
        REQUIRE(val == -10.0f);
    }
    {
        float val = 0.0f;
        char const* txt = "-1.5";
        REQUIRE(parsers::float_from_chars(txt, txt + 4, val) == true);
        REQUIRE(val == -1.5f);
    }

    {
        double val = 0.0;
        char const* txt = "0.0";
        REQUIRE(parsers::double_from_chars(txt, txt + 3, val) == true);
        REQUIRE(val == 0.0);
    }
    {
        double val = 0.0;
        char const* txt = "123";
        REQUIRE(parsers::double_from_chars(txt, txt + 3, val) == true);
        REQUIRE(val == 123.0);
    }
    {
        double val = 0.0;
        char const* txt = "0.25";
        REQUIRE(parsers::double_from_chars(txt, txt + 4, val) == true);
        REQUIRE(val == 0.25);
    }
    {
        double val = 0.0;
        char const* txt = "-10";
        REQUIRE(parsers::double_from_chars(txt, txt + 3, val) == true);
        REQUIRE(val == -10.0);
    }
    {
        double val = 0.0;
        char const* txt = "-1.5";
        REQUIRE(parsers::double_from_chars(txt, txt + 4, val) == true);
        REQUIRE(val == -1.5);
    }
}

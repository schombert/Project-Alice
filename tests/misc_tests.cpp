#include "catch.hpp"
#include "system_state.hpp"
#include "date_interface.hpp"
#include "cyto_any.hpp"

TEST_CASE("string pool tests", "[misc_tests]") {
	std::unique_ptr<sys::state> state = std::make_unique<sys::state>();

	auto a = state->add_to_pool(std::string_view("blah_blah"));
	auto b = state->add_to_pool(std::string_view("1234"));

	auto old_size_a = state->text_data.size();
	auto c = state->add_to_pool(std::string());
	REQUIRE(old_size_a == state->text_data.size());

	auto d = state->add_to_pool(std::string("last"));

	REQUIRE(state->to_string_view(a) == "blah_blah");
	REQUIRE(state->to_string_view(b) == "1234");
	REQUIRE(state->to_string_view(c).length() == size_t(0));
	REQUIRE(state->to_string_view(d) == "last");

	auto old_size = state->text_data.size();
	auto x = state->add_unique_to_pool("1234");
	REQUIRE(old_size == state->text_data.size());

	auto y = state->add_unique_to_pool("new");

	REQUIRE(state->to_string_view(x) == "1234");
	REQUIRE(state->to_string_view(y) == "new");

	auto la = state->add_to_pool_lowercase(std::string_view("MiXeD"));
	auto lb = state->add_to_pool_lowercase(std::string("LaTeX"));

	REQUIRE(state->to_string_view(la) == "mixed");
	REQUIRE(state->to_string_view(lb) == "latex");
}

TEST_CASE("date tests", "[misc_tests]") {
	sys::absolute_time_point base_time{sys::year_month_day{2020, 1, 2}};
	sys::date first{0};

	auto ymda = first.to_ymd(base_time);
	REQUIRE(ymda.year == 2020);
	REQUIRE(ymda.month == 1);
	REQUIRE(ymda.day == 2);

	sys::date start_date{sys::year_month_day{2023, 1, 16}, base_time};

	auto ymdb = start_date.to_ymd(base_time);
	REQUIRE(ymdb.year == 2023);
	REQUIRE(ymdb.month == 1);
	REQUIRE(ymdb.day == 16);

	sys::date end_date = start_date + 1673;

	auto ymdc = end_date.to_ymd(base_time);
	REQUIRE(ymdc.year == 2027);
	REQUIRE(ymdc.month == 8);
	REQUIRE(ymdc.day == 16);
}

TEST_CASE("cyto payload tests", "[misc_tests]") {
	SECTION("int_emplace") {
		Cyto::Any payload = int(64);
		REQUIRE(payload.holds_type<int>());
		REQUIRE(any_cast<int>(payload) == 64);
	}
	SECTION("same_struct_type_diff") {
		struct ctest_c1 {
			int a = 42;
			int b = 64;
		};
		struct ctest_c2 {
			int a = 42;
			int b = 64;
		};
		Cyto::Any payload = ctest_c1{};
		REQUIRE(payload.holds_type<ctest_c1>());
		REQUIRE(!payload.holds_type<ctest_c2>());
		REQUIRE(!payload.holds_type<int>());
		REQUIRE(any_cast<ctest_c1>(payload).a == 42);
		REQUIRE(any_cast<ctest_c1>(payload).b == 64);
		payload.emplace<ctest_c2>(ctest_c2{});
		REQUIRE(!payload.holds_type<ctest_c1>());
		REQUIRE(payload.holds_type<ctest_c2>());
		REQUIRE(!payload.holds_type<int>());
		REQUIRE(any_cast<ctest_c2>(payload).a == 42);
		REQUIRE(any_cast<ctest_c2>(payload).b == 64);
		payload.emplace<int>(12);
		REQUIRE(!payload.holds_type<ctest_c1>());
		REQUIRE(!payload.holds_type<ctest_c2>());
		REQUIRE(payload.holds_type<int>());
		REQUIRE(any_cast<int>(payload) == 12);
	}
	SECTION("primitives_test") {
		Cyto::Any c_payload = char(124);
		REQUIRE(c_payload.holds_type<char>());
		REQUIRE(!c_payload.holds_type<short>());
		REQUIRE(!c_payload.holds_type<int>());
		REQUIRE(!c_payload.holds_type<long>());
		REQUIRE(!c_payload.holds_type<long long>());
		REQUIRE(!c_payload.holds_type<float>());
		REQUIRE(!c_payload.holds_type<double>());
		REQUIRE(!c_payload.holds_type<void *>());
		REQUIRE(!c_payload.holds_type<char *>());
		REQUIRE(!c_payload.holds_type<short *>());
		REQUIRE(!c_payload.holds_type<int *>());
		REQUIRE(!c_payload.holds_type<long *>());
		REQUIRE(!c_payload.holds_type<long long *>());
		REQUIRE(!c_payload.holds_type<float *>());
		REQUIRE(!c_payload.holds_type<double *>());
		REQUIRE(any_cast<char>(c_payload) == 124);

		Cyto::Any s_payload = short(0xff);
		REQUIRE(!s_payload.holds_type<char>());
		REQUIRE(s_payload.holds_type<short>());
		REQUIRE(!s_payload.holds_type<int>());
		REQUIRE(!s_payload.holds_type<long>());
		REQUIRE(!s_payload.holds_type<long long>());
		REQUIRE(!s_payload.holds_type<float>());
		REQUIRE(!s_payload.holds_type<double>());
		REQUIRE(!s_payload.holds_type<void *>());
		REQUIRE(!s_payload.holds_type<char *>());
		REQUIRE(!s_payload.holds_type<short *>());
		REQUIRE(!s_payload.holds_type<int *>());
		REQUIRE(!s_payload.holds_type<long *>());
		REQUIRE(!s_payload.holds_type<long long *>());
		REQUIRE(!s_payload.holds_type<float *>());
		REQUIRE(!s_payload.holds_type<double *>());
		REQUIRE(any_cast<short>(s_payload) == 0xff);

		Cyto::Any i_payload = int(1);
		REQUIRE(!i_payload.holds_type<char>());
		REQUIRE(!i_payload.holds_type<short>());
		REQUIRE(i_payload.holds_type<int>());
		REQUIRE(!i_payload.holds_type<long>());
		REQUIRE(!i_payload.holds_type<long long>());
		REQUIRE(!i_payload.holds_type<float>());
		REQUIRE(!i_payload.holds_type<double>());
		REQUIRE(!i_payload.holds_type<void *>());
		REQUIRE(!i_payload.holds_type<char *>());
		REQUIRE(!i_payload.holds_type<short *>());
		REQUIRE(!i_payload.holds_type<int *>());
		REQUIRE(!i_payload.holds_type<long *>());
		REQUIRE(!i_payload.holds_type<long long *>());
		REQUIRE(!i_payload.holds_type<float *>());
		REQUIRE(!i_payload.holds_type<double *>());
		REQUIRE(any_cast<int>(i_payload) == 1);

		Cyto::Any l_payload = long(0xff);
		REQUIRE(!l_payload.holds_type<char>());
		REQUIRE(!l_payload.holds_type<short>());
		REQUIRE(!l_payload.holds_type<int>());
		REQUIRE(l_payload.holds_type<long>());
		REQUIRE(!l_payload.holds_type<long long>());
		REQUIRE(!l_payload.holds_type<float>());
		REQUIRE(!l_payload.holds_type<double>());
		REQUIRE(!l_payload.holds_type<void *>());
		REQUIRE(!l_payload.holds_type<char *>());
		REQUIRE(!l_payload.holds_type<short *>());
		REQUIRE(!l_payload.holds_type<int *>());
		REQUIRE(!l_payload.holds_type<long *>());
		REQUIRE(!l_payload.holds_type<long long *>());
		REQUIRE(!l_payload.holds_type<float *>());
		REQUIRE(!l_payload.holds_type<double *>());
		REQUIRE(any_cast<long>(l_payload) == 0xff);
	}

	SECTION("floating_primitives_test") {
		Cyto::Any f_payload = float(421.421f);
		REQUIRE(!f_payload.holds_type<char>());
		REQUIRE(!f_payload.holds_type<short>());
		REQUIRE(!f_payload.holds_type<int>());
		REQUIRE(!f_payload.holds_type<long>());
		REQUIRE(!f_payload.holds_type<long long>());
		REQUIRE(f_payload.holds_type<float>());
		REQUIRE(!f_payload.holds_type<double>());
		REQUIRE(!f_payload.holds_type<void *>());
		REQUIRE(!f_payload.holds_type<char *>());
		REQUIRE(!f_payload.holds_type<short *>());
		REQUIRE(!f_payload.holds_type<int *>());
		REQUIRE(!f_payload.holds_type<long *>());
		REQUIRE(!f_payload.holds_type<long long *>());
		REQUIRE(!f_payload.holds_type<float *>());
		REQUIRE(!f_payload.holds_type<double *>());
		REQUIRE(any_cast<float>(f_payload) == 421.421f);

		Cyto::Any d_payload = double(69.45f);
		REQUIRE(!d_payload.holds_type<char>());
		REQUIRE(!d_payload.holds_type<short>());
		REQUIRE(!d_payload.holds_type<int>());
		REQUIRE(!d_payload.holds_type<long>());
		REQUIRE(!d_payload.holds_type<long long>());
		REQUIRE(!d_payload.holds_type<float>());
		REQUIRE(d_payload.holds_type<double>());
		REQUIRE(!d_payload.holds_type<void *>());
		REQUIRE(!d_payload.holds_type<char *>());
		REQUIRE(!d_payload.holds_type<short *>());
		REQUIRE(!d_payload.holds_type<int *>());
		REQUIRE(!d_payload.holds_type<long *>());
		REQUIRE(!d_payload.holds_type<long long *>());
		REQUIRE(!d_payload.holds_type<float *>());
		REQUIRE(!d_payload.holds_type<double *>());
		REQUIRE(any_cast<double>(d_payload) == 69.45f);
	}

	SECTION("pointers_test") {
		Cyto::Any vp_payload = (void *)nullptr;
		REQUIRE(!vp_payload.holds_type<char>());
		REQUIRE(!vp_payload.holds_type<short>());
		REQUIRE(!vp_payload.holds_type<int>());
		REQUIRE(!vp_payload.holds_type<long>());
		REQUIRE(!vp_payload.holds_type<long long>());
		REQUIRE(!vp_payload.holds_type<float>());
		REQUIRE(!vp_payload.holds_type<double>());
		REQUIRE(vp_payload.holds_type<void *>());
		REQUIRE(!vp_payload.holds_type<char *>());
		REQUIRE(!vp_payload.holds_type<short *>());
		REQUIRE(!vp_payload.holds_type<int *>());
		REQUIRE(!vp_payload.holds_type<long *>());
		REQUIRE(!vp_payload.holds_type<long long *>());
		REQUIRE(!vp_payload.holds_type<float *>());
		REQUIRE(!vp_payload.holds_type<double *>());
		REQUIRE(any_cast<void *>(vp_payload) == (void *)nullptr);
	}
}

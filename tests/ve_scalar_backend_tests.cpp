#include "ve.hpp"

namespace {
	struct test_tag16 {
		using value_base_t = uint16_t;
		using zero_is_null_t = std::true_type;
		uint16_t value = 0;

		test_tag16() = default;
		explicit test_tag16(uint16_t v) : value(v) {}
		int32_t index() const { return int32_t(value); }
	};

	struct test_tag32 {
		using value_base_t = uint32_t;
		using zero_is_null_t = std::false_type;
		uint32_t value = 0;

		test_tag32() = default;
		explicit test_tag32(uint32_t v) : value(v) {}
		int32_t index() const { return int32_t(value); }
	};
}

TEST_CASE("ve scalar lanes and arithmetic", "[ve]") {
	ve::fp_vector fp(1.0f, 2.0f, 3.0f, 4.0f);
	ve::int_vector iv(4, 3, 2, 1);

	REQUIRE(fp[0] == 1.0f);
	REQUIRE(fp[3] == 4.0f);
	REQUIRE((fp + ve::fp_vector(1.0f))[2] == 4.0f);
	REQUIRE((fp * ve::fp_vector(2.0f))[1] == 4.0f);
	REQUIRE((iv + ve::int_vector(1))[0] == 5);
	REQUIRE((iv - ve::int_vector(1))[3] == 0);
	REQUIRE((iv * ve::int_vector(2))[2] == 4);
	REQUIRE(ve::to_float(iv)[1] == 3.0f);
	REQUIRE(ve::to_int(ve::fp_vector(1.0f, 2.0f, 3.0f, 4.0f))[2] == 3);
}

TEST_CASE("ve scalar comparisons masks and select", "[ve]") {
	auto lt = ve::fp_vector(1.0f, 5.0f, 3.0f, 8.0f) < ve::fp_vector(2.0f, 4.0f, 3.0f, 9.0f);
	REQUIRE(lt[0]);
	REQUIRE(!lt[1]);
	REQUIRE(!lt[2]);
	REQUIRE(lt[3]);

	auto mask = (ve::int_vector(1, 2, 3, 4) > ve::int_vector(0, 2, 4, 3)) | ve::mask_vector(false, true, false, false);
	REQUIRE(mask[0]);
	REQUIRE(mask[1]);
	REQUIRE(!mask[2]);
	REQUIRE(mask[3]);

	auto selected = ve::select(mask, ve::fp_vector(10.0f), ve::fp_vector(1.0f));
	REQUIRE(selected[0] == 10.0f);
	REQUIRE(selected[1] == 10.0f);
	REQUIRE(selected[2] == 1.0f);
	REQUIRE(selected[3] == 10.0f);

	ve::vbitfield_type bits{ 0b0101 };
	ve::mask_vector combined = !bits && ve::mask_vector(true, true, false, true);
	REQUIRE(!combined[0]);
	REQUIRE(combined[1]);
	REQUIRE(!combined[2]);
	REQUIRE(combined[3]);

	ve::mask_vector from_bool = true;
	REQUIRE(from_bool[0]);
	REQUIRE(from_bool[1]);
	REQUIRE(from_bool[2]);
	REQUIRE(from_bool[3]);

	auto eq_true = bits == true;
	REQUIRE(eq_true[0]);
	REQUIRE(!eq_true[1]);
	REQUIRE(eq_true[2]);
	REQUIRE(!eq_true[3]);
}

TEST_CASE("ve scalar contiguous unaligned and partial load store", "[ve]") {
	alignas(16) float floats[8] = { 0.f, 1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f };
	alignas(16) int32_t ints[8] = { 10, 11, 12, 13, 14, 15, 16, 17 };

	auto f_aligned = ve::load(ve::contiguous_tags<int32_t>(0), floats);
	auto f_unaligned = ve::load(ve::unaligned_contiguous_tags<int32_t>(1), floats);
	auto i_partial = ve::load(ve::partial_contiguous_tags<int32_t>(2, 2), ints);

	REQUIRE(f_aligned[0] == 0.f);
	REQUIRE(f_aligned[3] == 3.f);
	REQUIRE(f_unaligned[0] == 1.f);
	REQUIRE(f_unaligned[3] == 4.f);
	REQUIRE(i_partial[0] == 12);
	REQUIRE(i_partial[1] == 13);
	REQUIRE(i_partial[2] == 0);
	REQUIRE(i_partial[3] == 0);

	float out_floats[8] = {};
	int32_t out_ints[8] = {};
	ve::store(ve::contiguous_tags<int32_t>(0), out_floats, f_aligned);
	ve::store(ve::partial_contiguous_tags<int32_t>(3, 2), out_ints, ve::int_vector(100, 101, 102, 103));

	REQUIRE(out_floats[0] == 0.f);
	REQUIRE(out_floats[3] == 3.f);
	REQUIRE(out_ints[3] == 100);
	REQUIRE(out_ints[4] == 101);
	REQUIRE(out_ints[5] == 0);
}

TEST_CASE("ve scalar tagged vectors apply reduce and bitfields", "[ve]") {
	test_tag16 tag_data[4] = { test_tag16(0), test_tag16(2), test_tag16(3), test_tag16(4) };
	auto tags = ve::load(ve::contiguous_tags<int32_t>(0), tag_data);

	REQUIRE(tags[0].index() == 0);
	REQUIRE(tags[1].index() == 2);
	REQUIRE(tags[3].index() == 4);
	REQUIRE(ve::is_invalid(ve::tagged_vector<test_tag16>())[0]);

	auto sum = ve::apply([](float a, int32_t b) { return a + float(b); }, ve::fp_vector(1.f, 2.f, 3.f, 4.f), ve::int_vector(4, 3, 2, 1));
	REQUIRE(sum[0] == 5.f);
	REQUIRE(sum[3] == 5.f);
	REQUIRE(ve::fp_vector(1.f, 2.f, 3.f, 4.f).reduce() == 10.f);

	dcon::bitfield_type bits[2] = {};
	ve::store(ve::contiguous_tags<int32_t>(0), bits, ve::mask_vector(true, false, true, true));
	auto loaded_bits = ve::load(ve::contiguous_tags<int32_t>(0), bits);
	REQUIRE((loaded_bits.v & 0x01) != 0);
	REQUIRE((loaded_bits.v & 0x02) == 0);
	REQUIRE((loaded_bits.v & 0x04) != 0);
	REQUIRE((loaded_bits.v & 0x08) != 0);
}

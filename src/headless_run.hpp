#pragma once
// -headless -years N: runs N game years with no speed cap, prints seconds per game year, and with -dump DIR
// writes monthly CSVs (nations.csv, prices.csv, provinces.csv) on the 1st of each month.
#include <chrono>
#include <cstdio>
#include <filesystem>
#include <string>

namespace headless {

inline std::string ymd_str(sys::year_month_day d) {
	char buf[16];
	snprintf(buf, sizeof(buf), "%04d-%02d-%02d", d.year, int(d.month), int(d.day));
	return std::string(buf);
}
inline std::string ymd_of(sys::state& state) {
	return ymd_str(state.current_date.to_ymd(state.start_date));
}

inline std::string tag_of(sys::state& state, dcon::nation_id n) {
	if(!n)
		return "";
	return nations::int_to_tag(
			state.world.national_identity_get_identifying_int(state.world.nation_get_identity_from_identity_holder(n)));
}

inline void dump_month(sys::state& state, FILE* nations_csv, FILE* prices_csv, FILE* provinces_csv, std::string const& date) {
	// province index is Alice's own (land provinces first), not the Vic2 definition.csv number
	for(int32_t i = 0; i < state.province_definitions.first_sea_province.index(); ++i) {
		dcon::province_id p{dcon::province_id::value_base_t(i)};
		fprintf(provinces_csv, "%s,%d,%s,%s,%.6g\n", date.c_str(), i,
				tag_of(state, state.world.province_get_nation_from_province_ownership(p)).c_str(),
				tag_of(state, state.world.province_get_nation_from_province_control(p)).c_str(),
				state.world.province_get_demographics(p, demographics::total));
	}
	state.world.for_each_nation([&](dcon::nation_id n) {
		if(state.world.nation_get_owned_province_count(n) == 0)
			return;
		fprintf(nations_csv, "%s,%s,%.6g,%.6g,%.6g,%d,%d,%d,%.6g,%.6g,%d,%.6g,%.6g\n", date.c_str(), tag_of(state, n).c_str(),
				state.world.nation_get_prestige(n), float(state.world.nation_get_industrial_score(n)),
				float(state.world.nation_get_military_score(n)), int(state.world.nation_get_rank(n)),
				int(state.world.nation_get_is_great_power(n)), int(state.world.nation_get_is_civilized(n)),
				state.world.nation_get_stockpiles(n, economy::money), state.world.nation_get_demographics(n, demographics::total),
				int(state.world.nation_get_owned_province_count(n)), state.world.nation_get_infamy(n),
				state.world.nation_get_war_exhaustion(n));
	});
	state.world.for_each_commodity([&](dcon::commodity_id c) {
		if(c == economy::money)
			return;
		float supply = 0.f;
		state.world.for_each_market([&](dcon::market_id m) { supply += state.world.market_get_supply(m, c); });
		// price = supply-weighted mean over the per-state markets; median_price = median over markets
		fprintf(prices_csv, "%s,%s,%.6g,%.6g,%.6g\n", date.c_str(),
				text::produce_simple_string(state, state.world.commodity_get_name(c)).c_str(), economy::price(state, c),
				economy::median_price(state, c), supply);
	});
}

inline void run(sys::state& state, int years, std::string const& dump_dir) {
	using clock = std::chrono::steady_clock;
	FILE* nations_csv = nullptr;
	FILE* prices_csv = nullptr;
	FILE* provinces_csv = nullptr;
	if(!dump_dir.empty()) {
		std::filesystem::create_directories(dump_dir);
		nations_csv = fopen((dump_dir + "/nations.csv").c_str(), "w");
		prices_csv = fopen((dump_dir + "/prices.csv").c_str(), "w");
		provinces_csv = fopen((dump_dir + "/provinces.csv").c_str(), "w");
		fprintf(provinces_csv, "date,province,owner,controller,population\n");
		fprintf(nations_csv, "date,tag,prestige,industrial_score,military_score,rank,is_gp,is_civilized,treasury,population,provinces,infamy,war_exhaustion\n");
		fprintf(prices_csv, "date,good,price,median_price,world_supply\n");
	}

	auto start = state.current_date.to_ymd(state.start_date);
	int const end_year = start.year + years;
	int year = start.year;
	printf("HEADLESS start=%s years=%d seed=%u pops=%u\n", ymd_str(start).c_str(), years, state.game_seed, state.world.pop_size());
	fflush(stdout);
	if(nations_csv)
		dump_month(state, nations_csv, prices_csv, provinces_csv, ymd_str(start));

	state.user_settings.autosaves = sys::autosave_frequency::none; // parallel runs would share one autosave slot set
	auto t_start = clock::now();
	auto t_year = t_start;
	while(years > 0) {
		command::execute_pending_commands(state);
		state.single_game_tick();
		auto ymd = state.current_date.to_ymd(state.start_date);
		if(ymd.day == 1 && nations_csv) {
			dump_month(state, nations_csv, prices_csv, provinces_csv, ymd_str(ymd));
			fflush(nations_csv);
			fflush(prices_csv);
			fflush(provinces_csv);
		}
		if(ymd.year != year) {
			auto now = clock::now();
			printf("YEAR %d secs=%.3f pops=%u wars=%u\n", year, std::chrono::duration<double>(now - t_year).count(),
					state.world.pop_size(), state.world.war_size());
			fflush(stdout);
			t_year = now;
			year = ymd.year;
		}
		if(ymd.year >= end_year)
			break;
	}
	printf("TOTAL years=%d secs=%.3f\n", years, std::chrono::duration<double>(clock::now() - t_start).count());
	fflush(stdout);
	if(nations_csv) {
		fclose(nations_csv);
		fclose(prices_csv);
		fclose(provinces_csv);
	}
}

} // namespace headless

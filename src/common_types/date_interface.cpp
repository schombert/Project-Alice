#include "date_interface.hpp"

namespace sys {

// from http://howardhinnant.github.io/date_algorithms.html

constexpr int days_from_civil(int y, unsigned m, unsigned d) noexcept {
	y -= m <= 2;
	int const era = (y >= 0 ? y : y - 399) / 400;
	unsigned const yoe = static_cast<unsigned>(y - era * 400);			  // [0, 399]
	unsigned const doy = (153 * (m > 2 ? m - 3 : m + 9) + 2) / 5 + d - 1; // [0, 365]
	unsigned const doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;			  // [0, 146096]
	return era * 146097 + static_cast<int>(doe) - 719468;
}

constexpr year_month_day civil_from_days(int64_t z) noexcept {
	z += 719468;
	const int32_t era = int32_t((z >= 0 ? z : z - 146096) / 146097);
	unsigned const doe = static_cast<unsigned>(z - era * 146097);				// [0, 146096]
	unsigned const yoe = (doe - doe / 1460 + doe / 36524 - doe / 146096) / 365; // [0, 399]
	const int32_t y = static_cast<int32_t>(yoe) + era * 400;
	unsigned const doy = doe - (365 * yoe + yoe / 4 - yoe / 100); // [0, 365]
	unsigned const mp = (5 * doy + 2) / 153;					  // [0, 11]
	unsigned const d = doy - (153 * mp + 2) / 5 + 1;			  // [1, 31]
	unsigned const m = mp < 10 ? mp + 3 : mp - 9;				  // [1, 12]
	return year_month_day{int32_t(y + (m <= 2)), uint16_t(m), uint16_t(d)};
}

date::date(year_month_day const& v, absolute_time_point base) noexcept {
	auto difference = days_from_civil(v.year, v.month, v.day) - base.to_days();
	difference = std::clamp(difference, int64_t(0), int64_t(std::numeric_limits<uint16_t>::max()) - 1);
	value = date::value_base_t(1 + difference);
}

year_month_day date::to_ymd(absolute_time_point base) const noexcept {
	int64_t count = base.to_days() + (value - 1);
	return civil_from_days(count);
}

absolute_time_point::absolute_time_point(year_month_day const& d) { days = days_from_civil(d.year, d.month, d.day); }

bool is_playable_date(date d, absolute_time_point start, absolute_time_point end) {
	return bool(d) && start.to_days() + (d.to_raw_value() - 1) < end.to_days();
}

int32_t days_difference(year_month_day start, year_month_day end) {
	return days_from_civil(end.year, end.month, end.day) - days_from_civil(start.year, start.month, start.day);
}

} // namespace sys

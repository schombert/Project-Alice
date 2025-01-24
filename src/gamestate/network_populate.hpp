
namespace command {

struct province_ui_data {
	dcon::commodity_id rgo;
	ankerl::unordered_dense::map<uint8_t, uint8_t> building_level;
	bool is_slave;
	bool naval_rally_point;
	bool land_rally_point;
	bool is_colonial;
	dcon::crime_id crime;
	ankerl::unordered_dense::map<dcon::ideology_id, float>	party_loyalty;
	ankerl::unordered_dense::map<dcon::provincial_modifier_value, float> modifier_values;
	ankerl::unordered_dense::set<sys::dated_modifier> current_modifiers;
	float nationalism;
	ankerl::unordered_dense::map<dcon::demographics_key, float> demographics;
	ankerl::unordered_dense::map<dcon::demographics_key, float> demographics_alt;
	dcon::culture_id dominant_culture;
	dcon::culture_id dominant_accepted_culture;
	dcon::culture_id dominant_religion;
	dcon::culture_id dominant_ideology;
	dcon::issue_option_id dominant_issue_option;
	sys::date last_control_change;
	sys::date last_immigration;
	bool is_owner_core;

	float rgo_full_profit;
	float rgo_employment;
	float rgo_size;
	float subsistence_score;

	ankerl::unordered_dense::map<dcon::commodity_id, float> rgo_max_size_per_good;
	ankerl::unordered_dense::map<dcon::commodity_id, float> rgo_target_employment_per_good;
	ankerl::unordered_dense::map<dcon::commodity_id, float> rgo_employment_per_good;

	float landowners_share;
	float capitalists_share;
	float subsistence_employment;

	ankerl::unordered_dense::map<dcon::commodity_id, float> rgo_profit_per_good;
	ankerl::unordered_dense::map<dcon::commodity_id, float> rgo_actual_production_per_good;

	float daily_net_migration;
	float daily_net_immigration;

	float siege_progress;

	bool is_blockaded;

	dcon::nation_id former_controller;
	dcon::rebel_faction_id former_rebel_controller;

	province_ui_data{}
	~province_ui_data{}
};

	struct network_populate_data {
		union utype {
			province_ui_data province_ui;

			utype() { }
			~utype() { }
		} data;
	};
}

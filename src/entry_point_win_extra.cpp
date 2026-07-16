#ifndef UNICODE
#define UNICODE
#endif
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN

#include "system_state.hpp"
#include "economy.hpp"
#include "game_scene.hpp"
#include "serialization.hpp"
#include "parsers_declarations.hpp"

#include <Windows.h>
#include <shellapi.h>
#include "Objbase.h"
#include "window.hpp"

#include "network/webapi/controllers.hpp"

#pragma comment(lib, "Ole32.lib")
#pragma comment(lib, "Shell32.lib")
#pragma comment(lib, "icu.lib")

static sys::state game_state; // too big for the stack
static CRITICAL_SECTION guard_abort_handler;

void signal_abort_handler(int) {
	static bool run_once = false;

	EnterCriticalSection(&guard_abort_handler);
	if(run_once == false) {
		run_once = true;

		STARTUPINFO si;
		ZeroMemory(&si, sizeof(si));
		si.cb = sizeof(si);
		PROCESS_INFORMATION pi;
		ZeroMemory(&pi, sizeof(pi));
		// Start the child process.
		if(CreateProcessW(
			L"dbg_alice.exe",   // Module name
			NULL, // Command line
			NULL, // Process handle not inheritable
			NULL, // Thread handle not inheritable
			FALSE, // Set handle inheritance to FALSE
			0, // No creation flags
			NULL, // Use parent's environment block
			NULL, // Use parent's starting directory 
			&si, // Pointer to STARTUPINFO structure
			&pi) == 0) {

			// create process failed
			LeaveCriticalSection(&guard_abort_handler);
			return;

		}
		// Wait until child process exits.
		WaitForSingleObject(pi.hProcess, INFINITE);
		// Close process and thread handles. 
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
	}
	LeaveCriticalSection(&guard_abort_handler);
}

LONG WINAPI uef_wrapper(struct _EXCEPTION_POINTERS* lpTopLevelExceptionFilter) {
	signal_abort_handler(0);
	return EXCEPTION_CONTINUE_SEARCH;
}

void generic_wrapper() {
	signal_abort_handler(0);
}
void invalid_parameter_wrapper(
   const wchar_t* expression,
   const wchar_t* function,
   const wchar_t* file,
   unsigned int line,
   uintptr_t pReserved
) {
	signal_abort_handler(0);
}

void EnableCrashingOnCrashes() {
	typedef BOOL(WINAPI* tGetPolicy)(LPDWORD lpFlags);
	typedef BOOL(WINAPI* tSetPolicy)(DWORD dwFlags);
	const DWORD EXCEPTION_SWALLOWING = 0x1;

	HMODULE kernel32 = LoadLibraryA("kernel32.dll");
	if(kernel32) {
		tGetPolicy pGetPolicy = (tGetPolicy)GetProcAddress(kernel32, "GetProcessUserModeExceptionPolicy");
		tSetPolicy pSetPolicy = (tSetPolicy)GetProcAddress(kernel32, "SetProcessUserModeExceptionPolicy");
		if(pGetPolicy && pSetPolicy) {
			DWORD dwFlags;
			if(pGetPolicy(&dwFlags)) {
				// Turn off the filter
				pSetPolicy(dwFlags & ~EXCEPTION_SWALLOWING);
			}
		}
	}
	BOOL insanity = FALSE;
	SetUserObjectInformationA(GetCurrentProcess(), UOI_TIMERPROC_EXCEPTION_SUPPRESSION, &insanity, sizeof(insanity));
}

enum run_mode {
	economy_tick,
	battles_data
};


int WINAPI wWinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, LPWSTR /*commandline*/, int /*nCmdShow*/
) {

	if(SUCCEEDED(CoInitializeEx(NULL, COINIT_APARTMENTTHREADED))) {
		// do everything here: create a window, read messages

		int num_params = 0;
		auto parsed_cmd = CommandLineToArgvW(GetCommandLineW(), &num_params);
		if(num_params < 2) return 0;
		run_mode mode = run_mode::economy_tick;
		if(num_params >= 2) {			
			if(native_string(parsed_cmd[2]) == NATIVE("economy_tick")) {
				mode = run_mode::economy_tick;				
			} else if(native_string(parsed_cmd[2]) == NATIVE("battles_data")) {
				mode = run_mode::battles_data;
			}
		}
		LocalFree(parsed_cmd);


		// to make copying the code easier
		auto& state = game_state;

		if (mode == run_mode::economy_tick) {
			// scenario loading functions (would have to run these even when scenario is pre-built)
			if(sys::try_read_scenario_and_save_file(game_state, parsed_cmd[1])) {
				game_state.fill_unsaved_data();
			} else {
				return 0;
			}
			game_state.load_user_settings();
			ui::populate_definitions_map(game_state);
			network::init(game_state);

			game_state.single_game_tick();
			for(int i = 0; i < 1000; i++) {
				economy::update_employment(game_state, false, 1.f);
				economy::daily_update(game_state, false, 1.f);
			}
			network::finish(game_state, true);
		}

		if(mode == run_mode::battles_data) {
			std::string file_name = "battle_data_train.csv";
			auto pf = fopen(file_name.c_str(), "w");

			// assume armies of size 30 for now
			const int army_size = 30;

			for(int i = 0; i < army_size * 2;i++) {
				fprintf(pf, "a%d,d%d,", i, i);
			}
			fprintf(pf, "gen_a_att,gen_b_att,gen_a_def,gen_b_def,gen_a_prestige,gen_b_prestige,tactic_a,tactic_b,prov_def,crossing,digin,width,time,result\n");

			auto test_location = state.world.create_province();

			state.world.nation_resize_unit_stats(army_size * 2);
			state.world.province_resize_modifier_values(sys::provincial_mod_offsets::count);
			state.world.nation_resize_modifier_values(sys::national_mod_offsets::count);

			for(int attack = 0; attack < 8; attack++) {
				for(int defense = 0; defense < 8; defense++) {
					auto trait = state.world.create_leader_trait();
					state.world.leader_trait_set_attack(trait, attack);
					state.world.leader_trait_set_defense(trait, defense);
				}
			}

			auto empty_leader_background = state.world.create_leader_trait();

			for(int battle_simulation = 0; battle_simulation < 1000000; battle_simulation++) {
				state.current_date= sys::date{ 0 };

				auto attacker = state.world.create_nation();

				auto attacker_soldier_pop = state.world.create_pop();
				state.world.pop_set_size(attacker_soldier_pop, 1000000.f);
				auto defender_soldier_pop = state.world.create_pop();
				state.world.pop_set_size(defender_soldier_pop, 1000000.f);

				auto defender = state.world.create_nation();

				auto start_rval = rng::get_random_pair(state, battle_simulation);

				// 64b
				uint64_t ent = start_rval.high;

				int starting_combat_width = 5 + ent % (military::max_combat_width - 5);
				ent = ent >> 6;
				// 64 - 6 = 58 b

				auto crossing = military::crossing_type(ent % 3);
				ent = ent >> 2;
				// 58 - 2 = 56 b

				auto starting_dig_in = (uint8_t)(ent % 8);
				ent = ent >> 3;
				// 56 - 3 = 53 b

				auto defense_bonus = (uint8_t)(ent % 8);
				ent = ent >> 3;
				// 53 - 3 = 50 b

				auto attacker_trait = dcon::leader_trait_id{ dcon::leader_trait_id::value_base_t(ent % 64)};
				ent = ent >> 6;
				// 50 - 6 = 44 b

				auto defender_trait = dcon::leader_trait_id{ dcon::leader_trait_id::value_base_t(ent % 64)};
				ent = ent >> 6;
				// 44 - 6 = 38 b

				auto attacker_prestige = float(ent % 64) / 64.f;
				ent = ent >> 6;
				auto defender_prestige = float(ent % 64) / 64.f;
				ent = ent >> 6;
				// 38 - 12 = 26 b

				auto attacker_units = std::min((int)(1 + (ent % 32)), army_size);
				ent = ent >> 5;
				auto defender_units = std::min((int)(1 + (ent % 32)), army_size);
				ent = ent >> 5;
				// 26 - 10 = 16 b

				auto attacker_general = fatten(state.world, state.world.create_leader());
				{
					auto l = attacker_general;
					l.set_is_admiral(false);
					l.set_personality(attacker_trait);
					l.set_background(empty_leader_background);
					l.set_since(state.current_date);
					l.set_prestige(attacker_prestige);
					state.world.try_create_leader_loyalty(attacker, l);
				}
				auto defender_general = fatten(state.world, state.world.create_leader());
				{
					auto l = defender_general;
					l.set_is_admiral(false);
					l.set_personality(defender_trait);
					l.set_background(empty_leader_background);
					l.set_since(state.current_date);
					l.set_prestige(defender_prestige);
					state.world.try_create_leader_loyalty(defender, l);
				}

				state.world.province_set_modifier_values(test_location, sys::provincial_mod_offsets::defense, defense_bonus);

				auto tactic_attacker = float(rng::reduce(rng::get_random(state, battle_simulation ^ (1 << 31)), 300)) / 100.f;
				auto tactic_defender = float(rng::reduce(rng::get_random(state, battle_simulation ^ (1 << 30)), 300)) / 100.f;

				state.world.nation_set_modifier_values(attacker, sys::national_mod_offsets::military_tactics, tactic_attacker);
				state.world.nation_set_modifier_values(defender, sys::national_mod_offsets::military_tactics, tactic_defender);

				auto test_war = state.world.create_war();
				auto attacker_participant = state.world.force_create_war_participant(test_war, attacker);
				auto defender_participant = state.world.force_create_war_participant(test_war, defender);
				state.world.war_participant_set_is_attacker(attacker_participant, true);


				auto new_battle = fatten(game_state.world, game_state.world.create_land_battle());
				new_battle.set_war_attacker_is_attacker(true);
				new_battle.set_start_date(game_state.current_date);
				new_battle.set_war_from_land_battle_in_war(test_war);
				new_battle.set_location_from_land_battle_location(test_location);
				new_battle.set_dice_rolls(military::make_dice_rolls(state, uint32_t(new_battle.id.value)));
				new_battle.set_combat_width(starting_combat_width);


				std::vector<dcon::regiment_id> clean_up_vector {};

				state.military_definitions.unit_base_definitions.resize(army_size * 2);
				for(int i = 0; i < army_size * 2; i++) {
					auto regiment_type = dcon::unit_type_id{ (dcon::unit_type_id::value_base_t) i };
					auto& data = state.military_definitions.unit_base_definitions[regiment_type];

					data.build_time = 100;
					data.maximum_speed = 1.f;
					data.default_organisation = 10;

					data.defence_or_hull = float(rng::reduce(rng::get_random(state, battle_simulation^i + (1 << 10)) , 3000)) / 100.f;
					data.attack_or_gun_power = float(rng::reduce(rng::get_random(state, battle_simulation ^ i + (1 << 11)), 3000)) / 100.f;
					data.supply_consumption = float(rng::reduce(rng::get_random(state, battle_simulation ^ i + (1 << 12)), 3000)) / 100.f;
					data.support = float(rng::reduce(rng::get_random(state, battle_simulation ^ i + (1 << 13)), 3000)) / 100.f;
					data.siege_or_torpedo_attack = float(rng::reduce(rng::get_random(state, battle_simulation ^ i + (1 << 14)), 3000)) / 100.f;
					data.reconnaissance_or_fire_range = float(rng::reduce(rng::get_random(state, battle_simulation ^ i + (1 << 15)), 3000)) / 100.f;
					data.discipline_or_evasion = float(rng::reduce(rng::get_random(state, battle_simulation ^ i + (1 << 16)), 3000)) / 100.f;
					data.maneuver = float(rng::reduce(rng::get_random(state, battle_simulation ^ i + (1 << 17)), 3000)) / 100.f;

					data.is_land = true;

					auto type_data = rng::reduce(rng::get_random(state, battle_simulation ^ i + (1 << 18)), 3);

					if(type_data == 0) {
						data.type = military::unit_type::cavalry;
					} else if(type_data == 1) {
						data.type = military::unit_type::support;
					} else {
						data.type = military::unit_type::infantry;
					}

					state.world.nation_set_unit_stats(attacker, regiment_type, data);
					state.world.nation_set_unit_stats(defender, regiment_type, data);
				}

				// attacker army
				auto attacker_army = state.world.create_army();
				state.world.army_set_general_from_army_leadership(attacker_army, attacker_general);
				state.world.army_set_controller_from_army_control(attacker_army, attacker);
				state.world.army_set_location_from_army_location(attacker_army, test_location);
				for(int i = 0; i < attacker_units; i++) {
					auto regiment_type = dcon::unit_type_id { (dcon::unit_type_id::value_base_t) i};
					auto regiment = fatten(state.world, state.world.create_regiment());
					regiment.set_type(regiment_type);
					auto rvals = rng::get_random_pair(state, battle_simulation ^ i);
					auto exp = (float)(rvals.low % (100)) / 100.f;
					regiment.set_experience(std::clamp(exp, 0.f, 1.f));
					regiment.set_strength(1.f);
					regiment.set_org(1.f);

					state.world.force_create_army_membership(regiment, attacker_army);
					state.world.force_create_regiment_source(regiment, attacker_soldier_pop);
					clean_up_vector.push_back(regiment);
				}

				// defender army
				auto defender_army = state.world.create_army();
				state.world.army_set_general_from_army_leadership(defender_army, defender_general);
				state.world.army_set_controller_from_army_control(defender_army, defender);
				state.world.army_set_location_from_army_location(defender_army, test_location);
				state.world.army_set_dig_in(defender_army, starting_dig_in);
				for(int i = 0; i < defender_units; i++) {
					auto regiment_type = dcon::unit_type_id{ (dcon::unit_type_id::value_base_t)(i + army_size) };
					auto regiment = fatten(state.world, state.world.create_regiment());
					regiment.set_type(regiment_type);
					auto rvals = rng::get_random_pair(state, battle_simulation ^ (i + army_size));
					auto exp = (float)(rvals.low % (100)) / 100.f;
					regiment.set_experience(std::clamp(exp, 0.f, 1.f));
					regiment.set_strength(1.f);
					regiment.set_org(1.f);

					state.world.force_create_army_membership(regiment, defender_army);
					state.world.force_create_regiment_source(regiment, defender_soldier_pop);
					clean_up_vector.push_back(regiment);
				}

				uint16_t crossing_flag =0;

				switch(crossing) {
				case military::crossing_type::none:
					crossing_flag = military::battle_regiment::crossing_none;
					break;
				case military::crossing_type::river:
					crossing_flag =  military::battle_regiment::crossing_river;
					break;
				case military::crossing_type::sea:
					crossing_flag =  military::battle_regiment::crossing_strait;
					break;
				}

				// add armies to battle

				{
					auto a = attacker_army;
					auto reserves = state.world.land_battle_get_reserves(new_battle);
					for(auto reg : state.world.army_get_army_membership(a)) {
						auto type = state.military_definitions.unit_base_definitions[reg.get_regiment().get_type()].type;
						military::battle_regiment data{
							reg.get_regiment().id,
							true,
							military::battle_regiment::type_infantry,
							crossing_flag,
							0
						};
						if(type == military::unit_type::cavalry) {
							data.flags = data.flags ^ military::battle_regiment::type_infantry ^ military::battle_regiment::type_cavalry;
						}
						if(type == military::unit_type::special || type ==military::unit_type::support) {
							data.flags = data.flags ^ military::battle_regiment::type_infantry ^ military::battle_regiment::type_support;
						}
						reserves.push_back(data);
					}
				}
				{
					auto a = defender_army;
					auto reserves = state.world.land_battle_get_reserves(new_battle);
					for(auto reg : state.world.army_get_army_membership(a)) {
						auto type = state.military_definitions.unit_base_definitions[reg.get_regiment().get_type()].type;
						military::battle_regiment data{
							reg.get_regiment().id,
							false,
							military::battle_regiment::type_infantry,
							crossing_flag,
							starting_dig_in
						};
						if(type == military::unit_type::cavalry) {
							data.flags = data.flags ^ military::battle_regiment::type_infantry ^ military::battle_regiment::type_cavalry;
						}
						if(type == military::unit_type::special || type ==military::unit_type::support) {
							data.flags = data.flags ^ military::battle_regiment::type_infantry ^ military::battle_regiment::type_support;
						}
						reserves.push_back(data);
					}
				}
				state.world.army_set_battle_from_army_battle_participation(attacker_army, new_battle);
				state.world.army_set_battle_from_army_battle_participation(defender_army, new_battle);

				military::update_battle_leaders(state, new_battle);

				// run battle for two years:

				int battle_length = 0;
				int battle_status = 0;

				for(int day = 0; day < 365 * 2; day++) {
					auto index = new_battle.id.index();

					if((day - state.world.land_battle_get_start_date(new_battle).value) % 5 == 4) {
						uint8_t new_dice = military::make_dice_rolls(state, uint32_t(day));
						state.world.land_battle_set_dice_rolls(new_battle, new_dice);
						auto attacker_dice = new_dice & 0x0F;
						auto defender_dice = (new_dice >> 4) & 0x0F;
						// if the attacker rolls higher than the defender, remove 1 level of dig-in from each defender army, and each defender unit.
						if(attacker_dice > defender_dice) {
							if (state.world.army_get_dig_in(defender_army) > 0) {
								state.world.army_set_dig_in(defender_army, state.world.army_get_dig_in(defender_army) - 1);
							}
							auto& def_front = state.world.land_battle_get_defender_front_line(new_battle);
							auto& def_back = state.world.land_battle_get_defender_back_line(new_battle);
							auto reserves = state.world.land_battle_get_reserves(new_battle);
							auto combat_width = state.world.land_battle_get_combat_width(new_battle);
							for(uint8_t i = 0; i < combat_width; ++i) {
								auto def_front_dig_in = def_front[i].get_dig_in();
								auto def_back_dig_in = def_back[i].get_dig_in();
								if(def_front[i].regiment && def_front_dig_in > 0) {
									def_front[i].set_dig_in(def_front_dig_in - 1);
								}
								if(def_back[i].regiment && def_back_dig_in > 0) {
									def_back[i].set_dig_in(def_back_dig_in - 1);
								}
							}
							for(auto& bat_reg : reserves) {
								auto dig_in = bat_reg.get_dig_in();
								if(!bat_reg.get_is_attacking() && dig_in > 0) {
									bat_reg.set_dig_in(dig_in - 1);
								}
							}
						}			
					}

					military::land_battle_process_line_damage(state, new_battle);
					military::land_battle_clear_dead_regiments_from_battle_slots(state, new_battle);
					military::land_battle_compact_battle_slots(state, new_battle);
					military::land_battle_deploy_reserves_to_battle_slots(state, new_battle);

					auto& def_front = state.world.land_battle_get_defender_front_line(new_battle);
					auto& att_front = state.world.land_battle_get_attacker_front_line(new_battle);

					if(!def_front[0].regiment) {
						battle_length = day;
						battle_status = 1;
						break;
					}
					if(!att_front[0].regiment) {
						battle_length = day;
						battle_status = -1;
						break;
					}
				}



				for(int i = 0; i < attacker_units; i++) {
					auto regiment_type = dcon::unit_type_id{ (dcon::unit_type_id::value_base_t)i };
					auto& data = state.military_definitions.unit_base_definitions[regiment_type];
					fprintf(pf, "%f,%f,", data.attack_or_gun_power, data.defence_or_hull);
				}
				for(int i = attacker_units; i < army_size; i++) {
					fprintf(pf, "%f,%f,", 0.f, 0.f);
				}
				for(int i = army_size; i < army_size + defender_units; i++) {
					auto regiment_type = dcon::unit_type_id{ (dcon::unit_type_id::value_base_t)i };
					auto& data = state.military_definitions.unit_base_definitions[regiment_type];
					fprintf(pf, "%f,%f,", data.attack_or_gun_power, data.defence_or_hull);
				}
				for(int i = army_size + defender_units; i < army_size * 2; i++) {
					fprintf(pf, "%f,%f,", 0.f, 0.f);
				}
				fprintf(pf, "%f,%f,%f,%f,%f,%f,",
					state.world.leader_trait_get_attack(attacker_trait), state.world.leader_trait_get_attack(defender_trait),
					state.world.leader_trait_get_defense(attacker_trait), state.world.leader_trait_get_defense(defender_trait),
					attacker_prestige, defender_prestige
				);
				fprintf(pf, "%f,%f,%d,%d,%d,%d,%d,%d\n",
					tactic_attacker, tactic_defender,
					(int)defense_bonus, (int)crossing, starting_dig_in,
					starting_combat_width, battle_length, battle_status
				);

				state.world.delete_land_battle(new_battle);
				state.world.delete_nation(defender);
				state.world.delete_nation(attacker);
				state.world.delete_war(test_war);
				for(auto item : clean_up_vector) {
					state.world.delete_regiment(item);
				}
				state.world.delete_pop(defender_soldier_pop);
				state.world.delete_pop(attacker_soldier_pop);
				state.world.delete_army(defender_army);
				state.world.delete_army(attacker_army);
				state.world.delete_leader(defender_general);
				state.world.delete_leader(attacker_general);
			}

			fflush(pf);
			fclose(pf);
		}

		CoUninitialize();
	}
	return 0;
}

#pragma once

#include <vector>

std::vector<uint32_t> naval_map_from(sys::state& state) {
  uint32_t province_size = state.world.province_size();
  uint32_t texture_size = province_size + 256 - province_size % 256;

  std::vector<uint32_t> prov_color(texture_size * 2);

  state.world.for_each_province([&](dcon::province_id prov_id) {
    auto fat_id = dcon::fatten(state.world, prov_id);
    auto nation = fat_id.get_nation_from_province_ownership();

    if(nation == state.local_player_nation) {
      uint32_t color = 0x222222;
      uint32_t stripe_color = 0x222222;

      if(fat_id.get_is_coast()) {
        auto state_id = fat_id.get_abstract_state_membership();
        color = ogl::color_from_hash(state_id.get_state().id.index());
        stripe_color = 0x00FF00; // light green 
        
        auto state_has_naval_base = military::state_has_naval_base(state, fat_id.get_state_membership());
        auto naval_base_lvl = fat_id.get_naval_base_level();
        auto max_naval_base_lvl = state.world.nation_get_max_naval_base_level(nation);
        
        if(state_has_naval_base && naval_base_lvl == 0){
          stripe_color = ogl::color_from_hash(state_id.get_state().id.index());
        } else if (naval_base_lvl == max_naval_base_lvl){
          stripe_color = 0x005500; // dark green
        }

      }
      
      auto i = province::to_map_id(prov_id);
      prov_color[i] = color;
      prov_color[i + texture_size] = stripe_color;
    }

  });

  return prov_color;
}
#pragma once


namespace text {

uint16_t name_into_font_id(std::string_view text);
int32_t size_from_font_id(uint16_t id);
bool is_black_from_font_id(uint16_t id);
uint32_t font_index_from_font_id(uint16_t id);

}
#pragma once

namespace ui {

// Return text key for labour name on the provided labour type
std::string_view labour_type_to_employment_name_text_key(int32_t labor_type);
// Return text key for tooltips on the provided labour type
std::string_view labour_type_to_employment_type_text_key(int32_t type);


}

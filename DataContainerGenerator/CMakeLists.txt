
set(PROJECT_SRC "code_fragments.cpp" "code_fragments.hpp" "DataContainerGenerator.cpp" "object_member_fragments.cpp" "parsing.cpp" "parsing.hpp" "query_fragments.cpp" "serialize_fragments.cpp" "source_builder.hpp")

add_executable(DataContainerGenerator ${PROJECT_SRC})

target_compile_features(DataContainerGenerator PRIVATE cxx_std_20 c_std_11)
 
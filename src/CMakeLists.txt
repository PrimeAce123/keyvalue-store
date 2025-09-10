# Add any more files you want here
add_library(p1_library SHARED p1/p1_sample.cpp)
add_library(p2_library SHARED p2/p2_sample.cpp)

set_target_properties(p1_library PROPERTIES UNITY_BUILD ON)
set_target_properties(p2_library PROPERTIES UNITY_BUILD ON)
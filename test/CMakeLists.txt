# Add any other test files here
add_executable(unittest unittest.cpp
                        p1/p1_test.cpp
                        p2/p2_test.cpp)
target_link_libraries(unittest p1_library p2_library)

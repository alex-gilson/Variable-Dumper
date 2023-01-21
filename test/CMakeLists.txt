
set(VARIABLE_DUMPER_TEST_DIR ${VARIABLE_DUMPER_DIR}test/)

add_executable(VariableDumperTs
                    VariableDumperTs.cpp
               )
if(MSVC)
  target_compile_options(VariableDumperTs PRIVATE /W4 /WX)
else()
  target_compile_options(VariableDumperTs PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

target_include_directories(VariableDumperTs
                              PRIVATE
                                   .
                          )

target_link_libraries(VariableDumperTs
                         PRIVATE
                              VariableDumper
                    )

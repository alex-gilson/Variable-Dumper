
set(VARIABLE_DUMPER_TEST_DIR ${VARIABLE_DUMPER_DIR}test/)
set(VARIABLE_DUMPER_TEST_DATA_DIR ${VARIABLE_DUMPER_TEST_DIR}data/)


add_executable(VariableDumperTs
                    VariableDumperTs.cpp
               )
if(MSVC)
  target_compile_options(VariableDumperTs PRIVATE /W4 /WX)
else()
  target_compile_options(VariableDumperTs PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

target_compile_definitions(VariableDumperTs PUBLIC "VARIABLE_DUMPER_TEST_DATA_DIR=\"${VARIABLE_DUMPER_TEST_DATA_DIR}\"")


target_include_directories(VariableDumperTs
                              PRIVATE
                                   .
                          )

target_link_libraries(VariableDumperTs
                         PRIVATE
                              VariableDumper
                    )

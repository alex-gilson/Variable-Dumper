
set(VARIABLE_DUMPER_SRC_DIR ${VARIABLE_DUMPER_DIR}src/ )

target_sources(VariableDumper
	PRIVATE
		Dumper.cpp
)

target_include_directories(VariableDumper PUBLIC .)

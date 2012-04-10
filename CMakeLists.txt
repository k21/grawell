cmake_minimum_required (VERSION 2.8)
project (GraWell CXX)

macro(add_source_files_flags SOURCES FLAGS)
	foreach(SRC ${SOURCES})
		get_property(ORIG SOURCE "${SRC}" PROPERTY COMPILE_FLAGS)
		set_property(SOURCE ${SRC}
			PROPERTY COMPILE_FLAGS "${ORIG} ${FLAGS}")
	endforeach()
endmacro()

macro(add_target_flags TARGET FLAGS)
	get_property(ORIG TARGET "${TARGET}" PROPERTY LINK_FLAGS)
	set_property(TARGET ${TARGET}
		PROPERTY LINK_FLAGS "${ORIG} ${FLAGS}")
endmacro()

set(WARNINGS "-Wall -Wextra -Weffc++ -Wshadow -Wconversion")

aux_source_directory(src SOURCES)
add_executable(dist/grawell EXCLUDE_FROM_ALL ${SOURCES})
add_source_files_flags("${SOURCES}" "-std=gnu++0x")
add_source_files_flags("${SOURCES}" "${WARNINGS}")
add_source_files_flags("${SOURCES}" "-fno-exceptions")
add_target_flags("dist/grawell" "-flto -fno-exceptions")

target_link_libraries(dist/grawell
	sfml-graphics
	sfml-window
	sfml-audio
	sfml-network
	sfml-system
)

add_custom_target(dist_directory
	COMMAND ${CMAKE_COMMAND} -E make_directory dist
)
add_dependencies(dist/grawell dist_directory)

add_custom_target(grawell ALL)
add_dependencies(grawell dist/grawell)

set(TESTED_SOURCES src/Encoder.cpp src/Decoder.cpp)
aux_source_directory(test TEST_SOURCES)
aux_source_directory(test/gtest GTEST_SOURCES)
add_executable(grawell_test EXCLUDE_FROM_ALL
	${TESTED_SOURCES} ${TEST_SOURCES} ${GTEST_SOURCES})
add_source_files_flags("${GTEST_SOURCES}" "-std=c++98")
add_source_files_flags("${GTEST_SOURCES}" "-fno-exceptions")
add_source_files_flags("${TEST_SOURCES}" "-std=gnu++0x")
add_source_files_flags("${TEST_SOURCES}" "${WARNINGS}")
add_source_files_flags("${TEST_SOURCES}" "-fno-exceptions")
add_source_files_flags("${TEST_SOURCES}"
	"-isystem ${CMAKE_SOURCE_DIR}/test/gtest")
add_target_flags("grawell_test" "-flto -fno-exceptions")
target_link_libraries(grawell_test
	pthread
)
add_custom_target(check
	COMMAND grawell_test
	DEPENDS grawell_test
)

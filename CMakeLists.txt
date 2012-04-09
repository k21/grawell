cmake_minimum_required (VERSION 2.6)
project (GraWell CXX)

aux_source_directory(src SOURCES)
add_executable(dist/grawell EXCLUDE_FROM_ALL ${SOURCES})
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

aux_source_directory(test TEST_SOURCES)
add_executable(grawell_test EXCLUDE_FROM_ALL ${TEST_SOURCES})
target_link_libraries(grawell_test
	pthread
)
add_custom_target(check
	COMMAND grawell_test
	DEPENDS grawell_test
)

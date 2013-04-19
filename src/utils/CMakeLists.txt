add_library(utilities
	SHARED
	command-line-arguments.cpp
	helpers.cpp
	points.cpp)
target_link_libraries(utilities ${LIBS})

if(WIN32)
    set(LIBTYPE STATIC)
    
    # Shlwapi.lib is the lib required by PathRemoveFileSpec and PathStripPath
    # used in helpers.cpp to replace dirname and basename in Windows
    set(EXTRALIBS "Shlwapi.lib")
else()
    set(LIBTYPE SHARED)
    set(EXTRALIBS "")
endif()
    
add_library(utilities
	${LIBTYPE}
	command-line-arguments.cpp
	helpers.cpp
	points.cpp)
    
target_link_libraries(utilities ${LIBS} ${EXTRALIBS})
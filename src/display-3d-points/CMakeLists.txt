# -*-cmake-*-
add_executable(display-3d-points
  main.cpp)
set_target_properties(display-3d-points PROPERTIES DEBUG_POSTFIX "d")
target_link_libraries(display-3d-points
  utilities
  clmTracker
  ${LIBS})

install(TARGETS display-3d-points DESTINATION bin)
# -*-cmake-*-
add_executable(display-3d-points
  main.cpp)
target_link_libraries(display-3d-points
  utilities
  clmTracker
  ${LIBS})

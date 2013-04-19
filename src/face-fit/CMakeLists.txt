# -*-cmake-*-
add_executable(face-fit
  main.cpp)
target_link_libraries(face-fit
  utilities
  clmTracker
  ${LIBS})

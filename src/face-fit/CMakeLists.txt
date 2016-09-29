# -*-cmake-*-
add_executable(face-fit
  main.cpp)
target_link_libraries(face-fit
  utilities
  clmTracker
  ${LIBS})

install(TARGETS face-fit DESTINATION bin)
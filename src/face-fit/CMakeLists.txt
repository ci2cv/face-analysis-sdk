# -*-cmake-*-
add_executable(face-fit
  main.cpp)
set_target_properties(face-fit PROPERTIES DEBUG_POSTFIX "d")
target_link_libraries(face-fit
  utilities
  clmTracker
  ${LIBS})

install(TARGETS face-fit DESTINATION bin)
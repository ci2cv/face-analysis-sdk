# -*-cmake-*-

add_executable(display-tracking
  main.cpp)
set_target_properties(display-tracking PROPERTIES DEBUG_POSTFIX "d")
target_link_libraries(display-tracking
  utilities
  ${LIBS})

install(TARGETS display-tracking DESTINATION bin)
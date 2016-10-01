# -*-cmake-*-

add_executable(display-avatar
  main.cpp)
set_target_properties(display-avatar PROPERTIES DEBUG_POSTFIX "d")
target_link_libraries(display-avatar
  utilities
  avatarAnim
  ${LIBS})

install(TARGETS display-avatar DESTINATION bin)
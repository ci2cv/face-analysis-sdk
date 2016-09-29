# -*-cmake-*-

add_executable(display-avatar
  main.cpp)

target_link_libraries(display-avatar
  utilities
  avatarAnim
  ${LIBS})

install(TARGETS display-avatar DESTINATION bin)
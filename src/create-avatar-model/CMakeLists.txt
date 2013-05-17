# -*-cmake-*-

add_executable(create-avatar-model
  main.cpp)

target_link_libraries(create-avatar-model
  utilities
  avatarAnim
  ${LIBS})

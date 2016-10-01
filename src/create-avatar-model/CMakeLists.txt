# -*-cmake-*-

add_executable(create-avatar-model
  main.cpp)
set_target_properties(create-avatar-model PROPERTIES DEBUG_POSTFIX "d")
target_link_libraries(create-avatar-model
  utilities
  avatarAnim
  ${LIBS})

install(TARGETS create-avatar-model DESTINATION bin)
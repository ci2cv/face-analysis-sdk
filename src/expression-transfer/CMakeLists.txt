# -*-cmake-*-
add_executable(expression-transfer
  main.cpp)
set_target_properties(expression-transfer PROPERTIES DEBUG_POSTFIX "d")
target_link_libraries(expression-transfer
  utilities
  avatarAnim
  ${LIBS})

install(TARGETS expression-transfer DESTINATION bin)
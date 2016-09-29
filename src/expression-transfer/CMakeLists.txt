# -*-cmake-*-
add_executable(expression-transfer
  main.cpp)

target_link_libraries(expression-transfer
  utilities
  avatarAnim
  ${LIBS})

install(TARGETS expression-transfer DESTINATION bin)
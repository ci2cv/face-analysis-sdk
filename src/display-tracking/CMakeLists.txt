# -*-cmake-*-

add_executable(display-tracking
  main.cpp)

target_link_libraries(display-tracking
  utilities
  ${LIBS})

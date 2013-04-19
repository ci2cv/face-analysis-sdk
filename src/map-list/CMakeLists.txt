# -*-cmake-*-

add_executable(map-list
  main.cpp)

target_link_libraries(map-list
  utilities
  ${LIBS})

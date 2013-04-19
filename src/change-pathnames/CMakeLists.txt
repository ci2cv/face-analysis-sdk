# -*-cmake-*-

add_executable(change-pathnames
  main.cpp)

target_link_libraries(change-pathnames
  utilities
  ${LIBS})
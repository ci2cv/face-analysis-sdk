# -*-cmake-*-

add_executable(change-pathnames
  main.cpp)
set_target_properties(change-pathnames PROPERTIES DEBUG_POSTFIX "d")
target_link_libraries(change-pathnames
  utilities
  ${LIBS})
  
install(TARGETS change-pathnames DESTINATION bin)
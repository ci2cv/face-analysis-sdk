# -*-cmake-*-

add_executable(map-list
  main.cpp)
set_target_properties(map-list PROPERTIES DEBUG_POSTFIX "d")
target_link_libraries(map-list
  utilities
  ${LIBS})
  
install(TARGETS map-list DESTINATION bin)

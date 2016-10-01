# -*-cmake-*-

ADD_EXECUTABLE(test_avatar test_Avatar.cpp)
set_target_properties(test_avatar PROPERTIES DEBUG_POSTFIX "d")
TARGET_LINK_LIBRARIES(test_avatar ${LIBS} clmTracker)

TARGET_LINK_LIBRARIES(test_avatar avatarAnim)

ADD_EXECUTABLE(speed_test speed_test.cpp command-line-options.cpp)
set_target_properties(speed_test PROPERTIES DEBUG_POSTFIX "d")
TARGET_LINK_LIBRARIES(speed_test ${LIBS} clmTracker avatarAnim)

ADD_EXECUTABLE(add_avatar add_avatar.cpp)
set_target_properties(add_avatar PROPERTIES DEBUG_POSTFIX "d")
TARGET_LINK_LIBRARIES(add_avatar ${LIBS} avatarAnim)

install(TARGETS test_avatar DESTINATION bin)
install(TARGETS speed_test DESTINATION bin)
install(TARGETS add_avatar DESTINATION bin)
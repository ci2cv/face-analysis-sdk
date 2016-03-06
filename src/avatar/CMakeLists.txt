# -*-cmake-*-

SET(SDK_AVATAR_DATA_DIR ${PROJECT_SOURCE_DIR}/src/avatar/resources/)
SET(SDK_AVATAR_DEFAULT_MODEL_PATHNAME ${SDK_AVATAR_DATA_DIR}/CI2CV.avatar.binary)
SET(SDK_AVATAR_DEFAULT_PARAMS_PATHNAME ${SDK_AVATAR_DATA_DIR}/not-needed-at-present)

CONFIGURE_FILE("Config.h.in" "Config.h")

SET(AVATAR_FILES "Avatar.cpp" "myAvatar.cpp")

if(WIN32)
    set(LIBTYPE STATIC)
else()
    set(LIBTYPE SHARED)
endif()

ADD_LIBRARY(avatarAnim ${LIBTYPE} ${AVATAR_FILES})
TARGET_LINK_LIBRARIES(avatarAnim ${LIBS} utilities clmTracker)
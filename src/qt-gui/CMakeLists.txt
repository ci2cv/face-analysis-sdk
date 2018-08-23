# -*-cmake-*-
# find_package(Qt4 REQUIRED)

# INCLUDE( ${QT_USE_FILE} )

# qt
if(APPLE)
    set(QTDIR /Users/lusheng/Qt/5.11.1/clang_64)
    set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ${QTDIR})
endif(APPLE)
find_package(Qt5Widgets CONFIG REQUIRED)
include_directories(${Qt5Widgets_INCLUDE_DIRS})

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# opencv
if(WIN32)
    set(opencv_DIR E:/toolkits/opencv-3.2.0-contrib-cpu)
endif(WIN32)
if(APPLE)
    set(opencv_DIR ~/toolkits/opencv-3.4.1-mac/share/OpenCV)
endif(APPLE)
find_package(opencv REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
message("OpenCV_INCLUDE_DIRS = ${OpenCV_INCLUDE_DIRS}")

#set(CMAKE_VERBOSE_MAKEFILE true)

SET(BundleName "qt-gui")

SET(guiFILES 
  # "application-states.cpp"
  # "controllers.cpp"
  # "configuration.cpp"
  # "gui/avatar-selection.cpp"
  # "gui/main-window.cpp"
  # "gui/avatar.cpp"
  # "gui/mesh-drawer.cpp"
  # "gui/graphics-scrollbar.cpp"
  # "gui/windowed-gui-controller.cpp"
  # "gui/item-controllers.cpp"
  # "gui/worker-thread.cpp"
  # "gui/item-positions-calculator.cpp"

  application-states.cpp
  controllers.cpp
  configuration.cpp
  gui/main-window.cpp
  gui/windowed-gui-controller.cpp
  gui/item-controllers.cpp
  gui/item-positions-calculator.cpp
  gui/graphics-scrollbar.cpp
  gui/avatar-selection.cpp
  gui/avatar.cpp
  gui/worker-thread.cpp
  gui/mesh-drawer.cpp

  ../tracker/IO.cpp
  ../tracker/CLM.cpp
  ../tracker/FDet.cpp
  ../tracker/Patch.cpp
  ../tracker/ShapeModel.cpp
  # ../tracker/PRA.cpp
  # ../tracker/FacePredictor.cpp
  ../tracker/ShapePredictor.cpp
  ../tracker/ATM.cpp
  ../tracker/Warp.cpp
  ../tracker/FCheck.cpp
  ../tracker/FaceTracker.cpp
  ../tracker/myFaceTracker.cpp
  ../tracker/RegistrationCheck.cpp
  ../avatar/Avatar.cpp
  ../avatar/myAvatar.cpp
)

# SET(MOC_HEADERS
#   "controllers.hpp"
#   "gui/avatar-selection.hpp"
#   "gui/main-window.hpp"
#   "gui/avatar.hpp"
# #  "gui/mesh-drawer.hpp"
#   "gui/graphics-scrollbar.hpp"
#   "gui/windowed-gui-controller.hpp"
#   "gui/item-controllers.hpp"
#   "gui/worker-thread.hpp")
# #  "gui/item-positions-calculator.hpp")

# QT4_WRAP_CPP(MOC_SRCS ${MOC_HEADERS})

INCLUDE_DIRECTORIES(".")
set_source_files_properties(osx-configuration.mm PROPERTIES COMPILE_FLAGS -ObjC++)


add_executable(demo-application
  main.cpp
  command-line-configuration.cpp
  ${guiFILES}
  ${MOC_SRCS}
  ${EXTRA_SRC})
target_link_libraries(demo-application ${LIBS} 
  # ${QT_LIBRARIES} 
  Qt5::Widgets
  clmTracker avatarAnim utilities ${EXTRA_LIBS})

# Apple Specific Targets

IF(APPLE)
  SET(MACOSX_BUNDLE_BUNDLE_NAME ${BundleName})
  set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/Configuration.plist PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
  
  SET(EXTRA_SRC "Configuration.plist")

ENDIF()


ADD_EXECUTABLE(${BundleName} MACOSX_BUNDLE 
  main.cpp 
  osx-configuration.mm
  ${guiFILES} 
  ${MOC_SRCS}
  ${EXTRA_SRC}
  )
TARGET_LINK_LIBRARIES(${BundleName} ${LIBS} 
  # ${QT_LIBRARIES} 
  Qt5::Widgets
  clmTracker avatarAnim ${EXTRA_LIBS})


# IF(APPLE)

#  # set_target_properties(${BundleName} PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Configuration.plist)
 

# ELSEIF(UNIX)

# ENDIF()


IF(APPLE)

  SET(plugin_dest_dir "${BundleName}.app/Contents/PlugIns")
  SET(qtconf_dest_dir "${BundleName}.app/Contents/Resources")
  SET(APP_FOLDER "\${CMAKE_INSTALL_PREFIX}/${BundleName}.app")
  
  INSTALL(TARGETS ${BundleName}
    BUNDLE DESTINATION . COMPONENT Runtime
    RUNTIME DESTINATION bin COMPONENT Runtime
    )
  
  FILE(GLOB trackerFiles "${CMAKE_CURRENT_SOURCE_DIR}/../tracker/resources/*")
  FILE(GLOB avatarFiles "${CMAKE_CURRENT_SOURCE_DIR}/../avatar/resources/*")

  INSTALL(FILES ${trackerFiles} DESTINATION ${qtconf_dest_dir} COMPONENT Runtime)
  INSTALL(FILES ${avatarFiles} DESTINATION ${qtconf_dest_dir} COMPONENT Runtime)

  SET(DIRS ${QT_LIBRARY_DIR} "/usr/local/" "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")
  
  INSTALL(CODE "
    file(GLOB_RECURSE QTPLUGINS
      \"\${CMAKE_INSTALL_PREFIX}/${plugin_dest_dir}/*${CMAKE_SHARED_LIBRARY_SUFFIX}\")
    include(BundleUtilities)
    fixup_bundle(\"${APP_FOLDER}\" \"\${QTPLUGINS}\" \"${DIRS}\")
    " COMPONENT Runtime)

  include(CPack)

ENDIF()

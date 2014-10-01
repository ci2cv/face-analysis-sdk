# -*-cmake-*-
#set(CMAKE_VERBOSE_MAKEFILE true)
#Qt5 - minimum the 5.2.1 version + WebSockets
set(CMAKE_AUTOMOC ON)
#Qt5 Core
find_package(Qt5Widgets 5.2.1 REQUIRED)
include_directories(${Qt5Widgets_INCLUDE_DIRS})

message( STATUS "Qt5: Found")

IF(Qt5_POSITION_INDEPENDENT_CODE)
  SET(CMAKE_POSITION_INDEPENDENT_CODE ON)
ENDIF()
SET(BundleName "qt-gui")

SET(guiFILES "application-states.cpp"
  "controllers.cpp"
  "configuration.cpp"
  "command-line-configuration.cpp"
  "gui/avatar-selection.cpp"
  "gui/main-window.cpp"
  "gui/avatar.cpp"
  "gui/mesh-drawer.cpp"
  "gui/graphics-scrollbar.cpp"
  "gui/windowed-gui-controller.cpp"
  "gui/item-controllers.cpp"
  "gui/worker-thread.cpp"
  "gui/item-positions-calculator.cpp"
)

SET(MOC_HEADERS
  "controllers.hpp"
  "gui/avatar-selection.hpp"
  "gui/main-window.hpp"
  "gui/avatar.hpp"
#  "gui/mesh-drawer.hpp"
  "gui/graphics-scrollbar.hpp"
  "gui/windowed-gui-controller.hpp"
  "gui/item-controllers.hpp"
  "gui/worker-thread.hpp")
#  "gui/item-positions-calculator.hpp")

INCLUDE_DIRECTORIES(".")
IF(APPLE)
set_source_files_properties(osx-configuration.mm PROPERTIES COMPILE_FLAGS -ObjC++)
ENDIF()

add_executable(demo-application
  main.cpp
  command-line-configuration.cpp
  ${MOC_HEADERS}
  ${guiFILES}
  ${EXTRA_SRC})
target_link_libraries(demo-application ${LIBS} clmTracker avatarAnim utilities ${EXTRA_LIBS})
qt5_use_modules(demo-application Core Gui Widgets)
# Apple Specific Targets

IF(APPLE)
  SET(MACOSX_BUNDLE_BUNDLE_NAME ${BundleName})
  set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/Configuration.plist PROPERTIES MACOSX_PACKAGE_LOCATION Resources)

  SET(EXTRA_SRC "Configuration.plist")

  ADD_EXECUTABLE(${BundleName} MACOSX_BUNDLE
	main.cpp
	osx-configuration.mm
	${MOC_HEADERS}
	${guiFILES}
	${EXTRA_SRC}
  )
  TARGET_LINK_LIBRARIES(${BundleName} ${LIBS} clmTracker avatarAnim ${EXTRA_LIBS})
  qt5_use_modules(${BundleName} Widgets)
ELSE()
  ADD_EXECUTABLE(${BundleName}
	main.cpp
	${guiFILES}
	${EXTRA_SRC}
  )
  TARGET_LINK_LIBRARIES(${BundleName} ${LIBS} clmTracker avatarAnim ${EXTRA_LIBS})
  qt5_use_modules(${BundleName} Widgets)
ENDIF()


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

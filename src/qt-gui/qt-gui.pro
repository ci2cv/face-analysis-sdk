CONFIG += qt thread debug_and_release

INCLUDEPATH += \
  /usr/local \
  ../

LIBS += \
  -L/usr/local \
  -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_objdetect -lopencv_video -lobjc \
  -framework Foundation 

include(qt-gui.pro.local) {
  message("Loading in data from qt-gui.pro.local")
} else {
  message("No qt-gui.pro.local found. Continuing without it.")
}

HEADERS += \
  application-states.hpp \
  controllers.hpp \
  configuration.hpp \
  osx-configuration.hpp \
  gui/main-window.hpp \
  gui/windowed-gui-controller.hpp \
  gui/item-controllers.hpp \
  gui/item-positions-calculator.hpp \
  gui/graphics-scrollbar.hpp \
  gui/avatar-selection.hpp \
  gui/avatar.hpp \
  gui/worker-thread.hpp \
  gui/mesh-drawer.hpp

OBJECTIVE_SOURCES += \
  osx-configuration.mm 

SOURCES += \
  application-states.cpp \
  controllers.cpp \
  configuration.cpp \
  main.cpp \
  gui/main-window.cpp \
  gui/windowed-gui-controller.cpp \
  gui/item-controllers.cpp \
  gui/item-positions-calculator.cpp \
  gui/graphics-scrollbar.cpp \
  gui/avatar-selection.cpp \
  gui/avatar.cpp \
  gui/worker-thread.cpp \
  gui/mesh-drawer.cpp \
\
  ../tracker/IO.cpp \
  ../tracker/CLM.cpp \
  ../tracker/FDet.cpp \
  ../tracker/Patch.cpp \
  ../tracker/ShapeModel.cpp \
  ../tracker/PRA.cpp \
  ../tracker/FacePredictor.cpp \
  ../tracker/ShapePredictor.cpp \
  ../tracker/ATM.cpp \
  ../tracker/Warp.cpp \
  ../tracker/FCheck.cpp \
  ../tracker/FaceTracker.cpp \
  ../tracker/myFaceTracker.cpp \
  ../tracker/RegistrationCheck.cpp \
  ../avatar/Avatar.cpp \
  ../avatar/myAvatar.cpp

TARGET = qt-gui

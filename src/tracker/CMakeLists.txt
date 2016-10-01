# -*-cmake-*-

# Define the place to install tracker resources
SET(SDK_TRACKER_DATA_DIR ${PROJECT_SOURCE_DIR}/src/tracker/resources/)
SET(SDK_FACETRACKER_DEFAULT_CON_PATHNAME ${SDK_TRACKER_DATA_DIR}/face.con)
SET(SDK_FACETRACKER_DEFAULT_MODEL_PATHNAME ${SDK_TRACKER_DATA_DIR}/face.mytracker.binary)
SET(SDK_FACETRACKER_DEFAULT_PARAMS_PATHNAME ${SDK_TRACKER_DATA_DIR}/face.mytrackerparams.binary)

CONFIGURE_FILE("Config.h.in" "Config.h")

SET(TRACKER_FILES 
  "ATM.cpp"
  "FCheck.cpp"
  "IO.cpp"
  "Patch.cpp"
  "Detector.cpp"
  "ShapeModel.cpp"
  "Warp.cpp"
  "CLM.cpp"
  "FDet.cpp"
  "FaceTracker.cpp"
  "RegistrationCheck.cpp"
  "ShapePredictor.cpp"
  "myFaceTracker.cpp")

if(WIN32)
    set(LIBTYPE STATIC)
else()
    set(LIBTYPE SHARED)
endif()  

ADD_LIBRARY(clmTracker ${LIBTYPE} ${TRACKER_FILES})
set_target_properties(clmTracker PROPERTIES DEBUG_POSTFIX "d")
TARGET_LINK_LIBRARIES(clmTracker ${LIBS})

install(TARGETS clmTracker EXPORT CSIRO DESTINATION lib)
install(FILES FaceTracker.hpp IO.hpp DESTINATION include/tracker)
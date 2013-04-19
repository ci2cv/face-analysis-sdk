# -*-cmake-*-

# Script files to be generated
set(generated_scripts
  rotate-movie
  remove-rotation-metadata
  extract-frames-from-movie
  create-movie-from-frames)

# Build the list of executable scripts
set(executable_scripts)
foreach(_script ${generated_scripts})
  configure_file(${_script}.in ${_script})
  add_custom_command(
    OUTPUT ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${_script}
    COMMAND /usr/bin/install -m 755 ${_script} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${_script}
    DEPENDS ${_script}.in)
  list(APPEND executable_scripts ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${_script})
endforeach()

add_custom_target(mark-scripts-executable ALL
  DEPENDS ${executable_scripts})
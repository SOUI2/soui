function(dump_variables)
  message("SOUI build arguments:")
  foreach(_variable ${ARGN})
    message("- ${_variable}: ${${_variable}}")
  endforeach(_variable ${ARGN})
endfunction(dump_variables)

macro(SOUI_INSTALL_TARGETS)
  install(TARGETS ${ARGN} EXPORT SOUITargets
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
    ARCHIVE DESTINATION lib)
endmacro(SOUI_INSTALL_TARGETS)

set(SOUI_CMAKE_VARIABLE_PREFIX "$")

find_program (DOXYGEN doxygen)
find_program (BROWSER NAMES google-chrome firefox)

# modify doxygen configuration file
set (DOCUMENTED_CODE ${PROJECT_SOURCE_DIR}/trunk)
set (DOCUMENTATION_DIR ${PROJECT_SOURCE_DIR})
configure_file (
  "Doxyfile.in"
  "${PROJECT_BINARY_DIR}/Doxyfile"
)

# target for generating documentation
if (DOXYGEN)
  add_custom_target(doc ${DOXYGEN} Doxyfile
    VERBATIM
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
    )
else (DOXYGEN)
  message (STATUS "WARNING: doxygen was not found")
endif (DOXYGEN)

# target for showing html documentation in a browser
if (BROWSER AND DOXYGEN)
  add_custom_target(showdoc ${BROWSER} ${DOCUMENTATION_DIR}/html/index.html
    VERBATIM
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    )
else (BROWSER AND DOXYGEN)
  message (STATUS "WARNING: browser was not found")
endif (BROWSER AND DOXYGEN)

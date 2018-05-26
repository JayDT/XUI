include(ans.env)
include(ans.parse_arguments)

# doxygen_support([TARGET target_name] [TEMPLATE template_name])
# http://goo.gl/kKWx0
macro(doxygen_support)
    parse_arguments(
        DOXYGEN_SUPPORT
        "TARGET;TEMPLATE"
        ""
        ${ARGN}
        )
    list(APPEND DOXYGEN_SUPPORT_TARGET docs)
    car(DOXYGEN_SUPPORT_TARGET_ ${DOXYGEN_SUPPORT_TARGET})
    list(APPEND DOXYGEN_SUPPORT_TEMPLATE ${ANS_TEMPLATE_DIR}/Doxyfile.in)
    car(DOXYGEN_SUPPORT_TEMPLATE_ ${DOXYGEN_SUPPORT_TEMPLATE})
    find_package(Doxygen REQUIRED)
    configure_file(${DOXYGEN_SUPPORT_TEMPLATE_} ${PROJECT_BINARY_DIR}/Doxyfile @ONLY IMMEDIATE)
    add_custom_target(
        ${DOXYGEN_SUPPORT_TARGET_}
        #ALL
        COMMAND ${DOXYGEN_EXECUTABLE} ${PROJECT_BINARY_DIR}/Doxyfile
        SOURCES ${PROJECT_BINARY_DIR}/Doxyfile
        )
    # IF you do NOT want the documentation to be generated EVERY time you build the project
    # then leave out the 'ALL' keyword from the above command.
endmacro()

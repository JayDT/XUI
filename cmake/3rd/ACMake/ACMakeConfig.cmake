if (ACMake_DIR)
    set(ACMake_INSTALL_DIR "${ACMake_DIR}")
else()
    message(FATAL_ERROR "Can't find a ACMake installation.")
endif()

list(APPEND CMAKE_MODULE_PATH "${ACMake_INSTALL_DIR}")

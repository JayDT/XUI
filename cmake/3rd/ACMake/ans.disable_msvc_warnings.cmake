if(MSVC)
    add_definitions(-wd4786) # identifier was truncated to '255' characters in the debug information
    add_definitions(-wd4251) # 'identifier' : class 'type' needs to have dll-interface to be used by clients of class 'type2'
    add_definitions(-wd4250) # ... inherits ... via dominance
    add_definitions(-wd4099) # struct and class mixed
    add_definitions(-wd4996) #  'std::_Uninitialized_copy0': Function call with parameters that may be unsafe
endif()

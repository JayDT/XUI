#-------------------------------------------------------------------------------
# @author: answeror
# @create time: 2011-10-28
#-------------------------------------------------------------------------------

# create vcproj.user file for Visual Studio to set debug working directory
# or some other perpose
# @template_dir: your template vcproj and vcxproj file location
function(create_vcproj_userfile template_dir)
    # ensure no backslash
    file(TO_CMAKE_PATH ${template_dir} template_dir)
    create_vcproj_userfile_detail(${PROJECT_NAME} ${template_dir}/${PROJECT_NAME})
endfunction()

# create vcproj.user file for Visual Studio to set debug working directory
# or some other perpose
# @targetname: usually ${PROJECT_NAME}
# @template_prefix: usually <template directory>/${PROJECT_NAME}
function(create_vcproj_userfile_detail targetname template_prefix)
    if (MSVC)
        configure_file(
            ${template_prefix}.vcproj.user.in
            ${CMAKE_CURRENT_BINARY_DIR}/${targetname}.vcproj.user
            @ONLY
            )
        configure_file(
            ${template_prefix}.vcxproj.user.in
            ${CMAKE_CURRENT_BINARY_DIR}/${targetname}.vcxproj.user
            @ONLY
            )
    endif ()
endfunction()

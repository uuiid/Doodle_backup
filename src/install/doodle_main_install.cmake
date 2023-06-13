
file(GLOB 7Z_FOLDER_LISTS
        LIST_DIRECTORIES true
        RELATIVE ${CMAKE_INSTALL_PREFIX}
        ${CMAKE_INSTALL_PREFIX}/*
)

configure_file(${CMAKE_CURRENT_LIST_DIR}/main.iss ${CMAKE_INSTALL_PREFIX}/wix/main.iss)

execute_process(
        COMMAND ${DOODLE_PANDOC_PATH} -s @PROJECT_SOURCE_DIR@/LICENSE -f textile -o ./wix/LICENSE.rtf
        WORKING_DIRECTORY ${CMAKE_INSTALL_PREFIX}
)

list(REMOVE_ITEM 7Z_FOLDER_LISTS wix)
list(TRANSFORM 7Z_FOLDER_LISTS PREPEND ${CMAKE_INSTALL_PREFIX}/)
list(FILTER 7Z_FOLDER_LISTS EXCLUDE REGEX "Doodle-")
message("pack folder ${7Z_FOLDER_LISTS} to @DOODLE_PACKAGE_NAME@")

execute_process(
        COMMAND @SEVENZIP_BIN@ a -mx2 -mmt8
        "${CMAKE_INSTALL_PREFIX}/@DOODLE_PACKAGE_NAME@.7z"
        ${7Z_FOLDER_LISTS}
        WORKING_DIRECTORY ${CMAKE_INSTALL_PREFIX}/wix
)
execute_process(COMMAND ${DOODLE_INNO_EXE_PATH} ${CMAKE_INSTALL_PREFIX}/wix/main.iss
        WORKING_DIRECTORY ${CMAKE_INSTALL_PREFIX}/wix
)
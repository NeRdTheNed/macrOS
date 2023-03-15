include(CTest)

add_executable(versionCheck versionCheck.c)
target_link_libraries(versionCheck PRIVATE macrOS)
add_test(NAME versionCheck COMMAND versionCheck)

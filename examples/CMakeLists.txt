add_executable(popup popup.c)
target_link_libraries(popup PRIVATE macrOS)
target_link_libraries(popup PRIVATE "-framework Cocoa")

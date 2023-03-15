add_executable(popup MACOSX_BUNDLE popup.c)
set_target_properties(
  popup
  PROPERTIES OUTPUT_NAME "Popup demo"
             MACOSX_BUNDLE TRUE
             MACOSX_BUNDLE_BUNDLE_NAME "Popup demo")
target_link_libraries(popup PRIVATE macrOS)
target_link_libraries(popup PRIVATE "-framework Cocoa")

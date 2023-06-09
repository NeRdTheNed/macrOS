cmake_minimum_required(VERSION 3.7)

set(CMAKE_C_STANDARD 99)

project(
  macrOS
  VERSION 1.0.0
  LANGUAGES C)

option(macrOS_BUILD_EXAMPLES "Build macrOS example programs" ON)
option(macrOS_BUILD_TESTS "Build macrOS tests" ON)

set(DEFAULT_BUILD_TYPE "Release")

if(CMAKE_BUILD_TYPE MATCHES Release)
  # Symbol visibility
  set(CMAKE_CXX_VISIBILITY_PRESET hidden)
  set(CMAKE_C_VISIBILITY_PRESET hidden)
  set(CMAKE_VISIBILITY_INLINES_HIDDEN YES)
endif()

# Set source files
file(
  GLOB
  macrOS_SOURCE_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/include/macrOS/Avail.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/macrOS/baseMacros.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/macrOS/FindVersion.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/macrOS/GeneratedAvail.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/macrOS/macMacros.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/macrOS/macrOSObjC.h
  ${CMAKE_CURRENT_SOURCE_DIR}/src/FindVersion.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/macrOSObjC.c)

add_library(${PROJECT_NAME} STATIC ${macrOS_SOURCE_FILES})

target_sources(${PROJECT_NAME} PUBLIC ${macrOS_SOURCE_FILES})

target_include_directories(
  ${PROJECT_NAME} PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                         $<INSTALL_INTERFACE:include>)

# Header only
add_library(${PROJECT_NAME}_headers INTERFACE)

target_sources(${PROJECT_NAME}_headers INTERFACE ${macrOS_SOURCE_FILES})

target_include_directories(
  ${PROJECT_NAME}_headers
  INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
            $<INSTALL_INTERFACE:include>)

# Boost is only used for the preprocessor library
find_package(Boost REQUIRED)
target_include_directories(${PROJECT_NAME}_headers
                           INTERFACE ${Boost_INCLUDE_DIRS})
target_include_directories(${PROJECT_NAME} PUBLIC ${Boost_INCLUDE_DIRS})

# Link with Foundation and the Objective-C runtime
target_link_libraries(${PROJECT_NAME} PRIVATE "-framework Foundation")

# Link with CoreServices if needed. TODO Replace with less fragile check

# Check if the version check test compiles
try_compile(
  DID_VERSION_CHECK_COMPILE
  "${CMAKE_CURRENT_BINARY_DIR}/try_compile_versionCheck"
  SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/FindVersion.c
          ${CMAKE_CURRENT_SOURCE_DIR}/src/macrOSObjC.c
          ${CMAKE_CURRENT_SOURCE_DIR}/test/versionCheck.c
  CMAKE_FLAGS
    "-DINCLUDE_DIRECTORIES=${CMAKE_CURRENT_SOURCE_DIR}/include;${Boost_INCLUDE_DIRS}"
    "-DLINK_LIBRARIES=-framework Foundation"
  OUTPUT_VARIABLE VERSION_CHECK_OUT)

# If it fails and Gestalt is mentioned, link with CoreServices, as Gestalt is
# from CoreServices.
if(NOT DID_VERSION_CHECK_COMPILE AND VERSION_CHECK_OUT MATCHES ".*Gestalt")
  target_link_libraries(${PROJECT_NAME} PRIVATE "-framework CoreServices")
endif()

# Allow weak linking with old SDKs. TODO Replace with less fragile check

# Check if the Objective-C test compiles
try_compile(
  DID_OBJC_TEST_COMPILE "${CMAKE_CURRENT_BINARY_DIR}/try_compile_objcTest"
  SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/FindVersion.c
          ${CMAKE_CURRENT_SOURCE_DIR}/src/macrOSObjC.c
          ${CMAKE_CURRENT_SOURCE_DIR}/test/objcTest.c
  CMAKE_FLAGS
    "-DINCLUDE_DIRECTORIES=${CMAKE_CURRENT_SOURCE_DIR}/include;${Boost_INCLUDE_DIRS}"
    "-DLINK_LIBRARIES=-framework Foundation"
  OUTPUT_VARIABLE OBJC_TEST_OUT)

# Allow certain symbols to not be defined, as they're weakly linked
if(NOT DID_OBJC_TEST_COMPILE
   AND OBJC_TEST_OUT
       MATCHES
       ".*(_class_addMethod|_objc_allocateClassPair|_objc_registerClassPair|_objc_getProtocol|_class_addProtocol)"
)
  target_link_options(
    ${PROJECT_NAME}
    PUBLIC
    "LINKER:-U,_class_addMethod,-U,_objc_allocateClassPair,-U,_objc_registerClassPair,-U,_objc_getProtocol,-U,_class_addProtocol"
  )
  target_link_options(
    ${PROJECT_NAME}_headers
    INTERFACE
    "LINKER:-U,_class_addMethod,-U,_objc_allocateClassPair,-U,_objc_registerClassPair,-U,_objc_getProtocol,-U,_class_addProtocol"
  )
endif()

if(macrOS_BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()

find_program(ASTYLE astyle)

# TODO Fix this properly
if(NOT ASTYLE)
  if(WIN32)
    set(ASTYLE REM)
  else()
    set(ASTYLE :)
  endif()
endif()

add_custom_target(
  ${PROJECT_NAME}_format
  COMMAND
    "${ASTYLE}" --mode=c --attach-classes --attach-return-type
    --attach-return-type-decl --break-blocks --break-one-line-headers
    --add-braces --pad-oper --pad-header --attach-return-type
    --delete-empty-lines --suffix=none --style=google --recursive
    "${CMAKE_CURRENT_SOURCE_DIR}/examples/*.c"
  COMMAND
    "${ASTYLE}" --mode=c --attach-classes --attach-return-type
    --attach-return-type-decl --break-blocks --break-one-line-headers
    --add-braces --pad-oper --pad-header --attach-return-type
    --delete-empty-lines --suffix=none --style=google --recursive
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
  VERBATIM
  DEPENDS "${macrOS_SOURCE_FILES}"
  COMMENT "Formatting source files")

find_package(Java)

if(Java_FOUND)
  add_custom_command(
    OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/include/macrOS/GeneratedAvail.h"
    COMMAND
      ${CMAKE_COMMAND} -E remove -f
      "${CMAKE_CURRENT_SOURCE_DIR}/tools/Avail.class"
      "${CMAKE_CURRENT_SOURCE_DIR}/tools/MacVersion.class"
    COMMAND
      "${ASTYLE}" --mode=java --attach-classes --attach-return-type
      --attach-return-type-decl --break-blocks --break-one-line-headers
      --add-braces --pad-oper --pad-header --attach-return-type
      --delete-empty-lines --suffix=none --style=google
      "${CMAKE_CURRENT_SOURCE_DIR}/tools/Avail.java"
    VERBATIM
    COMMAND "${Java_JAVAC_EXECUTABLE}"
            "${CMAKE_CURRENT_SOURCE_DIR}/tools/Avail.java"
    COMMAND
      "${Java_JAVA_EXECUTABLE}" -cp "${CMAKE_CURRENT_SOURCE_DIR}/tools" Avail
      -outputPath "${CMAKE_CURRENT_SOURCE_DIR}/include/macrOS/GeneratedAvail.h"
    COMMAND
      ${CMAKE_COMMAND} -E remove -f
      "${CMAKE_CURRENT_SOURCE_DIR}/tools/Avail.class"
      "${CMAKE_CURRENT_SOURCE_DIR}/tools/MacVersion.class"
    DEPENDS "tools/Avail.java"
    COMMENT "Generating macOS availability headers")

  add_custom_target(
    genAvail
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/include/macrOS/GeneratedAvail.h)
  add_dependencies(${PROJECT_NAME} genAvail)
  add_dependencies(${PROJECT_NAME}_headers genAvail)
else()
  message(WARNING "Java not found, header files not regenerated")
endif()

if(macrOS_BUILD_TESTS)
  add_subdirectory(test)
endif()

cmake_minimum_required( VERSION 3.1 )
project( xrootd-cmsjson )

set( CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake )

find_package( Xrootd REQUIRED )
find_package( Jsoncpp REQUIRED )

include_directories( "${PROJECT_SOURCE_DIR}" "${XROOTD_INCLUDES}" "${JSONCPP_INCLUDES}")

add_library( XrdCmsJson MODULE src/XrdCmsJson.cc src/XrdCmsJson.hh)
target_link_libraries(XrdCmsJson ${XROOTD_UTILS} ${JSONCPP_LIBRARIES})

add_executable(test.out  src/XrdCmsJson.cc src/XrdCmsJson.hh main.cc)
target_link_libraries(test.out ${XROOTD_UTILS} ${JSONCPP_LIBRARIES})


if (NOT DEFINED CMAKE_INSTALL_LIBDIR)
  SET(CMAKE_INSTALL_LIBDIR "lib64")
endif()

install(
  TARGETS XrdCmsJson
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} )

install(
  FILES ${CMAKE_SOURCE_DIR}/src/XrdCmsJson.hh
  DESTINATION include )

set_target_properties(
  XrdCmsJson
  PROPERTIES
  VERSION   "0.0.1"
  SOVERSION "0"
  LINK_INTERFACE_LIBRARIES "" )

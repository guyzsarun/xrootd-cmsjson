
cmake_minimum_required( VERSION 3.11 )
project( xrootd-cmsjson )

set( CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake )

find_package( Xrootd REQUIRED )
find_package( Jsoncpp REQUIRED )

if( CMAKE_COMPILER_IS_GNUCXX )
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror" )
endif()

if( CMAKE_COMPILER_IS_GNUCC )
  set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror" )
endif()


message("JSONCPP INCLUDE - ${JSONCPP_INCLUDES} - ${XROOTD_UTILS} - ${JSONCPP_LIBRARIES}" )
include_directories( "${PROJECT_SOURCE_DIR}" "${XROOTD_INCLUDES}" "${JSONCPP_INCLUDES}")

add_library( XrdCmsJson MODULE src/XrdCmsJson.cc src/XrdCmsJson.hh)
target_link_libraries(XrdCmsJson ${XROOTD_UTILS} ${JSONCPP_LIBRARIES})

add_executable(test.out  src/XrdCmsJson.cc src/XrdCmsJson.hh main.cc)
target_link_libraries(test.out ${XROOTD_UTILS} ${JSONCPP_LIBRARIES})


if (NOT DEFINED CMAKE_INSTALL_LIBDIR)
  SET(CMAKE_INSTALL_LIBDIR "lib")
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

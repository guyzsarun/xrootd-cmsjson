
cmake_minimum_required( VERSION 2.6 )
project( xrootd-cmsjson )

set( CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake )

find_package( Xrootd REQUIRED )
find_package( XercesC REQUIRED )
find_package( Pcre REQUIRED )

if( CMAKE_COMPILER_IS_GNUCXX )
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror" )
endif()

if( CMAKE_COMPILER_IS_GNUCC )
  set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror" )
endif()

include_directories( "${PROJECT_SOURCE_DIR}" "${XROOTD_INCLUDES}"
"${XERCES_INCLUDES}" "${PCRE_INCLUDES}" )

add_library( XrdCmsJson MODULE src/XrdCmsJson.cc src/XrdCmsJson.hh )
target_link_libraries(XrdCmsJson ${XROOTD_UTILS} ${XERCES_LIB} ${PCRE_LIB})

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

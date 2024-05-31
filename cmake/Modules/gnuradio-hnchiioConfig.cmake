find_package(PkgConfig)

PKG_CHECK_MODULES(PC_GR_HNCHIIO gnuradio-hnchiio)

FIND_PATH(
    GR_HNCHIIO_INCLUDE_DIRS
    NAMES gnuradio/hnchiio/api.h
    HINTS $ENV{HNCHIIO_DIR}/include
        ${PC_HNCHIIO_INCLUDEDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/include
          /usr/local/include
          /usr/include
)

FIND_LIBRARY(
    GR_HNCHIIO_LIBRARIES
    NAMES gnuradio-hnchiio
    HINTS $ENV{HNCHIIO_DIR}/lib
        ${PC_HNCHIIO_LIBDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/lib
          ${CMAKE_INSTALL_PREFIX}/lib64
          /usr/local/lib
          /usr/local/lib64
          /usr/lib
          /usr/lib64
          )

include("${CMAKE_CURRENT_LIST_DIR}/gnuradio-hnchiioTarget.cmake")

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(GR_HNCHIIO DEFAULT_MSG GR_HNCHIIO_LIBRARIES GR_HNCHIIO_INCLUDE_DIRS)
MARK_AS_ADVANCED(GR_HNCHIIO_LIBRARIES GR_HNCHIIO_INCLUDE_DIRS)

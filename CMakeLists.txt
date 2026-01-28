cmake_minimum_required(VERSION 3.16.3)

project(demo CXX)
set(CMAKE_CXX_STANDARD 17)
option( BUILD_STATIC "Set to ON to include static versions of the library" OFF)

find_package(OpenFHE CONFIG HINTS REQUIRED)
if (OpenFHE_FOUND)
    message(STATUS "FOUND PACKAGE OpenFHE")
    message(STATUS "OpenFHE Version: ${BASE_OPENFHE_VERSION}")
    message(STATUS "OpenFHE installed as shared libraries: ${OpenFHE_SHARED}")
    message(STATUS "OpenFHE include files location: ${OpenFHE_INCLUDE}")
    message(STATUS "OpenFHE lib files location: ${OpenFHE_LIBDIR}")
    message(STATUS "OpenFHE Native Backend size: ${OpenFHE_NATIVE_SIZE}")
    message(STATUS "WITH_INTEL_HEXL:     ${OpenFHE_WITH_INTEL_HEXL}")
    message(STATUS "INTEL_HEXL_HINT_DIR: ${OpenFHE_INTEL_HEXL_HINT_DIR}")
else()
    message(FATAL_ERROR "PACKAGE OpenFHE NOT FOUND")
endif ()

set( CMAKE_CXX_FLAGS ${OpenFHE_CXX_FLAGS} )

include_directories( ${OPENMP_INCLUDES} )
include_directories( ${OpenFHE_INCLUDE} )
include_directories( ${OpenFHE_INCLUDE}/third-party/include )
include_directories( ${OpenFHE_INCLUDE}/core )
include_directories( ${OpenFHE_INCLUDE}/core/include )
include_directories( ${OpenFHE_INCLUDE}/pke )
include_directories( ${OpenFHE_INCLUDE}/binfhe )

if(UNIX AND NOT APPLE)
    add_link_options(-Wl,--no-as-needed)
endif()

if (OpenFHE_WITH_INTEL_HEXL)
    find_package(HEXL HINTS ${OpenFHE_INTEL_HEXL_HINT_DIR} REQUIRED)
    if (NOT TARGET HEXL::hexl)
        message(FATAL_ERROR, "Intel HEXL not found")
    endif()

    get_target_property(INTEL_HEXL_LOCATION HEXL::hexl LOCATION)
    message(STATUS "INTEL_HEXL_LOCATION: ${INTEL_HEXL_LOCATION}")

    get_target_property(INTEL_HEXL_INCLUDE_DIR HEXL::hexl INTERFACE_INCLUDE_DIRECTORIES)
    message(STATUS "INTEL_HEXL_INCLUDE_DIR: ${INTEL_HEXL_INCLUDE_DIR}")

    include_directories(${INTEL_HEXL_INCLUDE_DIR})
endif()

link_directories( ${OpenFHE_LIBDIR} )
link_directories( ${OPENMP_LIBRARIES} )
if(BUILD_STATIC)
    set( CMAKE_EXE_LINKER_FLAGS "${OpenFHE_EXE_LINKER_FLAGS} -static")
    link_libraries( ${OpenFHE_STATIC_LIBRARIES} )
else()
    set( CMAKE_EXE_LINKER_FLAGS ${OpenFHE_EXE_LINKER_FLAGS} )
    link_libraries( ${OpenFHE_SHARED_LIBRARIES} )
endif()

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/data)

include_directories( ${CMAKE_SOURCE_DIR}/include )

add_executable( test
    main.cpp
    src/global.cpp
    src/param.cpp
    src/signal.cpp
    src/setup.cpp
    src/detect.cpp
    src/compress.cpp
    src/decode.cpp
    src/sophomr.cpp
)
target_link_libraries( test ntl gmp m )

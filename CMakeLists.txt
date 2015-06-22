cmake_minimum_required (VERSION 2.8)

project (MeshSkeleton)

set (CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

if(MSVC)
    add_definitions ("-DBOOST_ALL_NO_LIB -D_SCL_SECURE_NO_WARNINGS -D_CRT_SECURE_NO_WARNINGS -DNOMINMAX")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj /EHsc /fp:fast /wd4800 /wd4521 /wd4251 /wd4275 /wd4305 /wd4355 /wd4819 /wd4250 /wd4267")
    # comment following 3 lines to disable global code optimization to speed up compile
    #set (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /GL /wd4800")
    #set (CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} /LTCG")
    #set (CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /LTCG")

    # /MANIFEST:NO") # please, don't disable manifest generation, otherwise crash at start for vs2008
    if(MSVC90 OR MSVC10 OR MSVC11 AND NOT ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION} LESS 2.8 AND NOT ${CMAKE_MINOR_VERSION}.${CMAKE_PATCH_VERSION} LESS 8.6)
    include(ProcessorCount)
    ProcessorCount(N)
    if(NOT N EQUAL 0)
        set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /MP${N} ")
        set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP${N} ")
    endif()
  endif()
endif()

set (MESH_SKELETON_OUTPUT_LIB_DIR ${PROJECT_BINARY_DIR}/lib)
set (MESH_SKELETON_OUTPUT_BIN_DIR ${PROJECT_BINARY_DIR}/bin)
make_directory(${MESH_SKELETON_OUTPUT_LIB_DIR})
make_directory(${MESH_SKELETON_OUTPUT_BIN_DIR})
if(WIN32)
    foreach(config ${CMAKE_CONFIGURATION_TYPES})
        string(TOUPPER ${config} CONFIG)
        set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${CONFIG} "${MESH_SKELETON_OUTPUT_LIB_DIR}")
        set (CMAKE_RUNTIME_OUTPUT_DIRECTORY_${CONFIG} "${MESH_SKELETON_OUTPUT_BIN_DIR}")
        # ---[ Windows requires DLLs (shared libraries) to be installed in the same directory as executables
        set (CMAKE_LIBRARY_OUTPUT_DIRECTORY_${CONFIG} "${MESH_SKELETON_OUTPUT_BIN_DIR}")
    endforeach(config)
else(WIN32)
    set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${MESH_SKELETON_OUTPUT_LIB_DIR}")
    set (CMAKE_RUNTIME_OUTPUT_DIRECTORY "${MESH_SKELETON_OUTPUT_BIN_DIR}")
    set (CMAKE_LIBRARY_OUTPUT_DIRECTORY "${MESH_SKELETON_OUTPUT_LIB_DIR}")
endif(WIN32)

add_subdirectory (MeshSkeleton)

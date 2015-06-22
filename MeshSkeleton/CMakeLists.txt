#Boost
set (Boost_USE_STATIC_LIBS  ON)
set (Boost_USE_MULTITHREADED ON)
set (Boost_USE_STATIC_RUNTIME OFF)
find_package (Boost REQUIRED system thread)
include_directories (${Boost_INCLUDE_DIRS})

#QT
find_package (Qt4 REQUIRED QtCore QtGui QtOpenGL QtXml)
include (${QT_USE_FILE})

#CGAL
find_package (CGAL REQUIRED)
include_directories (${CGAL_INCLUDE_DIRS})
link_directories (${CGAL_LIBRARIES_DIR})
add_definitions (${CGAL_DEFINITIONS})
set (CMAKE_MODULE_PATH ${CGAL_DIR}/cmake/modules)
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CGAL_DIR})
include (CGAL_Macros)

#TAUCS
find_package (TAUCS REQUIRED)
include_directories (${TAUCS_INCLUDE_DIR})
link_directories (${TAUCS_LIBRARIES_DIR})

#glut & QGLViewer
include_directories (${PROJECT_SOURCE_DIR}/3rdParty/include)
link_directories (${PROJECT_SOURCE_DIR}/3rdParty/lib)

include_directories (${CMAKE_CURRENT_BINARY_DIR})
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/algorithm/include)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/geometry/include)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/geometry/kdtree)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/basic/include)

# Set Qt files and resources here
set (uis resources/mainwindow.ui)
set (moc_incs
     include/mainwindow.h
     include/paint_canvas.h
     )
set (resources resources/mainwindow.qrc)

# Qt stuff
QT4_WRAP_UI (ui_srcs ${uis})
QT4_WRAP_CPP (moc_srcs ${moc_incs} OPTIONS -DBOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION)
QT4_ADD_RESOURCES (resource_srcs ${resources})

set (basic_incs
     basic/include/basic_types.h
     basic/include/counted.h
     basic/include/file_system.h
     basic/include/line_stream.h
     basic/include/smart_pointer.h
     basic/include/stop_watch.h
     )

set (basic_srcs
     basic/src/basic_types.cpp
     basic/src/counted.cpp
     basic/src/file_system.cpp
     basic/src/stop_watch.cpp
     )

set (geometry_incs
     geometry/include/attribute.h
     geometry/include/box_3d.h
     geometry/include/box_facet.h
     geometry/include/color.h
     geometry/include/colormap.h
     geometry/include/cylinder.h
     geometry/include/facet.h
     geometry/include/facet_edge.h
     geometry/include/file_io.h
     geometry/include/geometry_global.h
     geometry/include/geometry_types.h
     geometry/include/mesh.h
     geometry/include/model.h
     geometry/include/normalizer.h
     geometry/include/object.h
     geometry/include/point_set.h
     geometry/include/prism.h
     geometry/include/skeleton.h
     geometry/include/vertex.h
     )

set (geometry_srcs
     geometry/src/box_3d.cpp
     geometry/src/box_facet.cpp
     geometry/src/cylinder.cpp
     geometry/src/facet.cpp
     geometry/src/file_io.cpp
     geometry/src/geometry_global.cpp
     geometry/src/mesh.cpp
     geometry/src/model.cpp
     geometry/src/normalizer.cpp
     geometry/src/object.cpp
     geometry/src/point_set.cpp
     geometry/src/prism.cpp
     geometry/src/skeleton.cpp
     )

set (kdtree
    geometry/kdtree/vector2D.h
    geometry/kdtree/vector3D.h
    geometry/kdtree/PriorityQueue.h
    geometry/kdtree/kdTree.h
    geometry/kdtree/kdTree.cpp
    geometry/kdtree/QueryGrid.h
    )

set (algorithm_incs
     algorithm/include/center_of_points.h
     algorithm/include/normal_estimator.h
     algorithm/include/primitive_fitting.h
     )

set (algorithm_srcs
     algorithm/src/center_of_points.cpp
     algorithm/src/normal_estimator.cpp
     algorithm/src/primitive_fitting.cpp
     )

set (incs
     ${moc_incs}
     include/mainwindow.h
     include/paint_canvas.h
     include/select_node_tool.h
     include/select_slice_tool.h
     include/tool_manager3d.h
     include/tool_name.h
     include/tool3D.h
     )

set (srcs
     src/mainwindow.cpp
     src/paint_canvas.cpp
     src/select_node_tool.cpp
     src/select_slice_tool.cpp
     src/tool_manager3d.cpp
     src/tool3D.cpp
     )

# Organize files
SOURCE_GROUP ("Basic\\inc" FILES ${basic_incs})
SOURCE_GROUP ("Basic\\src" FILES ${basic_srcs})
SOURCE_GROUP ("Geometry\\inc" FILES ${geometry_incs})
SOURCE_GROUP ("Geometry\\src" FILES ${geometry_srcs})
SOURCE_GROUP ("Geometry\\kdtree" FILES ${kdtree})
SOURCE_GROUP ("Algorithm\\inc" FILES ${algorithm_incs})
SOURCE_GROUP ("Algorithm\\src" FILES ${algorithm_srcs})
SOURCE_GROUP ("Algorithm\\Pinocchio" FILES ${Pinocchio})
SOURCE_GROUP ("Resources" FILES ${uis} ${resources})
SOURCE_GROUP ("Generated" FILES ${ui_srcs} ${moc_srcs} ${resource_srcs})

SET_SOURCE_FILES_PROPERTIES (${srcs} PROPERTIES OBJECT_DEPENDS "${ui_srcs}")

# Put the ui in the windows project file
if (${CMAKE_BUILD_TOOL} MATCHES "msdev")
    SET (srcs ${srcs} ${uis})
endif (${CMAKE_BUILD_TOOL} MATCHES "msdev")
if (${CMAKE_BUILD_TOOL} MATCHES "devenv")
    SET (srcs ${srcs} ${uis})
endif (${CMAKE_BUILD_TOOL} MATCHES "devenv")

set (exe_name MeshSkeleton)
add_executable (${exe_name} ${ui_srcs} ${moc_srcs} ${resource_srcs} ${srcs} ${geometry_srcs} ${basic_srcs} ${algorithm_srcs} ${incs} ${geometry_incs} ${basic_incs} ${algorithm_incs} ${sparsematrix} ${kdtree})
target_link_libraries (${exe_name} glut64 debug QGLViewerd2 optimized QGLViewer2 ${Boost_THREAD_LIBRARY} ${Boost_SYSTEM_LIBRARY} ${Boost_TIMER_LIBRARY} ${Boost_CHRONO_LIBRARY} ${QT_QTOPENGL_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTXML_LIBRARY} ${QT_QTCORE_LIBRARY} debug  ${CGAL_LIBRIES} blas libtaucs libmetis vcf2c clapack)

if (WIN32 AND MSVC)
    set_target_properties(${exe_name} PROPERTIES LINK_FLAGS_RELEASE /OPT:REF)
elseif (CMAKE_SYSTEMname STREQUAL "Darwin")
    if (NOT CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        set_target_properties (${exe_name} PROPERTIES LINK_FLAGS -Wl)
    endif()
elseif (__COMPILER_PATHSCALE)
    set_target_properties(${exe_name} PROPERTIES LINK_FLAGS -mp)
else()
    set_target_properties(${exe_name} PROPERTIES LINK_FLAGS -Wl,--as-needed)
endif()

set_target_properties (${exe_name} PROPERTIES DEBUG_POSTFIX _debug)
set_target_properties (${exe_name} PROPERTIES RELEASE_POSTFIX _release)
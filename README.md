# Mesh Skeleton Tool
This utility which based on Qt provides interactive contorl to generate mesh skeleton manually. It's part of my undergraduate graduation project "Skeleton-based Deformation".
<img src="https://raw.githubusercontent.com/LemonChiu/Mesh-Skeleton-Tool/master/screenshot/screenshot.jpg" align="left" width="800">

### Dependency 
+ BOOST
+ QT
+ CGAL
+ GLUT
+ libQGLViewer

The x64 version of GLUT and QGLViewer are already in 3rdParty folder. However, BOOST, QT and CGAL still need to be installed separately. This project(x64) has been built successfuly under Visual Studio 2012 with QT4.8.4, Boost 1.53, CGAL4.1 and QGLViewer2.3.17.

### Compile
+ Open CMake(cmake-gui).
+ Choose "Where is the source code:".
+ Choose "Where to build the binaries:".
+ Click Configure. You will be asked to create the directory (yes), then for your VS version.
+ Click Generate. A VS solution will in the generated build folder.
+ In Visual Studio click on the MeshSkeleton project in the solution explorer, select "Set as StartUp Project" and build.
 
### Usage
+ The tool supports .obj and .ply model files.
+ The original .cg files are easy to read and store the skeleton data. 
+ Both model and skeleton files can be drag into the window to load.

### License
Licensed under The [GNU GPL v2.0](https://github.com/LemonChiu/Mesh-Skeleton-Tool/blob/master/LICENSE) License
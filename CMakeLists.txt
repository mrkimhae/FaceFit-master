cmake_minimum_required(VERSION 2.8)
 
PROJECT(QtFaceFitting)
 

#find_package(OpenCV REQUIRED)

FIND_PACKAGE(OpenGL REQUIRED)
if(NOT OPENGL_FOUND)
    message(ERROR " OPENGL not found!")
endif(NOT OPENGL_FOUND)

FIND_PACKAGE(Qt4 4.8.6 REQUIRED)

SET(QT_USE_QTOPENGL TRUE) 

INCLUDE(${QT_USE_FILE})  # include UseQt4.cmake

# support for out-of-source build
INCLUDE_DIRECTORIES(
  ${CMAKE_CURRENT_BINARY_DIR} #this is where ui_SimpleViewUI.h is generated
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${QT_QTOPENGL_INCLUDE_DIR} 
  ${OPENGL_INCLUDE_DIR} 
  ${CMAKE_SOURCE_DIR}/lib/local/CLM/include
  ${CMAKE_SOURCE_DIR}/lib/3rdParty/OpenCV3.0/include/opencv
  ${CMAKE_SOURCE_DIR}/lib/3rdParty/OpenCV3.0/include
  ${CMAKE_SOURCE_DIR}/lib/3rdParty/tbb/include
  ${CMAKE_SOURCE_DIR}/lib/3rdParty/dlib/include/dlib/..
  ${CMAKE_SOURCE_DIR}/lib/3rdParty/boost
  ${CMAKE_SOURCE_DIR}/lib/3rdParty/boost/boost
)

LINK_DIRECTORIES(
   ${CMAKE_SOURCE_DIR}/lib/3rdParty/boost/x86/vc11/lib
   ${CMAKE_SOURCE_DIR}/lib/local/lib
   ${CMAKE_SOURCE_DIR}/lib/3rdParty/tbb/lib/$(PlatformShortName)/$(PlatformToolset)
   ${CMAKE_SOURCE_DIR}/lib/3rdParty/OpenCV3.0/x86/$(PlatformToolset)/lib
)
 
# Set your files and resources here
SET(SimpleViewSrcs main.cpp mainwindow.cpp glwidget.cpp facePCA.cpp clm.cpp qtcvutility.cpp)
#SET(SimpleViewUI qtprogramwin.ui)
SET(SimpleViewHeaders mainwindow.h glwidget.h facePCA.h clm.h qtcvutility.h)
 
QT4_WRAP_UI(UISrcs ${SimpleViewUI})
QT4_WRAP_CPP(MOCSrcs ${SimpleViewHeaders} )
 
SOURCE_GROUP("Resources" FILES
  ${SimpleViewUI}
)
 
SOURCE_GROUP("Generated" FILES
  ${UISrcs}
  ${MOCSrcs}
)
 
ADD_EXECUTABLE( QtFaceFitting ${SimpleViewSrcs} ${UISrcs} ${MOCSrcs} ${SimpleViewHeaders} )
TARGET_LINK_LIBRARIES( QtFaceFitting  ${QT_LIBRARIES} ${OPENGL_LIBRARIES} opencv_world300.lib
tbb.lib
libboost_filesystem-vc110-mt-1_54.lib
kernel32.lib
user32.lib
gdi32.lib
winspool.lib
comdlg32.lib
advapi32.lib
shell32.lib
ole32.lib
oleaut32.lib
uuid.lib
odbc32.lib
odbccp32.lib
CLM.lib
dlib.lib
FaceAnalyser.lib
 )
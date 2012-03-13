CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT (CS348a_src)

INCLUDE_DIRECTORIES(../minmaxer/source.triangulation)

LINK_DIRECTORIES(/home/per/Dev/cs348a/project.git/lib)

ADD_EXECUTABLE(main Triangle IntersectionInfo Ray BezierCurve terrain Main)

TARGET_LINK_LIBRARIES(main GL GLU glut minmaxer)

INSTALL (TARGETS main DESTINATION bin)


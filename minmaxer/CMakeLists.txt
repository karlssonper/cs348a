CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT (CS348a_minmaxer)

INCLUDE_DIRECTORIES(source.triangulation
                    source.minmaxer)
                    
ADD_LIBRARY(minmaxer source.triangulation/angle
                     source.triangulation/angle.sos
                     source.triangulation/bitvector
                     source.triangulation/delaunay
                     source.triangulation/edgeinsert
                     source.triangulation/file_io
                     source.triangulation/flips
                     source.triangulation/graph
                     source.triangulation/hdag
                     source.triangulation/heap
                     source.triangulation/height
                     source.triangulation/heuristic.angle
                     source.triangulation/heuristic.height
                     source.triangulation/heuristic.slope
                     source.triangulation/longmath
                     source.triangulation/novisual
                     source.triangulation/persistent.quadedge
                     source.triangulation/planesweep
                     source.triangulation/quadedge
                     source.triangulation/queue
                     source.triangulation/quicksort
                     source.triangulation/regular
                     source.triangulation/slope
                     source.triangulation/sos
                     source.triangulation/stack
                     source.triangulation/timer
                     source.triangulation/triangulation
                     )

TARGET_LINK_LIBRARIES(minmaxer m)

INSTALL (TARGETS minmaxer DESTINATION lib)

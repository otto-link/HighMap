include(delaunator.cmake)
include(dkm.cmake)
include(FastNoiseLite.cmake)
include(hmm.cmake)
include(libnpy.cmake)
include(macro-logger.cmake)

add_subdirectory(Noise/NoiseLib)

if(HIGHMAP_ENABLE_OPENCL)
  add_subdirectory(CLWrapper)
endif()

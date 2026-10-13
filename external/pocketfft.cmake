include(FetchContent)

FetchContent_Declare(
  pocketfft
  GIT_REPOSITORY https://gitlab.mpcdf.mpg.de/mtr/pocketfft.git
  GIT_TAG cpp
  GIT_SHALLOW TRUE)

FetchContent_MakeAvailable(pocketfft)

if(NOT TARGET pocketfft::pocketfft)
  add_library(pocketfft INTERFACE)
  add_library(pocketfft::pocketfft ALIAS pocketfft)
  target_include_directories(pocketfft INTERFACE ${pocketfft_SOURCE_DIR})
endif()

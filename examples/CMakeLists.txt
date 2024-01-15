file(
  GLOB EXAMPLES
  LIST_DIRECTORIES true
  "*")
foreach(item ${EXAMPLES})
  if(IS_DIRECTORY ${item})
    add_subdirectory(${item})
  endif()
endforeach()

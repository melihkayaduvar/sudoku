# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\sudoku_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\sudoku_autogen.dir\\ParseCache.txt"
  "sudoku_autogen"
  )
endif()

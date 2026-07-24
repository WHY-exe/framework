# the name of the target operating system
set(CMAKE_SYSTEM_NAME Windows)

# which compilers to use for C and C++
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static -static-libgcc -static-libstdc++" CACHE STRING "" FORCE)
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -static -static-libgcc -static-libstdc++" CACHE STRING "" FORCE)
# set(CMAKE_CXX_FLAGS "-m32" CACHE STRING "32bit compile flags" FORCE)
# set(CMAKE_C_FLAGS "-m32" CACHE STRING "32bit compile flags" FORCE)
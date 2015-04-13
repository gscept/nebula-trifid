#-------------------------------------------------------------------------------
# StackWalker
#-------------------------------------------------------------------------------

SOURCE_GROUP(StackWalker FILES StackWalker.h StackWalker.cpp)

ADD_LIBRARY(StackWalker STATIC StackWalker.h StackWalker.cpp)
TARGET_INCLUDE_DIRECTORIES(StackWalker PUBLIC ${Nebula3_SOURCE_DIR}/code/extlibs/stackwalker)
SET_TARGET_PROPERTIES(StackWalker PROPERTIES FOLDER "extlibs")

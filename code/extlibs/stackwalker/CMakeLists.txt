#-------------------------------------------------------------------------------
# StackWalker
#-------------------------------------------------------------------------------
INCLUDE_DIRECTORIES(
    ${Nebula3_SOURCE_DIR}/code/extlibs/stackwalker
)


SOURCE_GROUP(StackWalker FILES StackWalker.h StackWalker.cpp)

ADD_LIBRARY(StackWalker STATIC StackWalker.h StackWalker.cpp)
SET_TARGET_PROPERTIES(StackWalker PROPERTIES FOLDER "extlibs")

#-------------------------------------------------------------------------------
# effect11
#-------------------------------------------------------------------------------
INCLUDE_DIRECTORIES(
    ${Nebula3_SOURCE_DIR}/code/foundation/
    ${Nebula3_SOURCE_DIR}/code/extlibs/
)
SET(extlibs_effects11_SRCS
d3dx11dbg.cpp
d3dx11dbg.h
d3dx11effect.h
d3dxGlobal.cpp
d3dxGlobal.h
Effect.h
EffectAPI.cpp
EffectLoad.cpp
EffectLoad.h
EffectNonRuntime.cpp
EffectReflection.cpp
EffectRuntime.cpp
EffectVariable.inl
EffectBinaryFormat.h
EffectStateBase11.h
EffectStates11.h
SOParser.h
)

SOURCE_GROUP(Effects11 FILES ${extlibs_effects11_SRCS})
ADD_LIBRARY(Effects11 STATIC ${extlibs_effects11_SRCS})
SET_TARGET_PROPERTIES(Effects11 PROPERTIES FOLDER "extlibs")

N_SET_COMPILER_DEFAULTS()
ADD_DEPENDENCIES(Effects11 foundation)

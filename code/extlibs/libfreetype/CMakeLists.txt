#------------------------------------------------------------------------------
# libfreetype
#-------------------------------------------------------------------------------

INCLUDE_DIRECTORIES(${Nebula3_SOURCE_DIR}/code/extlibs/libfreetype)

SET(SRC 	  "base/ftsystem.c"
			  "base/ftinit.c"
			  "base/ftdebug.c"
			  "base/ftbase.c"
			  "base/ftbbox.c"
			  "base/ftglyph.c"
			  "base/ftbdf.c"
			  "base/ftbitmap.c"
			  "base/ftcid.c"
			  "base/ftfstype.c"
			  "base/ftgasp.c"
			  "base/ftgxval.c"
			  "base/ftlcdfil.c"
			  "base/ftmm.c"
			  "base/ftotval.c"
			  "base/ftpatent.c"
			  "base/ftpfr.c"
			  "base/ftstroke.c"
			  "base/ftsynth.c"
			  "base/fttype1.c"
			  "base/ftwinfnt.c"
			  "base/ftxf86.c"
			  "bdf/bdf.c"
			  "cff/cff.c"   
			  "cid/type1cid.c"
			  "pcf/pcf.c"
			  "pfr/pfr.c"    
			  "sfnt/sfnt.c"
			  "truetype/truetype.c"
			  "type1/type1.c" 
			  "type42/type42.c"
			  "winfonts/winfnt.c"
			  "raster/raster.c"
			  "smooth/smooth.c"
			  "autofit/autofit.c"
			  "cache/ftcache.c"
			  "gzip/ftgzip.c"
			  "lzw/ftlzw.c"
			  "gxvalid/gxvalid.c"
			  "otvalid/otvalid.c"
			  "psaux/psaux.c"
			  "pshinter/pshinter.c"
			  "psnames/psnames.c")
FILE(GLOB_RECURSE HEAD "*.h")
ADD_LIBRARY(libfreetype STATIC ${SRC} ${HEAD})
ADD_DEFINITIONS(/DFT2_BUILD_LIBRARY)

SET_TARGET_PROPERTIES(libfreetype PROPERTIES FOLDER "extlibs")
N_SET_COMPILER_DEFAULTS()

SET(CMAKE_C_FLAGS "/D__WIN32__ /DWIN32 /D_HAS_EXCEPTIONS=0 /GF /fp:fast /W1 /WX /nologo /errorReport:prompt"  )
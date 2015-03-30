/* ionebula3.h -- IO base function header for compress/uncompress .zip
   files using zlib + zip or unzip API
   This IO API version uses the Nebula3 file API.

   (C) 2007 Radon Labs GmbH
*/
#ifdef __cplusplus
extern "C" {
#endif

void fill_nebula3_filefunc OF((zlib_filefunc_def* pzlib_filefunc_def));

#ifdef __cplusplus
}
#endif

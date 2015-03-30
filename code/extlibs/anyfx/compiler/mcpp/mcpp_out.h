/* mcpp_out.h: declarations of OUTDEST data types for MCPP  */
#ifndef _MCPP_OUT_H
#define _MCPP_OUT_H

#ifdef OUT
#undef OUT
#endif

#ifdef ERR
#undef ERR
#endif

#ifdef DBG
#undef DBG
#endif

#ifdef NUM_OUTDEST
#undef NUM_OUTDEST
#endif

/* Choices for output destination */
typedef enum {
    OUT,                        /* ~= fp_out    */
    ERR,                        /* ~= fp_err    */
    DBG,                        /* ~= fp_debug  */
    NUM_OUTDEST
} OUTDEST;

#endif  /* _MCPP_OUT_H  */

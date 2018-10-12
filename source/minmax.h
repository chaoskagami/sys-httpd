#ifndef UTIL_MINMAX_H
#define UTIL_MINMAX_H

// Basic typeof implementations of min and max, since the C standard
// does not provide them.

#ifdef max
  #undef max
#endif

#ifdef min
  #undef min
#endif

 #define max(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })

 #define min(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a < _b ? _a : _b; })
#endif

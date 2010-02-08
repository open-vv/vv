#ifndef CLITKPORTABILITY_H
#define CLITKPORTABILITY_H
  
#if defined(WIN32)
#  define rint(x)  floor(x+0.5)
#  define lrint(x) (long)rint(x) 
#  define _USE_MATH_DEFINES
#endif

#endif /* end #define CLITKPORTABILITY_H */

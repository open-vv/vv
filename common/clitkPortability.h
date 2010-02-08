#ifndef CLITKPORTABILITY_H
#define CLITKPORTABILITY_H

#if defined(WIN32)
#  define rint(x)  floor(x+0.5)
#  define lrint(x) (long)rint(x) 
#  define _USE_MATH_DEFINES
#endif

template<typename T>
int IsNormal(const T val) {
#if defined(_MSC_VER)
  return _finite(val);
#elif defined(unix)
  return std::isnormal(val);
#endif
}

#endif /* end #define CLITKPORTABILITY_H */

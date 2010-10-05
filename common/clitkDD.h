#ifndef clitkDD_h
#define clitkDD_h

#include <iostream>

// David's debug

#define DD(a) std::cout << #a " = [ " << a << " ]" << std::endl;std::cout.flush();
#define DDV(a,n) { std::cout << #a " = [ "; for(unsigned int _i_=0; _i_<n; _i_++) { std::cout << a[_i_] << " "; }; std::cout << " ]" << std::endl;std::cout.flush();}
  template<class T>
    void _print_container(T const& a)
    { for(typename T::const_iterator i=a.begin();i!=a.end();++i) { std::cout << *i << " "; };}
#define DDS(a) { std::cout << #a " = [ "; _print_container(a) ; std::cout << " ]" << std::endl;std::cout.flush():}

#endif

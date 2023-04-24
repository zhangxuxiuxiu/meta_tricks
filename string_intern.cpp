//#pragma once
#include <iostream>

//https://github.com/alexpolt/luple/blob/master/intern.h

#define N3599

namespace intern {

  template<char... NN> struct string {

    static constexpr char const value[ sizeof...(NN) ]{NN...};

    static_assert( value[ sizeof...(NN) - 1 ] == '\0', "interned string was too long, see $(...) macro" );

    static constexpr auto data() { return value; }
  };

  template<char... N> constexpr char const string<N...>::value[];
 
  template<int N>
  constexpr char ch ( char const(&s)[N], int i ) { return i < N ? s[i] : '\0'; }

  template<typename T> struct is_string {
    static const bool value = false;
  };

  template<char... NN> struct is_string< string<NN...> > {
    static const bool value = true;
  };

}


#if !defined( _MSC_VER ) && defined( N3599 )

  template<typename T, T... C>
  auto operator ""_intern() {
    return intern::string<C..., T{}>{};
  }

  #define $( s )  decltype( s ## _intern )

#else

  //prefixing macros with a $ helps with readability
  #define $c( a, b ) intern::ch( a, b )

  //10 characters + '\0', add $c(...) for bigger strings
  #define $( s ) intern::string< $c(s,0),$c(s,1),$c(s,2),$c(s,3),$c(s,4),$c(s,5),$c(s,6),$c(s,7),$c(s,8),$c(s,9),$c(s,10) >

#endif

template<class, int>
struct Uniquer{};

//template<class FileName= $(__FILE__), int N =__LINE__>
//using Uq = Uniquer<FileName,N>;

#define Uq Uniquer(__FILE__,__LINE__)
#define Uniquer(F,N) Uniquer<$(F),N>

int main(){
	std::cout  << typeid(Uq).name() << '\n' ;
	std::cout  << typeid(Uq).name() << '\n' ;

}

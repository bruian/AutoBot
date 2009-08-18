#ifndef ___BASE_TYPES_H___
#define ___BASE_TYPES_H___

namespace base {
    //
    // Platform-independed types
    //
    typedef signed char         sint8;
    typedef unsigned char       uint8;
    typedef signed short        sint16;
    typedef unsigned short      uint16;
    typedef signed long         sint32;
    typedef unsigned long       uint32;
    
    //
    // Platfrom-independed, but not guaranteed
    //
    typedef signed __int64      sint64;
    typedef unsigned __int64    uint64;
    
    //
    // Platform-optimized types
    //
    typedef signed int          sint;
    typedef unsigned int        uint;
}

using base::sint64;
using base::uint64;
using base::sint32;
using base::uint32;
using base::sint16;
using base::uint16;
using base::sint8;
using base::uint8;
using base::sint;
using base::uint;


//
// The are 3 types of chars: char (single byte), wchar (unicode), xchar (char or wchar - switchable)
//

    typedef wchar_t wchar;

#if(jkUNICODE)
    typedef wchar xchar;
#   define xSTRING(text) L##text

#   define WIDEN2(x) L ## x
#   define WIDEN(x) WIDEN2(x)
#   define __STRINGFILE__       WIDEN(__FILE__)
#   define __STRINGTIMESTAMP__  WIDEN(__TIMESTAMP__)
#   define __STRINGFUNCSIG__    WIDEN(__FUNCSIG__)
#else 
    typedef char xchar;
#   define xSTRING(text) text

#   define __STRINGFILE__       __FILE__
#   define __STRINGTIMESTAMP__  __TIMESTAMP__
#   define __STRINGFUNCSIG__    __FUNCSIG__
#endif

//
// String defines
//

#endif
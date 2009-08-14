#ifndef ___SCRIPT_DETAIL_LANGUAGE___
#define ___SCRIPT_DETAIL_LANGUAGE___

#include <script/custom/CustomSystem.h>

#if defined(jkSCRIPT_LANGUAGE_IS_SQUIRREL)
//
// language is: squirrel
//
#pragma warning(push)
#pragma warning(disable: 4231 4250 4251 4275 4660 4661 4700 4706 4786 4355 4100 4244 4127)
#   include <squirrel.h>
#pragma warning(pop)
//
#else 
//
// language is: none
//
#   error unsupported script language
//
#endif

#endif//___SCRIPT_DETAIL_LANGUAGE___
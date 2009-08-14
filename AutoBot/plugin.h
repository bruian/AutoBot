#ifndef PLUGIN_H
#define PLUGIN_H
/*
	plugin.h
	Plugin API for AutoBot
*/

/*
Copyright (c) 2008 Bruian aka Dergach Viktor
All rights reserved.
 www.bruian.com

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:
1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
3. The name of the authors may not be used to endorse or promote products
   derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

EXCEPTION:
AutoBot plugins that use this header file can be distributed under any
other possible license with no implications from the above license on them.
*/
//**************************************************
#define AUTOBOTVERSION_MAJOR 0
#define AUTOBOTVERSION_MINOR 5
#define AUTOBOTVERSION_BUILD 10

#define MAKEAUTOBOTVERSION(major,minor,build) ( ((major)<<8) | (minor) | ((build)<<16))

#define AUTOBOTVERSION MAKEAUTOBOTVERSION(AUTOBOTVERSION_MAJOR, AUTOBOTVERSION_MINOR, AUTOBOTVERSION_BUILD)
//**************************************************
#include <wx/wx.h>
#include <wx/msw/private.h>

#include <script/VMCore.h>
#include <script/Declarator.h>
//**************************************************
#define IMPLEMENT_PLUGIN(name)														\
	extern "C" __declspec(dllexport) Plugin* CreatePlugin(){return  new name;}		\
																					\
	class name ## DLLApp : public wxApp												\
	{																				\
		bool OnInit() {return true;}												\
	};																				\
	IMPLEMENT_APP_NO_MAIN(name ## DLLApp)											\
	DECLARE_APP(name ## DLLApp)														\
																					\
	BOOL APIENTRY DllMain( HANDLE hModule,											\
                       DWORD  ul_reason_for_call,									\
                       LPVOID lpReserved											\
	                                         )										\
	{																				\
		    switch (ul_reason_for_call)												\
			{																		\
                case DLL_PROCESS_ATTACH:											\
				{       wxSetInstance((HINSTANCE)hModule);							\
                        int argc = 0;												\
                        char **argv = NULL;											\
                        wxEntryStart(argc, argv);									\
						if ( !wxTheApp || !wxTheApp->CallOnInit() )					\
							return FALSE;											\
						return TRUE;												\
                }																	\
                break;																\
																					\
                case DLL_THREAD_ATTACH:												\
                break;																\
																					\
                case DLL_THREAD_DETACH:												\
                break;																\
																					\
                case DLL_PROCESS_DETACH:											\
                 wxEntryCleanup();													\
                break;																\
			}																		\
		return TRUE;																\
	}																				\
//**************************************************
//# if defined(WXUSINGDLL) && (defined(WXMAKING_MY_DLL) || defined(WXUSING_MY_DLL))
#	if defined(WXMAKING_MY_DLL)
#		define WXP_EXPORTED_FROM_APP_TO_PLUGIN	WXEXPORT // When building the DLL exports classes
#	elif defined(WXUSING_MY_DLL)
#		define WXP_EXPORTED_FROM_APP_TO_PLUGIN	WXIMPORT // When building the DLL imports classes
#	endif
//# else
//#	define WXP_EXPORTED_FROM_APP_TO_PLUGIN
//# endif
//**************************************************
struct PluginStartupInfo
{
	int StructSize;
	const wchar_t *PluginName;
	const wchar_t *ComponentName;
	const wchar_t *ComponentGUID;
	const wchar_t *ComponentAutor;
	INT_PTR MajorVers;
	INT_PTR MinorVers;
	INT_PTR ReleaseVers;
};
//**************************************************
//define the interface for the plugin
class WXP_EXPORTED_FROM_APP_TO_PLUGIN Plugin : public wxObject
{
	DECLARE_ABSTRACT_CLASS(Plugin)
public:
	virtual void GetPluginInfo(struct PluginStartupInfo *Info) = 0;
	virtual void InitClass(script::VMCore *core) = 0;
};
//define a function pointer type for convenience
#ifndef __PLUGIN_FUNCTION
#define __PLUGIN_FUNCTION
typedef Plugin* ( *CreatePlugin_function)();
#endif //__PLUGIN_FUNCTION
#endif // PLUGIN_H
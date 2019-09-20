#include "RackAFXDLL.h"

///////////////////////////////////////////////////////////
//
// Global variables
//
void* moduleHandle;   // DLL module handle

///////////////////////////////////////////////////////////
//
// DLL module information
//

extern "C"
{
	// entry point DllMain
	BOOL APIENTRY DllMain(HINSTANCE hInst, DWORD dwReason, LPVOID /*lpvReserved*/)
	{
		if(dwReason == DLL_PROCESS_ATTACH)
		{
			moduleHandle = hInst;
		}
		else if(dwReason == DLL_PROCESS_DETACH)
		{
			moduleHandle = nullptr;
		}

		return TRUE ;
	}
}

// --- RackAFX Creation Function
DllExport Rafx2PluginBase* createRafx2Plugin()
{
	Rafx2PluginBase* rafx2Plugin = new Rafx2Plugin();
	return rafx2Plugin;
}

DllExport UINT getAPIVersion()
{
	return CURRENT_PLUGIN_API;
}






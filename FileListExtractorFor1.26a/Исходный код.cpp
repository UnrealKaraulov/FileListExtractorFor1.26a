
#pragma region Includes
// Все WINAPI и прочие функции
#include <Windows.h>
#include <intrin.h>
#include <vector>
#include <tchar.h>
#include <fstream> 
#include <iostream>
#pragma intrinsic(_ReturnAddress)
// Перехват функций
#include <MinHook.h>
#include <string>
using namespace std;
#pragma comment(lib,"libMinHook.x86.lib")
#pragma endregion



FILE * logfile;

void WatcherLog( const char * format, ... )
{
	if ( !logfile )
		return;
	/*char buffer[ 256 ]; va_list args; va_start( args , format );
	vsprintf_s( buffer , 256 , format , args ); va_end( args );
	int lentowrite = strlen( buffer );
	fwrite( &buffer , lentowrite , 1 , logfile ); fflush( logfile );*/
	va_list args; va_start( args, format ); vfprintf_s( logfile, format, args ); va_end( args ); fflush( logfile );
}

HMODULE StormDll;


typedef int( __stdcall * pStorm_279 )( const char*  a1, int a2, int a3, size_t Size, int a5 );
pStorm_279 Storm_279_org;
pStorm_279 Storm_279_ptr;


int __stdcall Storm_279my( const char* a1, int a2, int a3, size_t Size, int a5 )
{
	int StormReturn = Storm_279_ptr( a1, a2, a3, Size, a5 );
	if ( a1 && *a1)
	{
		WatcherLog( "%s\n", a1 );
	}
	//ReplaceIconPathIfNeed( );
	return StormReturn;
}




BOOL __stdcall DllMain( HINSTANCE Module, UINT reason, LPVOID )
{

	if ( reason == DLL_PROCESS_ATTACH )
	{
	
		MH_Initialize( );
		logfile = _fsopen( "Filelist_dump.txt", "a+", _SH_DENYWR );
		StormDll = GetModuleHandle( "Storm.dll" );

		if ( !StormDll )
		{
			MessageBox( 0, "ERROR NO STORM", "NO STORM FOUND", 0 );
		}

		Storm_279_org = ( pStorm_279 ) ( GetProcAddress( StormDll, ( LPCSTR ) 279 ) );
		MH_CreateHook( Storm_279_org, &Storm_279my, reinterpret_cast< void** >( &Storm_279_ptr ) );
		MH_EnableHook( Storm_279_org );


	}
	else if ( reason == DLL_PROCESS_DETACH )
	{
		MH_DisableHook( Storm_279_org );
		MH_Uninitialize( );
	

	}
	return TRUE;
}
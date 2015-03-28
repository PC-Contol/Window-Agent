//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#include "MainUnit.h"
#include "WebServerThread.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------

__fastcall WebServerThread::WebServerThread()
	: TThread(false)
{
	try
	{
		IdHTTPServer1 = new TIdHTTPServer(Application);
		IdHTTPServer1->DefaultPort = 9090;
		IdHTTPServer1->OnCommandGet = IdHTTPServer1CommandGet;
		IdHTTPServer1->Active = true;

	}
	catch(Exception &e)
	{
		Application->Terminate();
	}
}
//---------------------------------------------------------------------------
void __fastcall WebServerThread::Execute()
{
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
void __fastcall WebServerThread::IdHTTPServer1CommandGet(TIdContext *AContext,
	TIdHTTPRequestInfo *ARequestInfo, TIdHTTPResponseInfo *AResponseInfo)
{
	//AResponseInfo->ContentText = "dddddddddddddd";
	AnsiString str;
	str.sprintf("Command %s [%s] received from %s",
		AnsiString(ARequestInfo->Command).c_str(),
		AnsiString(ARequestInfo->Document).c_str(),
		AnsiString(ARequestInfo->RemoteIP).c_str());

//MainForm->Memo1->Lines->Add(str);
//	if(MainForm->Memo1->Perform(EM_LINEFROMCHAR, MainForm->Memo1->SelStart, 0) > 400) MainForm->Memo1->Clear();

	AnsiString LFileName;
	LFileName = ARequestInfo->Document;

	if(LFileName.SubString(2, 3).UpperCase() == "CMD")
	{
		ProcCMD(LFileName.SubString(6, LFileName.Length()), AResponseInfo);
	}

}
//---------------------------------------------------------------------------


void __fastcall WebServerThread::MyShutDownSystem(int iFlag)

{
	HANDLE hToken;
	TOKEN_PRIVILEGES tp;
	LUID luid;
	OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
	LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &luid);

	tp.PrivilegeCount = 1;
	tp.Privileges[0].Luid = luid;
	tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	AdjustTokenPrivileges(hToken, FALSE, &tp, 0, NULL, NULL);
	ExitWindowsEx(iFlag | EWX_FORCE, 0);

}

void __fastcall WebServerThread::ProcCMD(String LFileName, TIdHTTPResponseInfo *AResponseInfo)
{

   if(LFileName.UpperCase() == "SHUTDOWN")
   {
		AResponseInfo->ContentText = "<H1>SHUTDOWN</H1>";
		AResponseInfo->WriteContent();
		MyShutDownSystem(EWX_SHUTDOWN);
   }
   else if(LFileName.UpperCase() == "REBOOT")
   {
		AResponseInfo->ContentText = "<H1>REBOOT</H1>";
		AResponseInfo->WriteContent();
		MyShutDownSystem(EWX_REBOOT);
   }

}




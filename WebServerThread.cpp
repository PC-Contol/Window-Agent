//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#include <DBXJSON.hpp>
#include "MainUnit.h"
#include "WebServerThread.h"
#include "VolumeControlcpp.h"
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
		TJSONObject *result = new TJSONObject();
		result->AddPair(new TJSONPair("result", "success") );
		result->AddPair(new TJSONPair("cmd",LFileName) );

		AResponseInfo->ContentType ="application/json ";
		AResponseInfo->ContentText =  result->ToJSON() ;
		AResponseInfo->WriteContent();

		MyShutDownSystem(EWX_SHUTDOWN);
	}
	else if(LFileName.UpperCase() == "REBOOT")
	{
		TJSONObject *result = new TJSONObject();
		result->AddPair(new TJSONPair("result", "success") );
		result->AddPair(new TJSONPair("cmd",LFileName) );

		AResponseInfo->ContentType ="application/json ";
		AResponseInfo->ContentText =  result->ToJSON() ;
		AResponseInfo->WriteContent();

		MyShutDownSystem(EWX_REBOOT);
	}
	else if(LFileName.UpperCase().SubString(0, 7) == "SET_VOL")
	{
		AnsiString sVol =  LFileName.UpperCase().SubString(9,LFileName.Length() );
		ChangeVolume((double)sVol.ToDouble()/100, true);

		TJSONObject *result = new TJSONObject();
		result->AddPair(new TJSONPair("result", "success") );
		result->AddPair(new TJSONPair("preVolume", GetVolume() * 100 ) );
		result->AddPair(new TJSONPair("setVolume", sVol) );

		AResponseInfo->ContentType ="application/json ";
		AResponseInfo->ContentText =  result->ToJSON() ;
		AResponseInfo->WriteContent();
	}
	else if(LFileName.UpperCase().SubString(0, 7) == "GET_VOL")
	{
		TJSONObject *result = new TJSONObject();
		result->AddPair(new TJSONPair("result", "success") );
		result->AddPair(new TJSONPair("volume", GetVolume()) );

		AResponseInfo->ContentType ="application/json ";
		AResponseInfo->ContentText =  result->ToJSON() ;
		AResponseInfo->WriteContent();
	}
	else if(LFileName.UpperCase() == "GET_RESOLUTION")
	{
		MONITORINFOEXW MonitorInfoEx;

		TJSONObject *resolution = new TJSONObject();
		resolution->AddPair(new TJSONPair("result", "success") );
		resolution->AddPair(new TJSONPair("monitor_count",Screen->MonitorCount) );

		TJSONArray *monitors = new TJSONArray();
		MonitorInfoEx.cbSize = sizeof(MONITORINFOEXW);
		for(int i=0; i<Screen->MonitorCount; i++)
		{
			TJSONObject *monitor = new TJSONObject();
			GetMonitorInfoW(Screen->Monitors[i]->Handle, &MonitorInfoEx);
			monitor->AddPair(new TJSONPair("handle",(const)Screen->Monitors[i]->Handle ));
			monitor->AddPair(new TJSONPair("device",MonitorInfoEx.szDevice));
			monitor->AddPair(new TJSONPair("width",Screen->Monitors[i]->Width));
			monitor->AddPair(new TJSONPair("height",Screen->Monitors[i]->Height));
			monitors->Add(monitor);
		}
		resolution->AddPair("monitor", monitors);
	   	AResponseInfo->ContentType ="application/json ";
		AResponseInfo->ContentText =  resolution->ToJSON() ;
		AResponseInfo->WriteContent();
	}



}



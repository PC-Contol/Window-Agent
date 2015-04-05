//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#include <DBXJSON.hpp>
#include <jpeg.hpp>
#include "MainUnit.h"
#include "WebServerThread.h"
#include "VolumeControl.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------
SystemInfo *si;
__fastcall WebServerThread::WebServerThread(SystemInfo *pSi)
	: TThread(false)
{
	try
	{
		IdHTTPServer1 = new TIdHTTPServer(Application);
		IdHTTPServer1->DefaultPort = 9090;
		IdHTTPServer1->OnCommandGet = IdHTTPServer1CommandGet;
		IdHTTPServer1->Active = true;
		si = pSi;

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

	MainForm->LogMemoAdd(str);
//	if(MainForm->Memo1->Perform(EM_LINEFROMCHAR, MainForm->Memo1->SelStart, 0) > 400) MainForm->Memo1->Clear();

	AnsiString LFileName;
	LFileName = ARequestInfo->Document;
	TStrings *params = 	ARequestInfo->Params;
	MainForm->LogMemoAdd(ARequestInfo->URI);
	ProcCMD(params, AResponseInfo);
	//if(LFileName.SubString(2, 3).UpperCase() == "CMD")
	{
		//ProcCMD(LFileName.SubString(6, LFileName.Length()), AResponseInfo);
		//ProcCMD(params, AResponseInfo);
	}
	//AContext->
	//AContext->Connection->Disconnect();

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

void __fastcall WebServerThread::ProcCMD(TStrings * prams, TIdHTTPResponseInfo *AResponseInfo)
{
	for(int i = 0; i < prams->Count; i++)
		MainForm->LogMemoAdd("[" + IntToStr(i) + "] " + prams->Names[i] + " : " + prams->Values[prams->Names[i]] );

	AnsiString cmd = prams->Values["cmd"].UpperCase();
	TJSONObject *result = new TJSONObject();
	AResponseInfo->ContentEncoding = "UTF-8";
	AResponseInfo->ContentType = "application/json";

	if(cmd == "SHUTDOWN")
	{
		result->AddPair(new TJSONPair("result", "success") );
		result->AddPair(new TJSONPair("cmd",cmd) );

		AResponseInfo->ContentText =  prams->Values["callback"]+"("+result->ToJSON()+")";

		MyShutDownSystem(EWX_SHUTDOWN);
	}
	else if(cmd == "REBOOT")
	{
		result->AddPair(new TJSONPair("result", "success") );
		result->AddPair(new TJSONPair("cmd",cmd) );

		AResponseInfo->ContentText =  prams->Values["callback"]+"("+result->ToJSON()+")";

		MyShutDownSystem(EWX_REBOOT);
	}
	else if(cmd == "SET_VOL")
	{
		AnsiString sVol =  prams->Values["val"];
		ChangeVolume((double)sVol.ToDouble()/100, true);

		result->AddPair(new TJSONPair("result", "success") );
		result->AddPair(new TJSONPair("preVolume", GetVolume() * 100 ) );
		result->AddPair(new TJSONPair("setVolume", sVol) );

		AResponseInfo->ContentText =  prams->Values["callback"]+"("+result->ToJSON()+")";
	}
	else if(cmd == "GET_VOL")
	{
		result->AddPair(new TJSONPair("result", "success") );
		result->AddPair(new TJSONPair("volume", GetVolume()) );


		AResponseInfo->ContentText =  prams->Values["callback"]+"("+result->ToJSON()+")";
	}
	else if(cmd == "GET_RESOLUTION")
	{
		MONITORINFOEXW MonitorInfoEx;

		result->AddPair(new TJSONPair("result", "success") );
		result->AddPair(new TJSONPair("monitor_count",Screen->MonitorCount) );

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
		result->AddPair("monitor", monitors);

		AResponseInfo->ContentText =  prams->Values["callback"]+"("+result->ToJSON()+")";
	}
	else if(cmd == "GET_DESKTOP.JPG")
	{
		int Monitor = 0;
		Graphics::TBitmap *bmp = new Graphics::TBitmap;
		TJPEGImage* jpg = new TJPEGImage();
		TStream *ms = new TMemoryStream();
		try
		{
			bmp->Width = Screen->Monitors[Monitor]->Width / 2;
			bmp->Height = Screen->Monitors[Monitor]->Height / 2;
			bmp->PixelFormat = pf24bit;

			SetStretchBltMode(bmp->Canvas->Handle, HALFTONE);
			StretchBlt(bmp->Canvas->Handle, 0, 0, bmp->Width, bmp->Height, GetDC(0),
				Screen->Monitors[Monitor]->Left,  Screen->Monitors[Monitor]->Top,
				Screen->Monitors[Monitor]->Width, Screen->Monitors[Monitor]->Height,
				SRCCOPY);

			bmp->Canvas->Brush->Color = 0x000000;
			bmp->Canvas->Brush->Style << bsSolid;
			bmp->Canvas->Font->Size  = 11;
			bmp->Canvas->Font->Style << fsBold;
			bmp->Canvas->Font->Color = 0xffffff;
			bmp->Canvas->TextOut(1, 1, " [] " + FormatDateTime("yyyy/mm/dd hh:nn:ss", Now()));

			jpg->Assign(bmp);
			jpg->CompressionQuality = 80;
			jpg->Compress();
			jpg->SaveToStream(ms);

			AResponseInfo->ContentType ="image/jpeg";
			AResponseInfo->ContentStream = ms;
		}
		__finally
		{
			jpg->Free();
			bmp->Free();
		}
	}
	else if(cmd == "GET_SYSTEMINFO")
	{
		result->AddPair(new TJSONPair("result", "success") );
		result->AddPair(new TJSONPair("cpu", si->UseCpu) );
		result->AddPair(new TJSONPair("rem-total", si->Mem.Total) );
		result->AddPair(new TJSONPair("rem-use", si->Mem.Use) );
		result->AddPair(new TJSONPair("hdd-total", si->Hdd.Total) );
		result->AddPair(new TJSONPair("hdd-use", si->Hdd.Use) );
		AResponseInfo->ContentText =  prams->Values["callback"]+"("+result->ToJSON()+")";
	}
	else if(cmd == "SET_MONITOR_ON")
	{
		result->AddPair(new TJSONPair("result", "success") );
		result->AddPair(new TJSONPair("cmd",cmd) );

		AResponseInfo->ContentText =  prams->Values["callback"]+"("+result->ToJSON()+")";

		SendMessage(HWND_BROADCAST, WM_SYSCOMMAND, SC_MONITORPOWER, -1);
	}
	else if(cmd == "SET_MONITOR_OFF")
	{

		result->AddPair(new TJSONPair("result", "success") );
		result->AddPair(new TJSONPair("cmd",cmd) );

		AResponseInfo->ContentText =  prams->Values["callback"]+"("+result->ToJSON()+")";

		SendMessage(HWND_BROADCAST, WM_SYSCOMMAND, SC_MONITORPOWER, 2);
	}
	else if(cmd == "GET_MONITOR_ONOFF")
	{
		result->AddPair(new TJSONPair("result", "success") );
		result->AddPair(new TJSONPair("monitoer", "") );

		AResponseInfo->ContentText =  prams->Values["callback"]+"("+result->ToJSON()+")";
	}
	else
	{
		result->AddPair(new TJSONPair("result", "failed") );

		AResponseInfo->ResponseNo = 404;
		AResponseInfo->ContentText =  prams->Values["callback"]+"("+result->ToJSON()+")";
	}


}




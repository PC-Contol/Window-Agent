//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#include <DBXJSON.hpp>
#include <jpeg.hpp>
#include "MainUnit.h"
#include "WebServerThread.h"
#include "VolumeControlcpp.h"
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
	else if(LFileName.UpperCase() == "GET_DESKTOP.JPG")
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
	else if(LFileName.UpperCase() == "GET_SYSTEMINFO")
	{
 		TJSONObject *result = new TJSONObject();
		result->AddPair(new TJSONPair("result", "success") );
		result->AddPair(new TJSONPair("cpu", si->UseCpu) );
		result->AddPair(new TJSONPair("rem-total", si->Mem.Total) );
		result->AddPair(new TJSONPair("rem-use", si->Mem.Use) );
		result->AddPair(new TJSONPair("hdd-total", si->Hdd.Total) );
		result->AddPair(new TJSONPair("hdd-use", si->Hdd.Use) );
		AResponseInfo->ContentType ="application/json";
	   //	AResponseInfo->ResponseNo = 404;
		AResponseInfo->ContentText =  result->ToJSON() ;
		AResponseInfo->WriteContent();
    }
	else
	{
		TJSONObject *result = new TJSONObject();
		result->AddPair(new TJSONPair("result", "failed") );
		AResponseInfo->ContentType ="application/json";
		AResponseInfo->ResponseNo = 404;
		AResponseInfo->ContentText =  result->ToJSON() ;
		AResponseInfo->WriteContent();
	}



}



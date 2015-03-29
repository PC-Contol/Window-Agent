//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "MainUnit.h"
#include "WebServerThread.h"
#include "SystemInfoThread.h"
#include "About.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TMainForm *MainForm;
WebServerThread *wst;
SystemInfoThread *sit;

//---------------------------------------------------------------------------
__fastcall TMainForm::TMainForm(TComponent* Owner)
	: TForm(Owner)
{

	Application->ShowMainForm = False;
	this->Visible = True;
	Application->Minimize();
	ShowWindow(Application->Handle, SW_HIDE);
	Application->OnMinimize = OnAppMinimize;

}
//---------------------------------------------------------------------------
void __fastcall TMainForm::N41Click(TObject *Sender)
{
	this->Close();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::N11Click(TObject *Sender)
{
	wst->MyShutDownSystem(EWX_REBOOT);
}
//---------------------------------------------------------------------------


void __fastcall TMainForm::N21Click(TObject *Sender)
{
	wst->MyShutDownSystem(EWX_SHUTDOWN);
}
//---------------------------------------------------------------------------


void __fastcall TMainForm::About1Click(TObject *Sender)
{
	AboutBox->Show();
}
//---------------------------------------------------------------------------


void __fastcall TMainForm::TrayIcon1DblClick(TObject *Sender)
{
	bool bCtrlKeyDown  = GetAsyncKeyState( VK_CONTROL )>>((sizeof(SHORT)*8)-1);
	if(bCtrlKeyDown)
	{
		ShowWindow(Application->Handle, SW_SHOW);
		Application->NormalizeAllTopMosts();
		Application->Restore();
	}
}
//---------------------------------------------------------------------------


void __fastcall TMainForm::OnAppMinimize(TObject *Sender)
{
	ShowWindow(Application->Handle, SW_HIDE);
	Application->Minimize();
}

void __fastcall TMainForm::LogMemoAdd(String str)
{
	String time = FormatDateTime("[yyyy/mm/dd hh:nn:ss]", Now());
	Memo1->Lines->Add(time + " " + str);
	if(Memo1->Lines->Count > 4000) Memo1->Clear();
}



void __fastcall TMainForm::FormCreate(TObject *Sender)
{
	LogMemoAdd("FormCreate()");
	sit = new SystemInfoThread(&si);
	wst = new WebServerThread(&si);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "MainUnit.h"
#include "WebServerThread.h"
#include "About.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TMainForm *MainForm;
WebServerThread *wst;
//---------------------------------------------------------------------------
__fastcall TMainForm::TMainForm(TComponent* Owner)
	: TForm(Owner)
{
	wst = new WebServerThread();
	Application->ShowMainForm = False;
	this->Visible = True;
	Application->Minimize();
	ShowWindow(Application->Handle, SW_HIDE);
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


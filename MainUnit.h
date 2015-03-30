//---------------------------------------------------------------------------

#ifndef MainUnitH
#define MainUnitH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.Menus.hpp>
#include <Vcl.Buttons.hpp>

typedef struct
{
	int pid;
	String name;
	String path;
	String param;
	bool use;

} TAppInfo;

typedef struct
{
	int TotalCount;
	int UseCount;
	AnsiString Version;
	int ProcCheckTime;
	int UpdateCheckTime;
	TAppInfo List[16];
} TAppList;

typedef struct
{
	ULONGLONG Use;
	ULONGLONG Total;
} DiskInfo;

typedef struct
{
	float UseCpu;
	DiskInfo Mem;
	DiskInfo Hdd;
} SystemInfo;


//---------------------------------------------------------------------------
class TMainForm : public TForm
{
__published:	// IDE-managed Components
	TTrayIcon *TrayIcon1;
	TPopupMenu *PopupMenu1;
	TMenuItem *N11;
	TMenuItem *N21;
	TMenuItem *N41;
	TMenuItem *N1;
	TMenuItem *About1;
	TMemo *Memo1;
	TEdit *Edit1;
	TSpeedButton *SpeedButton1;
	TPanel *Panel1;
	void __fastcall N41Click(TObject *Sender);
	void __fastcall N11Click(TObject *Sender);
	void __fastcall N21Click(TObject *Sender);
	void __fastcall About1Click(TObject *Sender);
	void __fastcall TrayIcon1DblClick(TObject *Sender);
	void __fastcall OnAppMinimize(TObject *Sender);
	void __fastcall FormCreate(TObject *Sender);
private:	// User declarations
  void __fastcall WMDiaplayChange(TMessage& Msg);
public:		// User declarations
	void __fastcall LogMemoAdd(String str);
	__fastcall TMainForm(TComponent* Owner);
	SystemInfo si;
BEGIN_MESSAGE_MAP
  MESSAGE_HANDLER(WM_DISPLAYCHANGE, TMessage, WMDiaplayChange)
END_MESSAGE_MAP(TForm)
};
//---------------------------------------------------------------------------
extern PACKAGE TMainForm *MainForm;
//---------------------------------------------------------------------------
#endif

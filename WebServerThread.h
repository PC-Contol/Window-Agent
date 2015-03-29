//---------------------------------------------------------------------------

#ifndef WebServerThreadH
#define WebServerThreadH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <IdHTTPServer.hpp>
#include <IdCustomHTTPServer.hpp>
#include <MultiMon.h>
//---------------------------------------------------------------------------


class WebServerThread : public TThread
{
private:
protected:
	void __fastcall Execute();
public:
	TIdHTTPServer *IdHTTPServer1;
	void __fastcall IdHTTPServer1CommandGet(TIdContext *AContext, TIdHTTPRequestInfo *ARequestInfo, TIdHTTPResponseInfo *AResponseInfo);
	void __fastcall ProcCMD(String cmd, TIdHTTPResponseInfo *AResponseInfo);
    void __fastcall MyShutDownSystem(int iFlag);
	__fastcall WebServerThread();
};

//---------------------------------------------------------------------------
#endif

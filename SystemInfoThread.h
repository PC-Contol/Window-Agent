//---------------------------------------------------------------------------

#ifndef SystemInfoThreadH
#define SystemInfoThreadH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
//---------------------------------------------------------------------------
class SystemInfoThread : public TThread
{
private:
protected:
	void __fastcall Execute();
public:
	__fastcall SystemInfoThread(SystemInfo *pSi);
};
void GetSystemInfo();
void getCPU(void);
void GetHddInfo();
void GetMemInfo();
//---------------------------------------------------------------------------
#endif

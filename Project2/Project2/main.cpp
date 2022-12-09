#include "systemclass.h"

int WINAPI WinMain(HINSTANCE hInstance, 
	HINSTANCE hPrevInstance, 
	PSTR pScmdline, 
	int iCmdshow)
{
	SystemClass* System;
	bool result;

	//시스템 오브젝트 생성
	System = new SystemClass;
	if (!System)
	{
		return 0;
	}

	//시스템 오브젝트 초기화 및 실행
	result = System->Initialize();
	if (result)
	{
		System->Run();
	}

	//시스템 오브젝트 해제 및 제거
	System -> Shutdown();
	delete System;
	System = 0;

	return 0;
}


#include "systemclass.h"

int WINAPI WinMain(_In_ HINSTANCE hInstance, 
	_In_opt_ HINSTANCE hPrevInstance, 
	_In_ PSTR pScmdline, 
	_In_ int iCmdshow)
	//_In_ 읽기 전용으로 처리
	//_In_opt_ 매개변수는 선택사항
	//_Inout_ 수정 가능
	//_Inout_opt_
	//_Out_ 쓰기 가능
	//_Outptr_ 호출자에 대한 출력과 같음
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
	System = nullptr;

	return 0;
}


//DirectX11_Practice.cpp:응용 프로그램의 진입점을 정의

#include "stdafx.h"
#include "SystemClass.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR lpCmdLine,
	_In_ int nCmdShow)
{
	//System 오브젝트 생성
	SystemClass* System = new SystemClass;
	if (!System)
	{
		return -1;
	}

	//System 오브젝트 초기화 및 실행
	if (System->Initialize())
	{
		System->Run();
	}

	//System 오브젝트 종료 및 메모리 반환
	System->Shutdown();
	delete System;
	System = nullptr;

	return 0;
}
//DirectX11_Practice.cpp:���� ���α׷��� �������� ����

#include "stdafx.h"
#include "SystemClass.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR lpCmdLine,
	_In_ int nCmdShow)
{
	//System ������Ʈ ����
	SystemClass* System = new SystemClass;
	if (!System)
	{
		return -1;
	}

	//System ������Ʈ �ʱ�ȭ �� ����
	if (System->Initialize())
	{
		System->Run();
	}

	//System ������Ʈ ���� �� �޸� ��ȯ
	System->Shutdown();
	delete System;
	System = nullptr;

	return 0;
}
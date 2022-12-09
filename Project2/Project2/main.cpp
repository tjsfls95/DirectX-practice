#include "systemclass.h"

int WINAPI WinMain(HINSTANCE hInstance, 
	HINSTANCE hPrevInstance, 
	PSTR pScmdline, 
	int iCmdshow)
{
	SystemClass* System;
	bool result;

	//�ý��� ������Ʈ ����
	System = new SystemClass;
	if (!System)
	{
		return 0;
	}

	//�ý��� ������Ʈ �ʱ�ȭ �� ����
	result = System->Initialize();
	if (result)
	{
		System->Run();
	}

	//�ý��� ������Ʈ ���� �� ����
	System -> Shutdown();
	delete System;
	System = 0;

	return 0;
}


#include "systemclass.h"

int WINAPI WinMain(_In_ HINSTANCE hInstance, 
	_In_opt_ HINSTANCE hPrevInstance, 
	_In_ PSTR pScmdline, 
	_In_ int iCmdshow)
	//_In_ �б� �������� ó��
	//_In_opt_ �Ű������� ���û���
	//_Inout_ ���� ����
	//_Inout_opt_
	//_Out_ ���� ����
	//_Outptr_ ȣ���ڿ� ���� ��°� ����
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
	System = nullptr;

	return 0;
}


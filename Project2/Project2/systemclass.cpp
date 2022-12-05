#include "systemclass.h"

SystemClass::SystemClass()
{
	m_Input = 0;
	m_Graphics = 0;
}

SystemClass::SystemClass(const SystemClass& other)
{

}

SystemClass::~SystemClass()
{

}

bool SystemClass::Initialize()
{
	//��ũ�� ũ�� ����
	int screenWidth, screenHeight;
	//������ ����
	bool result;

	//�Լ��� ���� ���� �ޱ� ���� ���� 0���� �ʱ�ȭ
	screenWidth = 0;
	screenHeight = 0;

	//windows api ����(int&,int&)��
	InitializeWindows(screenWidth, screenHeight);

	//Ű �Է¹��� �Է� ������Ʈ ����
	m_Input = new InputClass;
	if (!m_Input)
	{
		return false;
	}

	//�Է� ������Ʈ Ȱ��ȭ
	m_Input->Initialize();

	//�׷��� ������Ʈ ����(�� ���α׷��� ��� �׷��� �������� �ٷ��)
	m_Graphics = new GraphicsClass;
	if (!m_Graphics)
	{
		return false;
	}

	//Initialize the graphics object
	result = m_Graphics->Initialize(screenWidth, screenHeight, m_hwnd);
	if (!result)
	{
		return false;
	}

	return true;
}

void SystemClass::Shutdown()
{
	//�׷��� ������Ʈ�� ����
	if (m_Graphics)
	{
		m_Graphics->Shutdown();
		delete m_Graphics;
		m_Graphics = 0;
	}

	//�Է� ������Ʈ�� ����
	if (m_Input)
	{
		delete m_Input;
		m_Input = 0;
	}

	//���α׷� â�� ����
	ShutdownWindows();

	return;
}

void SystemClass::Run()
{
	MSG msg;
	bool done, result;

	//�޼��� ������ Ȱ��ȭ�Ѵ�
	ZeroMemory(&msg, sizeof(MSG));
	//ZeroMemory�� �޸� ������ 0x00���� ä��� ��ũ��

	//�����쳪 ����ڰ� �޼����� �����Ҷ����� �ݺ��Ѵ�
	done = false;
	while (!done)
	{
		//������ �޼����� ����
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		//PeekMessage -> �޼��� ���� ���ο� ������� ��ȯ
		//�޼����� ������ true,������ false
		//false��� ������ ��� ����
		//true��� WM_QUIT���� �˻�,�´ٸ� true ��ȯ �� ���α׷� ����
		//WM_QUIT�� �ƴ϶�� �޼����� ó���ϰ� �޼��� ť���� ����
		//false�� �ٽ� �ȴٸ� InvalidateRect�� updateWindow ȣ��
		
		//TranslateMessage�� WM_QUIT�� �ƴ϶�� ���� Ű �Է��� ���� �Է�(WM_CHAR)�� �ٲ۴�
		//DispatchMEssage�� �޼����� ������ ���ν����� ���� MSG ����ü�� hwnd ����� ���� ������ �޼��� ó�� �Լ��� �����Ѵ�
	
		//���� ���α׷� ���� ��ȣ�� ������ ������ �����ϰ� Ż���Ѵ�
		if (msg.message == WM_QUIT)
		{
			done = true;
		}
		else
		{
			//�ƴ϶�� frame processing�Ѵ�
			result = Frame();
			if (!result)
			{
				done = true;
			}
		}
	}

	return;
}

bool SystemClass::Frame()
{
	bool result;

	//����ڰ� ��ư�� ���ų� ���α׷��� �����ϱ� ���ϴ��� �˻�
	if (m_Input->IsKeyDown(VK_ESCAPE))
	{
		return false;
	}

	//�׷��� ������Ʈ ������ processing
	result = m_Graphics->Frame();
	if (!result)
	{
		return false;
	}

	return true;
}

LRESULT CALLBACK SystemClass::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	switch (umsg)
	{
		//Ű���忡 Ű�� �������� �˻�
	case WM_KEYDOWN:
	{
		//���¸� ����� �� �ְ� Ű�� �������� �Է� ������Ʈ�� �����Ѵ�
		m_Input->KeyDown((unsigned int)wparam);
		return 0;
	}
	//Ű�� �������� �˻�
	case WM_KEYUP:
	{
		//���¸� ����� �� �ְ� Ű�� �������� �Է¿�����Ʈ�� ����
		m_Input->KeyUp((unsigned int)wparam);
		return 0;
	}
	//������� �ʱ⸦ ���ϴ� �޼����� �⺻ �޼��� �ڵ鷯�� ����
	default:
	{
		return DefWindowProc(hwnd, umsg, wparam, lparam);
	}
	}
}

void SystemClass::InitializeWindows(int& screenWidth, int& screenHeight)
{
	WNDCLASSEX wc;
	//WNDCLASSEX ->������ â Ŭ����
	DEVMODE dmScreenSettings;
	//DEVMODE ->������ �Ǵ� ���÷��� ����̽��� �ʱ�ȭ �� ȯ�濡 ���� ������ ����
	int posX, posY;

	//�� ������Ʈ�� ���� �ܺ� �����͸� ����
	ApplicationHandle = this;

	//�� ���α׷��� ���� �ν��Ͻ��� ����
	m_hinstance = GetModuleHandle(NULL);
	//��������(exe,dll ��)�� ��Ⱚ�� �������µ� ����ϴ� �Լ�
	//�Ķ���͸� NULL�� ä��� �ڽ��� ��Ⱚ�� ������

	//���� ���α׷� �̸��� ����
	m_applicationName = L"Engine"; //�����ڵ��� Ÿ�� �����ε�

	//�⺻ ���õ� ������ Ŭ���� ����
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	//CS_HREDRAW -> horizontal ������ �������� �̵��̳� ũ�� ���� �� horizontal �������� �ٽñ׸�
	//CS_VREDRAW -> vertical ������ �������� �̵��̳� ũ�� ���� �� vertical �������� �ٽñ׸�
	//CS_OWNDC -> Ŭ���� ������ �� ��������� ������ DC�� �Ҵ�
	wc.lpfnWndProc = WndProc;
	//�޼��� ���ν����� �̸��� �Է¹޴� �������� ����
	wc.cbClsExtra = 0;
	//Ŭ���� ���� �޸�
	wc.cbWndExtra = 0;
	//������ ���� �޸�
	wc.hInstance = m_hinstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	//�⺻ ������ ����
	wc.hIconSm = wc.hIcon;
	//���� ������ ����
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	//Ŀ�� ����
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	//����
	wc.lpszMenuName = NULL;
	//�޴� �̸�
	wc.lpszClassName = m_applicationName;
	//������ Ŭ���� �̸��� ���ڿ� Ÿ������ ����.
	wc.cbSize = sizeof(WNDCLASSEX);
	//WNDCLASSEX�� ũ��

	//������ Ŭ���� ���
	RegisterClassEx(&wc);
	//RegisterClassEx->�����ϸ� ��ϵ� ������ Ŭ������ ��Ÿ���� ���� ����,���� �߻��� 0 ����

	//Ŭ���̾�Ʈ ����ũž ȭ���� �ػ󵵸� ����
	screenWidth = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);
	//GetSystemMetrics �Լ��� ��ǻ�Ϳ��� �ϵ��� ���� �޶����ų� ����ڿ� ���ؼ� �缳�� ������
	//�������� ���α׷����� �� ����ϰ� ���� �� ����Ѵ�
	//SM_CXSCREEN->���� ȭ�� �ػ��� X�� ũ��
	//SM_CYSCREEN->���� ȭ�� �ػ��� Y�� ũ��

	//Ǯ��ũ������ ������ ��������� ���� ȭ�� ���� ��ġ
	if (FULL_SCREEN)
	{
		//���� ����ũž ȭ�� ũ�Ⱑ �ִ�� 32bit���
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (unsigned long)screenWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		//Ǯ��ũ������ ȭ�� ������ �ٲ۴�
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		//�»�ܿ� ������ ��ġ�� �����Ѵ�
		posX = posY = 0;
	}
	else
	{
		//800x640 �ػ󵵷� ���� ���
		screenWidth = 800;
		screenHeight = 600;

		//������ â�� ȭ�� �߾ӿ� ���´�
		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	//ȭ�� ������ �Բ� ������ â �����ϰ� �����Ѵ�
	m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName,
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP, posX, posY, screenWidth, screenHeight, 
		NULL, NULL, m_hinstance, NULL);

	//������ â�� ȭ�鿡 ���� main focus�� �д�
	ShowWindow(m_hwnd, SW_SHOW);
	SetForegroundWindow(m_hwnd);
	SetFocus(m_hwnd);

	//���콺 Ŀ���� ����
	ShowCursor(false);

	return;
}
//System Ŭ���� �� �Լ� ���� �κ�

#include "stdafx.h"
#include "InputClass.h"
#include "GraphicsClass.h"
#include "SystemClass.h"

SystemClass::SystemClass()
{

}

SystemClass::SystemClass(const SystemClass& other)
{

}

SystemClass::~SystemClass()
{

}

bool SystemClass::Initialize()
{
	//������ â ����,���� ���� ���� �ʱ�ȭ
	int screenWidth = 0;
	int screenHeight = 0;

	//������ ���� �ʱ�ȭ
	InitializeWindows(screenWidth, screenHeight);

	//m_Input ������Ʈ ����, ������� Ű���� �Է� ó���� ���
	m_Input = new InputClass;
	if (!m_Input)
	{
		return false;
	}

	//m_Input ������Ʈ �ʱ�ȭ
	m_Input->Initialize();

	//m_Graphics ������Ʈ ����, �׷��� �������� ó���ϱ� ���� ������Ʈ
	m_Graphics = new GraphicsClass;
	if (!m_Graphics)
	{
		return false;
	}

	//m_Graphics ������Ʈ �ʱ�ȭ
	return m_Graphics->Initialize(screenWidth, screenHeight, m_hwnd);
}

void SystemClass::Shutdown()
{
	//m_Graphics ������Ʈ ��ȯ
	if (m_Graphics)
	{
		m_Graphics->Shutdown();
		delete m_Graphics;
		m_Graphics = 0;
	}

	//m_Input ������Ʈ ��ȯ
	if (m_Input)
	{
		delete m_Input;
		m_Input = 0;
	}

	//Windows ���� ó��
	ShutdownWindows();
}

void SystemClass::Run()
{
	//�޼��� ����ü ���� �� �ʱ�ȭ
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	//����ڷκ��� ���� �޼����� ���������� �޼��������� ��
	while (true)
	{
		//������ �޼��� ó��
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			//���� �޼����� ���� ��� �޼��� ������ Ż��
			if (msg.message == WM_QUIT)
				break;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		else
		{
			//�޼����� ���� ���� ��� Frame �Լ��� ó��
			if (!Frame())
				break;
		}
	}
	
}

bool SystemClass::Frame()
{
	//ESC Ű ���� �� ���� ���θ� ó��
	if (m_Input->IsKeyDown(VK_ESCAPE))
	{
		return false;
	}

	//�׷��� ������Ʈ�� Frame�� ó��
	return m_Graphics->Frame();
}

LRESULT CALLBACK SystemClass::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	switch (umsg)
	{
		//Ű���尡 �������°� ó��
	case WM_KEYDOWN:
	{
		//Ű ���� flag�� m_Input ������Ʈ�� ó���ϵ��� �Ѵ�
		m_Input->KeyDown((unsigned int)wparam);
		return 0;
	}

	//Ű���尡 �������°� ó��
	case WM_KEYUP:
	{
		//Ű ���� flag�� m_Input ������Ʈ�� ó���ϵ��� �Ѵ�
		m_Input->KeyUp((unsigned int)wparam);
		return 0;
	}

	//�� ���� ��� �޼������� �⺻ �޼��� ó���� �ѱ��
	default:
	{
		return DefWindowProc(hwnd, umsg, wparam, lparam);
		//DefWindowProc->�޼����� ó���� ����� �����ϸ� ������� �޼����� ���� �ٸ���
		//������ ���ν����� �� �Լ��� ������ ���� �ٽ� ����
	}
	}
}

void SystemClass::InitializeWindows(int& screenWidth, int& screenHeight)
{
	//�ܺ� �����͸� �� ������Ʈ�� �����Ѵ�
	ApplicationHandle = this;

	//�� ���α׷��� �ν��Ͻ��� �����´�
	m_hInstance = GetModuleHandle(NULL);

	//���α׷� �̸��� �����Ѵ�
	m_applicationName = L"DirectX11_Practice";

	//windows Ŭ������ �Ʒ��� ���� ����
	WNDCLASSEX wc;
	//WNDCLASSEX ������ Ȯ���� ����ü
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	//CS_HREDRAW -> ���ι��� ũ�Ⱑ ���ϸ� ȭ���� �ٽñ׸�
	//CS_VREDRAW -> ���ι��� ũ�Ⱑ ���ϸ� ȭ���� �ٽ� �׸�
	//CS_OWNDC -> Ŭ���� ������ �� ��������� ���� DC(Device Context)�� �Ҵ�
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = m_applicationName;
	wc.cbSize = sizeof(WNDCLASSEX);

	//windows class�� ���
	RegisterClassEx(&wc);

	//����� ȭ���� �ػ󵵸� �о�´�
	screenWidth = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	int posX = 0;
	int posY = 0;

	//FULL_SCREEN ���� ���� ���� ȭ���� �����Ѵ�
	if (FULL_SCREEN)
	{
		//Ǯ ��ũ�� ���� �����ߴٸ� ����� ȭ�� �ػ󵵸� ����ũ�� �ػ󵵷� �����ϰ� ������ 32bit�� ����
		DEVMODE dmScreenSettings;
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (unsigned long)screenWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		//Ǯ ��ũ������ ���÷��� ������ �����Ѵ�
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);
	}
	else
	{
		//������ ����� ��� 1920 * 1080 ũ�⸦ ����
		screenWidth = 800;
		screenHeight = 600;

		//������ â�� ����,������ �� ��� ������ �Ѵ�
		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	//�����츦 �����ϰ� �ڵ��� ���Ѵ�
	m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName,
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
		posX, posY, screenWidth, screenHeight, NULL, NULL, m_hInstance, NULL);

	//�����츦 ȭ�鿡 ǥ���ϰ� ��Ŀ�� ����
	ShowWindow(m_hwnd, SW_SHOW);
	SetForegroundWindow(m_hwnd);
	SetFocus(m_hwnd);
}

void SystemClass::ShutdownWindows()
{
	//Ǯ��ũ�� ��忴�ٸ� ���÷��� ������ �ʱ�ȭ
	if (FULL_SCREEN)
	{
		ChangeDisplaySettings(NULL, 0);
	}

	//â�� �����Ѵ�
	DestroyWindow(m_hwnd);
	m_hwnd = NULL;

	//���α׷� �ν��Ͻ��� ����
	UnregisterClass(m_applicationName, m_hInstance);
	m_hInstance = NULL;

	//�ܺ������� ������ �ʱ�ȭ
	ApplicationHandle = NULL;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch (umessage)
	{
		//������ ���Ḧ Ȯ��
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	}
	//�����찡 �������� Ȯ��
	case WM_CLOSE:
	{
		PostQuitMessage(0);
		return 0;
	}
	//�� ���� ��� �޼������� �ý��� Ŭ������ �޼��� ó���� �ѱ��
	default:
	{
		return ApplicationHandle->MessageHandler(hwnd, umessage, wparam, lparam);
	}
	}
}
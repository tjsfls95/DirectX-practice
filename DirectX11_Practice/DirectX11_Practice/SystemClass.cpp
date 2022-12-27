//System 클래스 및 함수 정의 부분

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
	//윈도우 창 가로,세로 넓이 변수 초기화
	int screenWidth = 0;
	int screenHeight = 0;

	//윈도우 생성 초기화
	InitializeWindows(screenWidth, screenHeight);

	//m_Input 오브젝트 생성, 사용자의 키보드 입력 처리에 사용
	m_Input = new InputClass;
	if (!m_Input)
	{
		return false;
	}

	//m_Input 오브젝트 초기화
	m_Input->Initialize();

	//m_Graphics 오브젝트 생성, 그래픽 렌더링을 처리하기 위한 오브젝트
	m_Graphics = new GraphicsClass;
	if (!m_Graphics)
	{
		return false;
	}

	//m_Graphics 오브젝트 초기화
	return m_Graphics->Initialize(screenWidth, screenHeight, m_hwnd);
}

void SystemClass::Shutdown()
{
	//m_Graphics 오브젝트 반환
	if (m_Graphics)
	{
		m_Graphics->Shutdown();
		delete m_Graphics;
		m_Graphics = 0;
	}

	//m_Input 오브젝트 반환
	if (m_Input)
	{
		delete m_Input;
		m_Input = 0;
	}

	//Windows 종료 처리
	ShutdownWindows();
}

void SystemClass::Run()
{
	//메세지 구조체 생성 및 초기화
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	//사용자로부터 종료 메세지를 받을때까지 메세지루프를 돎
	while (true)
	{
		//윈도우 메세지 처리
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			//종료 메세지를 받을 경우 메세지 루프를 탈출
			if (msg.message == WM_QUIT)
				break;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		else
		{
			//메세지를 받지 않을 경우 Frame 함수를 처리
			if (!Frame())
				break;
		}
	}
	
}

bool SystemClass::Frame()
{
	//ESC 키 감지 및 종료 여부를 처리
	if (m_Input->IsKeyDown(VK_ESCAPE))
	{
		return false;
	}

	//그래픽 오브젝트의 Frame을 처리
	return m_Graphics->Frame();
}

LRESULT CALLBACK SystemClass::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	switch (umsg)
	{
		//키보드가 눌러졌는가 처리
	case WM_KEYDOWN:
	{
		//키 눌림 flag를 m_Input 오브젝트에 처리하도록 한다
		m_Input->KeyDown((unsigned int)wparam);
		return 0;
	}

	//키보드가 떨어졌는가 처리
	case WM_KEYUP:
	{
		//키 해제 flag를 m_Input 오브젝트에 처리하도록 한다
		m_Input->KeyUp((unsigned int)wparam);
		return 0;
	}

	//그 외의 모든 메세지들은 기본 메세지 처리로 넘긴다
	default:
	{
		return DefWindowProc(hwnd, umsg, wparam, lparam);
		//DefWindowProc->메세지를 처리한 결과를 리턴하며 결과값은 메세지에 따라 다르다
		//윈도우 프로시저는 이 함수가 리턴한 값을 다시 리턴
	}
	}
}

void SystemClass::InitializeWindows(int& screenWidth, int& screenHeight)
{
	//외부 포인터를 이 오브젝트로 지정한다
	ApplicationHandle = this;

	//이 프로그램의 인스턴스를 가져온다
	m_hInstance = GetModuleHandle(NULL);

	//프로그램 이름을 지정한다
	m_applicationName = L"DirectX11_Practice";

	//windows 클래스를 아래와 같이 설정
	WNDCLASSEX wc;
	//WNDCLASSEX 윈도우 확장형 구조체
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	//CS_HREDRAW -> 가로방향 크기가 변하면 화면을 다시그림
	//CS_VREDRAW -> 세로방향 크기가 변하면 화면을 다시 그림
	//CS_OWNDC -> 클래스 내부의 각 윈도우들의 고유 DC(Device Context)가 할당
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

	//windows class를 등록
	RegisterClassEx(&wc);

	//모니터 화면의 해상도를 읽어온다
	screenWidth = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	int posX = 0;
	int posY = 0;

	//FULL_SCREEN 변수 값에 따라 화면을 설정한다
	if (FULL_SCREEN)
	{
		//풀 스크린 모드로 지정했다면 모니터 화면 해상도를 데스크톱 해상도로 지정하고 색상을 32bit로 지정
		DEVMODE dmScreenSettings;
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (unsigned long)screenWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		//풀 스크린으로 디스플레이 설정을 변경한다
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);
	}
	else
	{
		//윈도우 모드의 경우 1920 * 1080 크기를 지정
		screenWidth = 800;
		screenHeight = 600;

		//윈도우 창을 가로,세로의 정 가운데 오도록 한다
		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	//윈도우를 생성하고 핸들을 구한다
	m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName,
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
		posX, posY, screenWidth, screenHeight, NULL, NULL, m_hInstance, NULL);

	//윈도우를 화면에 표시하고 포커스 지정
	ShowWindow(m_hwnd, SW_SHOW);
	SetForegroundWindow(m_hwnd);
	SetFocus(m_hwnd);
}

void SystemClass::ShutdownWindows()
{
	//풀스크린 모드였다면 디스플레이 설정을 초기화
	if (FULL_SCREEN)
	{
		ChangeDisplaySettings(NULL, 0);
	}

	//창을 제거한다
	DestroyWindow(m_hwnd);
	m_hwnd = NULL;

	//프로그램 인스턴스를 제거
	UnregisterClass(m_applicationName, m_hInstance);
	m_hInstance = NULL;

	//외부포인터 참조를 초기화
	ApplicationHandle = NULL;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch (umessage)
	{
		//윈도우 종료를 확인
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	}
	//윈도우가 닫히는지 확인
	case WM_CLOSE:
	{
		PostQuitMessage(0);
		return 0;
	}
	//그 외의 모든 메세지들은 시스템 클래스의 메세지 처리로 넘긴다
	default:
	{
		return ApplicationHandle->MessageHandler(hwnd, umessage, wparam, lparam);
	}
	}
}
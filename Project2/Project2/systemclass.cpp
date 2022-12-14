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
	//스크린 크기 설정
	int screenWidth, screenHeight;
	//실행결과 변수
	bool result;

	//함수에 변수 전달 받기 전에 값을 0으로 초기화
	screenWidth = 0;
	screenHeight = 0;

	//windows api 실행(int&,int&)형
	InitializeWindows(screenWidth, screenHeight);

	//키 입력받을 입력 오브젝트 생성
	m_Input = new InputClass;
	if (!m_Input)
	{
		return false;
	}

	//입력 오브젝트 활성화
	m_Input->Initialize();

	//그래픽 오브젝트 생성(이 프로그램의 모든 그래픽 렌더링을 다룬다)
	m_Graphics = new GraphicsClass;
	if (!m_Graphics)
	{
		return false;
	}

	//그래픽 오브젝트 초기화
	result = m_Graphics->Initialize(screenWidth, screenHeight, m_hwnd);
	if (!result)
	{
		return false;
	}

	return true;
}

void SystemClass::Shutdown()
{
	//그래픽 오브젝트를 해제
	if (m_Graphics)
	{
		m_Graphics->Shutdown();
		delete m_Graphics;
		m_Graphics = 0;
	}

	//입력 오브젝트를 해제
	if (m_Input)
	{
		delete m_Input;
		m_Input = 0;
	}

	//프로그램 창을 끈다
	ShutdownWindows();
}

void SystemClass::Run()
{
	MSG msg;
	bool done, result;

	//메세지 구조를 활성화한다
	ZeroMemory(&msg, sizeof(MSG));
	//ZeroMemory는 메모리 영역을 0x00으로 채우는 매크로

	//윈도우나 사용자가 메세지를 종료할때까지 반복한다
	done = false;
	while (!done)
	{
		//윈도우 메세지를 관리
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		//PeekMessage -> 메세지 존재 여부와 관계없이 반환
		//메세지가 있으면 true,없으면 false
		//false라면 루프를 계속 실행
		//true라면 WM_QUIT인지 검사,맞다면 true 반환 후 프로그램 종료
		//WM_QUIT이 아니라면 메세지를 처리하고 메세지 큐에서 제거
		//false가 다시 된다면 InvalidateRect와 updateWindow 호출
		
		//TranslateMessage는 WM_QUIT이 아니라면 가상 키 입력을 문자 입력(WM_CHAR)로 바꾼다
		//DispatchMEssage는 메세지를 윈도우 프로시저로 보내 MSG 구조체의 hwnd 멤버를 보고 윈도우 메세지 처리 함수로 전달한다
	
		//만약 프로그램 종료 신호가 나오면 루프를 종료하고 탈출한다
		if (msg.message == WM_QUIT)
		{
			done = true;
		}
		else
		{
			//아니라면 frame processing한다
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

	//사용자가 버튼을 떼거나 프로그램을 종료하길 원하는지 검사
	if (m_Input->IsKeyDown(VK_ESCAPE))
	{
		return false;
	}

	//그래픽 오브젝트 프레임 processing
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
		//키보드에 키를 눌렀는지 검사
	case WM_KEYDOWN:
	{
		//상태를 기록할 수 있게 키가 눌렸으면 입력 오브젝트에 전달한다
		m_Input->KeyDown((unsigned int)wparam);
		return 0;
	}
	//키가 떼졌는지 검사
	case WM_KEYUP:
	{
		//상태를 기록할 수 있게 키가 떼졌으면 입력오브젝트에 저장
		m_Input->KeyUp((unsigned int)wparam);
		return 0;
	}
	//사용하지 않기를 원하는 메세지를 기본 메세지 핸들러로 전달
	default:
	{
		return DefWindowProc(hwnd, umsg, wparam, lparam);
	}
	}
}

void SystemClass::InitializeWindows(int& screenWidth, int& screenHeight)
{
	WNDCLASSEX wc;
	//WNDCLASSEX ->윈도우 창 클래스
	DEVMODE dmScreenSettings;
	//DEVMODE ->프린터 또는 디스플레이 디바이스의 초기화 및 환경에 대한 정보가 포함
	int posX, posY;

	//이 오브젝트에 대한 외부 포인터를 얻음
	ApplicationHandle = this;

	//이 프로그램에 대한 인스턴스를 얻음
	m_hinstance = GetModuleHandle(NULL);
	//실행파일(exe,dll 등)의 모듈값을 가져오는데 사용하는 함수
	//파라미터를 NULL로 채우면 자신의 모듈값을 가져옴

	//현재 프로그램 이름을 저장
	m_applicationName = L"DirectX_Practice"; //유니코드형 타입 저장인듯

	//기본 세팅된 윈도우 클래스 저장
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	//CS_HREDRAW -> horizontal 방향의 윈도우의 이동이나 크기 조절 시 horizontal 방향으로 다시그림
	//CS_VREDRAW -> vertical 방향의 윈도우의 이동이나 크기 조절 시 vertical 방향으로 다시그림
	//CS_OWNDC -> 클래스 내부의 각 윈도우들의 고유의 DC가 할당
	wc.lpfnWndProc = WndProc;
	//메세지 프로시저의 이름을 입력받는 포인터형 문자
	wc.cbClsExtra = 0;
	//클래스 여분 메모리
	wc.cbWndExtra = 0;
	//윈도우 여분 메모리
	wc.hInstance = m_hinstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	//기본 아이콘 지정
	wc.hIconSm = wc.hIcon;
	//작은 아이콘 지정
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	//커서 지정
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	//배경색
	wc.lpszMenuName = NULL;
	//메뉴 이름
	wc.lpszClassName = m_applicationName;
	//윈도우 클래스 이름을 문자열 타입으로 저장.
	wc.cbSize = sizeof(WNDCLASSEX);
	//WNDCLASSEX의 크기

	//윈도우 클래스 등록
	RegisterClassEx(&wc);
	//RegisterClassEx->성공하면 등록된 윈도우 클래스를 나타내는 값을 리턴,에러 발생시 0 리턴

	//클라이언트 데스크탑 화면의 해상도를 결정
	screenWidth = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);
	//GetSystemMetrics 함수는 컴퓨터에서 하드웨어에 따라 달라지거나 사용자에 의해서 재설정 가능한
	//정보들을 프로그램에서 얻어서 사용하고 싶을 때 사용한다
	//SM_CXSCREEN->현재 화면 해상도의 X축 크기
	//SM_CYSCREEN->현재 화면 해상도의 Y축 크기

	//풀스크린인제 윈도우 모드인지에 따라 화면 설정 설치
	if (FULL_SCREEN)
	{
		//유저 데스크탑 화면 크기가 최대고 32bit라면
		//데스크탑 해상도로 지정, 색상은 32bit로
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (unsigned long)screenWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		//풀스크린으로 화면 설정을 바꾼다
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		//좌상단에 윈도우 위치를 설정한다
		posX = posY = 0;
	}
	else
	{
		//윈도우 모드로 800x600 해상도로 맞출 경우
		screenWidth = 800;
		screenHeight = 600;

		//윈도우 창을 화면 중앙에 놓는다
		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	//화면 설정과 함께 윈도우 창 생성하고 제어한다
	m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName,
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP, posX, posY, screenWidth, screenHeight, 
		NULL, NULL, m_hinstance, NULL);
	//WS_EX_APPWINDOW -> 창이 표시되면 최상위 창을 작업 표시줄에 강제로 표시
	//WS_CLIPSIBLINGS -> 자식창이 겹친다면 그리지 않는다
	//WS_CLIPCHILDREN -> 부모 창에서 그리기 수행하면 자식창은 그리지 않는다
	//WS_POPUP -> 창을 팝업창으로 설정

	//윈도우 창을 화면에 띄우고 main focus에 둔다
	ShowWindow(m_hwnd, SW_SHOW);
	SetForegroundWindow(m_hwnd);
	//SetFOregroundWindow -> 지정된 윈도우를 만든 스레드의 우선순위를
	//최상위로 가져와 창을 활성화
	SetFocus(m_hwnd);

	//마우스 커서를 숨김
	ShowCursor(false);
}

void SystemClass::ShutdownWindows()
{
	//마우스 커서를 보이게 함
	ShowCursor(true);

	//풀 스크린 모드라면 디스플레이 설정 복구
	if (FULL_SCREEN)
	{
		ChangeDisplaySettings(NULL, 0);
	}

	//창 제거
	DestroyWindow(m_hwnd);
	m_hwnd = NULL;

	//프로그램 인스턴스 제거
	UnregisterClass(m_applicationName, m_hinstance);
	m_hinstance = NULL;

	//이 클래스의 포인터를 해제
	ApplicationHandle = NULL;

}

LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch (umessage)
	{
		//창이 파괴되었는지 확인
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	}
	//창이 닫겨 있는 상태인지 확인
	case WM_CLOSE:
	{
		PostQuitMessage(0);
		return 0;
	}
	//다른 모든 메세지는 시스템 클래스의 메세지 핸들러에 전달
	default:
	{
		return ApplicationHandle->MessageHandler(hwnd, umessage, wparam, lparam);
	}
	}

	//WM_CLOSE -> 윈도우 종료전에 실행
	//WM_DESTROY -> 윈도우 종료될 때 실행
	//WM_QUIT -> 프로그램을 종료하라는 신호
}
#include "d3dclass.h"

D3DClass::D3DClass()
{
	m_swapChain = 0;
	m_device = 0;
	m_deviceContext = 0;
	m_renderTargetView = 0;
	m_depthStencillBuffer = 0;
	m_depthStencillState = 0;
	m_depthStencillView = 0;
	m_rasterState = 0;
}

D3DClass::D3DClass(const D3DClass& other)
{

}

D3DClass::~D3DClass()
{

}

bool D3DClass::Initialize(int screenWidth, int screenHeight, bool vsync,
	HWND hwnd, bool fullscreen, float screenDepth, float screenNear)
{
	HRESULT result;
	IDXGIFactory* factory;
	//DXGI를 사용하기 위해 필요한 각종 자원을 얻어오는 인터페이스
	IDXGIAdapter* adapter;
	//DXGI에서 그래픽/디스플레이 관련기능을 지원하는 인터페이스
	IDXGIOutput* adapterOutput;
	//디스플레이 출력을 제공하는 인터페이스
	unsigned int numModes, i, numerator, denominator, stringLength;
	DXGI_MODE_DESC* displayModeList;
	//디스플레이 모드를 설정하는 구조체
	DXGI_ADAPTER_DESC adapterDesc;
	//어댑터(그래픽/디스플레이 관련기능)을 설정하는 구조체
	int error;
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	//스왑체인을 설정하는 구조체
	D3D_FEATURE_LEVEL featureLevel;
	//Direct3D 특성레벨 속성값(하드웨어 지원 레벨)
	ID3D11Texture2D* backBufferPtr;
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	//2D텍스처 구조체
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	//스텐실 구조체
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	//스텐실 자원 뷰 구조체
	D3D11_RASTERIZER_DESC rasterDesc;
	//래스터라이즈 구조체
	D3D11_VIEWPORT viewport;
	//뷰포트 구조체
	float fieldOfView, screenAspect;

	//vsyng 설정 저장
	m_vsync_enabled = vsync;

	//DirectX 그래픽 인터페이스 factory를 만든다
	result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
	if (FAILED(result))
	{
		return false;
	}

	//그래픽 인터페이스(비디오카드)에 사용하는 어댑터를 만들기 위해 factory 생성
	result = factory->EnumAdapters(0, &adapter);
	if (FAILED(result))
	{
		return false;
	}

	//어댑터 출력(모니터)를 카운트한다
	result = adapter->EnumOutputs(0, &adapterOutput);
	if (FAILED(result))
	{
		return false;
	}

	//어댑터 출력(모니터)에 맞는 디스플레이 포맷인 DXGI_FORMAT_R8G8B8A8_UNORM에 맞는 모델넘버를 가져온다
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
	if (FAILED(result))
	{
		return false;
	}

	//모니터,비디오 카드에 맞는 가능한 모든 디스플레이 모드들을 담은 리스트를 생성
	displayModeList = new DXGI_MODE_DESC[numModes];
	if (!displayModeList)
	{
		return false;
	}

	//디스플레이 모드 리스트 구조체를 채운다
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
	if (FAILED(result))
	{
		return false;
	}

	//화면의 넓이와 높이에 맞는 디스플레이 모드를 찾기 위해 검색한다
	//모니터 재생률과 같은 저장된 값을 찾으면 저장한다
	for (i = 0; i < numModes; i++)
	{
		if (displayModeList[i].Width == (unsigned int)screenWidth)
		{
			if (displayModeList[i].Height == (unsigned int)screenHeight)
			{
				numerator = displayModeList[i].RefreshRate.Numerator;
				denominator = displayModeList[i].RefreshRate.Denominator;
			}
		}
	}

	//어댑터(비디오카드) 구조체를 가져온다
	result = adapter->GetDesc(&adapterDesc);
	if (FAILED(result))
	{
		return false;
	}

	//메가바이트 단위의 비디오카드 전용 메모리 저장
	m_videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

	//비디오 카드 이름을 문자 배열로 바꾼다음 저장
	error = wcstombs_s(&stringLength, m_videoCardDescription, 128, adapterDesc.Description, 128);
	if (error != 0)
	{
		return false;
	}

	//디스플레이 모드 리스트를 해제
	delete[] displayModeList;
	displayModeList = 0;

	//어댑터 출력(모니터)를 해제
	adapterOutput->Release();
	adapterOutput = 0;

	//어댑터를 해제
	adapter->Release();
	adapter = 0;

	//팩토리 해제
	factory->Release();
	factory = 0;

	//스왑 체인 구조체를 초기화
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

	//싱글 백 버퍼를 세팅
	swapChainDesc.BufferCount = 1;

	//백버퍼 높이와 넓이 세팅
	swapChainDesc.BufferDesc.Width = screenWidth;
	swapChainDesc.BufferDesc.Height = screenHeight;

	//백버퍼를 32비트로 세팅
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	//백버퍼의 재생률 세팅
	if (m_vsync_enabled)
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
	}
	else
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	}

	//백버퍼의 용도 세팅
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	//렌더링하려는 윈도우의 핸들값 세팅
	swapChainDesc.OutputWindow = hwnd;

	//멀티샘프링 off로 전환
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;

	//풀스크린 또는 윈도우 모드 세팅
	if (fullscreen)
	{
		swapChainDesc.Windowed = false;
	}
	else
	{
		swapChainDesc.Windowed = true;
	}

	//스캔 라인 순서와 크기를 지정하지 않음으로 설정
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	//출력 이후 백 버퍼 내용을 삭제
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

	//추가적인 플래그는 세팅하지 않음
	swapChainDesc.Flags = 0;

	//DirectX 레벨을 설정
	featureLevel = D3D_FEATURE_LEVEL_11_0;

	//스왑 체인과 Direct3D 디바이스, Direct3D 디바이스 context 생성
	result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &featureLevel, 1,
		D3D11_SDK_VERSION, &swapChainDesc, &m_swapChain, &m_device, NULL, &m_deviceContext);
	if (FAILED(result))
	{
		return false;
	}

	//백 버퍼 포인터를 얻는다
	result = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr);
	if (FAILED(result))
	{
		return false;
	}

	//백 버퍼 포인터로 렌더 타겟 뷰 생성
	result = m_device->CreateRenderTargetView(backBufferPtr, NULL, &m_renderTargetView);
	if (FAILED(result))
	{
		return false;
	}

	//더 이상 필요가 없는 백버퍼 포인터를 해제
	backBufferPtr->Release();
	backBufferPtr = 0;
}
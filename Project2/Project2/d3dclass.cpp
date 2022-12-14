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
	IDXGIFactory* factory = nullptr;
	//DXGI를 사용하기 위해 필요한 각종 자원을 얻어오는 인터페이스
	IDXGIAdapter* adapter = nullptr;
	//DXGI에서 그래픽/디스플레이 관련기능을 지원하는 인터페이스
	IDXGIOutput* adapterOutput = nullptr;
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
	//2D텍스처 구조체(깊이버퍼)
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
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

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

	//깊이 버퍼 구조체를 초기화
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	//깊이 버퍼 구조체 세팅
	depthBufferDesc.Width = screenWidth;
	depthBufferDesc.Height = screenHeight;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	//깊이버퍼 24bit 스텐실 8bit 구조
	depthBufferDesc.SampleDesc.Count = 1;
	//픽셀당 멀티 샘플 수
	depthBufferDesc.SampleDesc.Quality = 0;
	//품질 레벨
	//멀티샘플 해제
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	//텍스쳐(깊이버퍼)의 저장/불러오기 방식 설정
	//D3D11_USAGE_DEFAULT -> GPU로 저장/불러오기
	//D3D11_USAGE_IMMUTABLE -> GPU로 불러오기만
	//D3D11_USAGE_DYNAMIC -> GPU로 불러오기,CPU로 저장
	//D3D11_USAGE_STAGING -> GPU에서 CPU로 복사(이동)만 가능
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	//2D텍스쳐인 depthBufferDesc를 출력단계에서 depth-stencil 타겟으로 바인드
	depthBufferDesc.CPUAccessFlags = 0;
	//CPU 액세스를 허용하지 않음
	depthBufferDesc.MiscFlags = 0;
	//기타 플래그 사용안함

	//깊이 버퍼 구조체를 이용하여 깊이버퍼텍스처 생성
	result = m_device->CreateTexture2D(&depthBufferDesc, NULL, &m_depthStencillBuffer);
	if (FAILED(result))
	{
		return false;
	}

	//깊이 스텐실 상태 구조체 초기화
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	//깊이 스텐실 상태 구조체 세팅
	depthStencilDesc.DepthEnable = true;
	//깊이 테스트 활성화
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	//쓰기 활성화
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	//깊이 데이터 비교함수
	//D3D11_COMPARISON_LESS는 비교데이터보다 현재데이터가
	//더 z값이 적을 때, 즉 현재 데이터가 더 앞에 있을때만 갱신
	//다시말해 현재 정점이 더 앞이면 렌더링한다
	
	depthStencilDesc.StencilEnable = true;
	//깊이 스텐실 테스트 활성화
	depthStencilDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	depthStencilDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
	
	//픽셀이 정면방향일 때의 스텐실 설정
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	//깊이-스텐실 테스트 실패했을 때,스텐실 데이터 보존(D3D11_STENCIL_OP_KEEP)
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	//스텐실 테스트는 통과하고 깊이 테스트가 실패할때,스텐실 값을 1 올리고
	//필요하다면 결과를 wrap함(D3D11_STENCIL_OP_INCR)
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	//깊이-스텐실 테스트를 통과 했을 때,스텐실 데이터 보존
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	//스텐실 데이터 비교함수
	//항상 비교함

	//픽셀이 뒷면방향일 때의 스텐실 설정
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	//스텐실 테스트는 통과하고 깊이 테스트가 실패할때 스텐실 값을 1 내리고
	//필요하다면 결과를 wrap함(D3D11_STENCIL_OP_DECR)
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	//깊이 스텐실 상태 생성
	result = m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencillState);
	if(FAILED(result))
	{
		return false;
	}

	//깊이 스텐실 상태 세팅
	m_deviceContext->OMSetDepthStencilState(m_depthStencillState, 1);
	//OMSetDepthStencilState->스텐실 상태를 출력단계에서 세팅

	//깊이 스텐실 뷰 초기화
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	//깊이 스텐실 뷰 구조체 세팅
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	//깊이버퍼 24bit 스텐실 8bit 구조
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	//깊이-스텐실 뷰가 2D 텍스처에 액세스
	depthStencilViewDesc.Texture2D.MipSlice = 0;
	//사용되는 첫번쨰 mipmap 인덱스

	//깊이 스텐실 뷰 생성
	result = m_device->CreateDepthStencilView(m_depthStencillBuffer, 
		&depthStencilViewDesc, &m_depthStencillView);
	if (FAILED(result))
	{
		return false;
	}

	//렌더링 타겟 뷰와 깊이 스텐실 버퍼를 출력 렌더링 파이프라인에 바인딩
	m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencillView);

	//어떻게 그리고 어떤 폴리곤을 그릴지 결정하는 래스터 구조체를 세팅
	rasterDesc.AntialiasedLineEnable = false;
	//안티에일리어싱 하지 않음
	rasterDesc.CullMode = D3D11_CULL_BACK;
	//후면에 존재하는것은 그리지 않음
	//컬링:렌더링 할 필요가 없는 것을 선별해 제외(그리지 않음)
	rasterDesc.DepthBias = 0;
	//픽셀에 부여하는 깊이값
	rasterDesc.DepthBiasClamp = 0.0f;
	//
	rasterDesc.DepthClipEnable = true;
	//z축 깊이 클리핑 가능여부(클리핑->뷰포트(보이는 영역,윈도우)에만
	//렌더링 처리
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	//D3D11_FILL_WIREFRAME -> 선만 이음
	//D3D11_FILL_SOLID -> 속도 채움,표준
	rasterDesc.FrontCounterClockwise = false;
	//반시계 방향을 앞면으로 설정 -> 이유작성
	rasterDesc.MultisampleEnable = false;
	//멀티샘플링 하지 않음
	//true면 사변형 라인 안티 에일리어싱
	//false면 알파 라인 안티 에일리어싱 -> 더 자세하게
	rasterDesc.ScissorEnable = false;
	//Rectangle 영역에 벗어난것은 다 컬링하는 모드인듯
	rasterDesc.SlopeScaledDepthBias = 0.0f;
	//픽셀 기울기에 스칼라값을 주는듯
	
	//작성한 래스터 구조체로 래스터라이저 상태를 생성한다
	result = m_device->CreateRasterizerState(&rasterDesc, &m_rasterState);
	if (FAILED(result))
	{
		return false;
	}

	//래스터라이저 상태 세팅
	m_deviceContext->RSSetState(m_rasterState);

	//렌더링을 위해 뷰포트 설정
	viewport.Width = (float)screenWidth;
	viewport.Height = (float)screenHeight;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	//뷰포트 생성
	m_deviceContext->RSSetViewports(1, &viewport);

	//투영 행렬(projection) 설정
	fieldOfView = (float)XM_PIDIV4;
	screenAspect = (float)screenWidth / (float)screenHeight;

	//3D 렌더링을 하기 위한 projection(투영) 매트릭스 생성
	m_projectionMatrix = XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, screenNear, screenDepth);

	//world 행렬을 항등행렬(I-행렬)로 초기화
	m_worldMatrix = XMMatrixIdentity();

	//2D 렌더링을 위해 직교 투영 행렬을 생성
	m_orthoMatrix = XMMatrixOrthographicLH((float)screenWidth, (float)screenHeight, screenNear, screenDepth);

	return true;
}

void D3DClass::Shutdown()
{
	//프로그램 종료 전 윈도우 설정으로 해제, 그렇지 않으면 스왑 체인 시에 예외 발생
	if (m_swapChain)
	{
		m_swapChain->SetFullscreenState(false, NULL);
	}

	if (m_rasterState)
	{
		m_rasterState->Release();
		m_rasterState = 0;
	}

	if (m_depthStencillView)
	{
		m_depthStencillView->Release();
		m_depthStencillView = 0;
	}

	if (m_depthStencillState)
	{
		m_depthStencillState->Release();
		m_depthStencillState = 0;
	}

	if (m_depthStencillBuffer)
	{
		m_depthStencillBuffer->Release();
		m_depthStencillBuffer = 0;
	}

	if (m_renderTargetView)
	{
		m_renderTargetView->Release();
		m_renderTargetView = 0;
	}

	if (m_deviceContext)
	{
		m_deviceContext->Release();
		m_deviceContext = 0;
	}

	if (m_device)
	{
		m_device->Release();
		m_device = 0;
	}

	if (m_swapChain)
	{
		m_swapChain->Release();
		m_swapChain = 0;
	}

	return;
}

void D3DClass::BeginScene(float red, float green, float blue, float alpha)
{
	float color[4];

	//버퍼를 지울 색을 세팅
	color[0] = red;
	color[1] = green;
	color[2] = blue;
	color[3] = alpha;

	//백 버퍼를 지움
	m_deviceContext->ClearRenderTargetView(m_renderTargetView, color);

	//깊이 버퍼를 지움
	m_deviceContext->ClearDepthStencilView(m_depthStencillView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	return;
}

void D3DClass::EndScene()
{
	//렌더링이 완성되면 백버퍼를 출력
	if (m_vsync_enabled)
	{
		//화면 refresh(새로고침)비율 고정
		m_swapChain->Present(1, 0);
	}
	else
	{
		//최대한 빠르게 출력
		m_swapChain->Present(0, 0);
	}

	return;
}

ID3D11Device* D3DClass::GetDevice()
{
	return m_device;
}

ID3D11DeviceContext* D3DClass::GetDeviceContext()
{
	return m_deviceContext;
}

void D3DClass::GetProjectionMatrix(XMMATRIX& projectionMatrix)
{
	projectionMatrix = m_projectionMatrix;
	return;
}

void D3DClass::GetWorldMatrix(XMMATRIX& worldMatrix)
{
	worldMatrix = m_worldMatrix;
	return;
}

void D3DClass::GetOrthoMatrix(XMMATRIX& orthoMatrix)
{
	orthoMatrix = m_orthoMatrix;
	return;
}

void D3DClass::GetVideoCardInfo(char* cardName, int& memory)
{
	strcpy_s(cardName, 128, m_videoCardDescription);
	memory = m_videoCardMemory;
	return;
}

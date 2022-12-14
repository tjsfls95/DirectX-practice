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
	//DXGI�� ����ϱ� ���� �ʿ��� ���� �ڿ��� ������ �������̽�
	IDXGIAdapter* adapter = nullptr;
	//DXGI���� �׷���/���÷��� ���ñ���� �����ϴ� �������̽�
	IDXGIOutput* adapterOutput = nullptr;
	//���÷��� ����� �����ϴ� �������̽�
	unsigned int numModes, i, numerator, denominator, stringLength;
	DXGI_MODE_DESC* displayModeList;
	//���÷��� ��带 �����ϴ� ����ü
	DXGI_ADAPTER_DESC adapterDesc;
	//�����(�׷���/���÷��� ���ñ��)�� �����ϴ� ����ü
	int error;
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	//����ü���� �����ϴ� ����ü
	D3D_FEATURE_LEVEL featureLevel;
	//Direct3D Ư������ �Ӽ���(�ϵ���� ���� ����)
	ID3D11Texture2D* backBufferPtr;
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	//2D�ؽ�ó ����ü(���̹���)
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	//���ٽ� ����ü
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	//���ٽ� �ڿ� �� ����ü
	D3D11_RASTERIZER_DESC rasterDesc;
	//�����Ͷ����� ����ü
	D3D11_VIEWPORT viewport;
	//����Ʈ ����ü
	float fieldOfView, screenAspect;

	//vsyng ���� ����
	m_vsync_enabled = vsync;

	//DirectX �׷��� �������̽� factory�� �����
	result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
	if (FAILED(result))
	{
		return false;
	}

	//�׷��� �������̽�(����ī��)�� ����ϴ� ����͸� ����� ���� factory ����
	result = factory->EnumAdapters(0, &adapter);
	if (FAILED(result))
	{
		return false;
	}

	//����� ���(�����)�� ī��Ʈ�Ѵ�
	result = adapter->EnumOutputs(0, &adapterOutput);
	if (FAILED(result))
	{
		return false;
	}

	//����� ���(�����)�� �´� ���÷��� ������ DXGI_FORMAT_R8G8B8A8_UNORM�� �´� �𵨳ѹ��� �����´�
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
	if (FAILED(result))
	{
		return false;
	}

	//�����,���� ī�忡 �´� ������ ��� ���÷��� ������ ���� ����Ʈ�� ����
	displayModeList = new DXGI_MODE_DESC[numModes];
	if (!displayModeList)
	{
		return false;
	}

	//���÷��� ��� ����Ʈ ����ü�� ä���
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
	if (FAILED(result))
	{
		return false;
	}

	//ȭ���� ���̿� ���̿� �´� ���÷��� ��带 ã�� ���� �˻��Ѵ�
	//����� ������� ���� ����� ���� ã���� �����Ѵ�
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

	//�����(����ī��) ����ü�� �����´�
	result = adapter->GetDesc(&adapterDesc);
	if (FAILED(result))
	{
		return false;
	}

	//�ް�����Ʈ ������ ����ī�� ���� �޸� ����
	m_videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

	//���� ī�� �̸��� ���� �迭�� �ٲ۴��� ����
	error = wcstombs_s(&stringLength, m_videoCardDescription, 128, adapterDesc.Description, 128);
	if (error != 0)
	{
		return false;
	}

	//���÷��� ��� ����Ʈ�� ����
	delete[] displayModeList;
	displayModeList = 0;

	//����� ���(�����)�� ����
	adapterOutput->Release();
	adapterOutput = 0;

	//����͸� ����
	adapter->Release();
	adapter = 0;

	//���丮 ����
	factory->Release();
	factory = 0;

	//���� ü�� ����ü�� �ʱ�ȭ
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

	//�̱� �� ���۸� ����
	swapChainDesc.BufferCount = 1;

	//����� ���̿� ���� ����
	swapChainDesc.BufferDesc.Width = screenWidth;
	swapChainDesc.BufferDesc.Height = screenHeight;

	//����۸� 32��Ʈ�� ����
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	//������� ����� ����
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

	//������� �뵵 ����
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	//�������Ϸ��� �������� �ڵ鰪 ����
	swapChainDesc.OutputWindow = hwnd;

	//��Ƽ������ off�� ��ȯ
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;

	//Ǯ��ũ�� �Ǵ� ������ ��� ����
	if (fullscreen)
	{
		swapChainDesc.Windowed = false;
	}
	else
	{
		swapChainDesc.Windowed = true;
	}

	//��ĵ ���� ������ ũ�⸦ �������� �������� ����
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	//��� ���� �� ���� ������ ����
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	//�߰����� �÷��״� �������� ����
	swapChainDesc.Flags = 0;

	//DirectX ������ ����
	featureLevel = D3D_FEATURE_LEVEL_11_0;

	//���� ü�ΰ� Direct3D ����̽�, Direct3D ����̽� context ����
	result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &featureLevel, 1,
		D3D11_SDK_VERSION, &swapChainDesc, &m_swapChain, &m_device, NULL, &m_deviceContext);
	if (FAILED(result))
	{
		return false;
	}

	//�� ���� �����͸� ��´�
	result = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr);
	if (FAILED(result))
	{
		return false;
	}

	//�� ���� �����ͷ� ���� Ÿ�� �� ����
	result = m_device->CreateRenderTargetView(backBufferPtr, NULL, &m_renderTargetView);
	if (FAILED(result))
	{
		return false;
	}

	//�� �̻� �ʿ䰡 ���� ����� �����͸� ����
	backBufferPtr->Release();
	backBufferPtr = 0;

	//���� ���� ����ü�� �ʱ�ȭ
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	//���� ���� ����ü ����
	depthBufferDesc.Width = screenWidth;
	depthBufferDesc.Height = screenHeight;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	//���̹��� 24bit ���ٽ� 8bit ����
	depthBufferDesc.SampleDesc.Count = 1;
	//�ȼ��� ��Ƽ ���� ��
	depthBufferDesc.SampleDesc.Quality = 0;
	//ǰ�� ����
	//��Ƽ���� ����
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	//�ؽ���(���̹���)�� ����/�ҷ����� ��� ����
	//D3D11_USAGE_DEFAULT -> GPU�� ����/�ҷ�����
	//D3D11_USAGE_IMMUTABLE -> GPU�� �ҷ����⸸
	//D3D11_USAGE_DYNAMIC -> GPU�� �ҷ�����,CPU�� ����
	//D3D11_USAGE_STAGING -> GPU���� CPU�� ����(�̵�)�� ����
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	//2D�ؽ����� depthBufferDesc�� ��´ܰ迡�� depth-stencil Ÿ������ ���ε�
	depthBufferDesc.CPUAccessFlags = 0;
	//CPU �׼����� ������� ����
	depthBufferDesc.MiscFlags = 0;
	//��Ÿ �÷��� ������

	//���� ���� ����ü�� �̿��Ͽ� ���̹����ؽ�ó ����
	result = m_device->CreateTexture2D(&depthBufferDesc, NULL, &m_depthStencillBuffer);
	if (FAILED(result))
	{
		return false;
	}

	//���� ���ٽ� ���� ����ü �ʱ�ȭ
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	//���� ���ٽ� ���� ����ü ����
	depthStencilDesc.DepthEnable = true;
	//���� �׽�Ʈ Ȱ��ȭ
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	//���� Ȱ��ȭ
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	//���� ������ ���Լ�
	//D3D11_COMPARISON_LESS�� �񱳵����ͺ��� ���絥���Ͱ�
	//�� z���� ���� ��, �� ���� �����Ͱ� �� �տ� �������� ����
	//�ٽø��� ���� ������ �� ���̸� �������Ѵ�
	
	depthStencilDesc.StencilEnable = true;
	//���� ���ٽ� �׽�Ʈ Ȱ��ȭ
	depthStencilDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	depthStencilDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
	
	//�ȼ��� ��������� ���� ���ٽ� ����
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	//����-���ٽ� �׽�Ʈ �������� ��,���ٽ� ������ ����(D3D11_STENCIL_OP_KEEP)
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	//���ٽ� �׽�Ʈ�� ����ϰ� ���� �׽�Ʈ�� �����Ҷ�,���ٽ� ���� 1 �ø���
	//�ʿ��ϴٸ� ����� wrap��(D3D11_STENCIL_OP_INCR)
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	//����-���ٽ� �׽�Ʈ�� ��� ���� ��,���ٽ� ������ ����
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	//���ٽ� ������ ���Լ�
	//�׻� ����

	//�ȼ��� �޸������ ���� ���ٽ� ����
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	//���ٽ� �׽�Ʈ�� ����ϰ� ���� �׽�Ʈ�� �����Ҷ� ���ٽ� ���� 1 ������
	//�ʿ��ϴٸ� ����� wrap��(D3D11_STENCIL_OP_DECR)
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	//���� ���ٽ� ���� ����
	result = m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencillState);
	if(FAILED(result))
	{
		return false;
	}

	//���� ���ٽ� ���� ����
	m_deviceContext->OMSetDepthStencilState(m_depthStencillState, 1);
	//OMSetDepthStencilState->���ٽ� ���¸� ��´ܰ迡�� ����

	//���� ���ٽ� �� �ʱ�ȭ
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	//���� ���ٽ� �� ����ü ����
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	//���̹��� 24bit ���ٽ� 8bit ����
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	//����-���ٽ� �䰡 2D �ؽ�ó�� �׼���
	depthStencilViewDesc.Texture2D.MipSlice = 0;
	//���Ǵ� ù���� mipmap �ε���

	//���� ���ٽ� �� ����
	result = m_device->CreateDepthStencilView(m_depthStencillBuffer, 
		&depthStencilViewDesc, &m_depthStencillView);
	if (FAILED(result))
	{
		return false;
	}

	//������ Ÿ�� ��� ���� ���ٽ� ���۸� ��� ������ ���������ο� ���ε�
	m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencillView);

	//��� �׸��� � �������� �׸��� �����ϴ� ������ ����ü�� ����
	rasterDesc.AntialiasedLineEnable = false;
	//��Ƽ���ϸ���� ���� ����
	rasterDesc.CullMode = D3D11_CULL_BACK;
	//�ĸ鿡 �����ϴ°��� �׸��� ����
	//�ø�:������ �� �ʿ䰡 ���� ���� ������ ����(�׸��� ����)
	rasterDesc.DepthBias = 0;
	//�ȼ��� �ο��ϴ� ���̰�
	rasterDesc.DepthBiasClamp = 0.0f;
	//
	rasterDesc.DepthClipEnable = true;
	//z�� ���� Ŭ���� ���ɿ���(Ŭ����->����Ʈ(���̴� ����,������)����
	//������ ó��
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	//D3D11_FILL_WIREFRAME -> ���� ����
	//D3D11_FILL_SOLID -> �ӵ� ä��,ǥ��
	rasterDesc.FrontCounterClockwise = false;
	//�ݽð� ������ �ո����� ���� -> �����ۼ�
	rasterDesc.MultisampleEnable = false;
	//��Ƽ���ø� ���� ����
	//true�� �纯�� ���� ��Ƽ ���ϸ����
	//false�� ���� ���� ��Ƽ ���ϸ���� -> �� �ڼ��ϰ�
	rasterDesc.ScissorEnable = false;
	//Rectangle ������ ������� �� �ø��ϴ� ����ε�
	rasterDesc.SlopeScaledDepthBias = 0.0f;
	//�ȼ� ���⿡ ��Į���� �ִµ�
	
	//�ۼ��� ������ ����ü�� �����Ͷ����� ���¸� �����Ѵ�
	result = m_device->CreateRasterizerState(&rasterDesc, &m_rasterState);
	if (FAILED(result))
	{
		return false;
	}

	//�����Ͷ����� ���� ����
	m_deviceContext->RSSetState(m_rasterState);

	//�������� ���� ����Ʈ ����
	viewport.Width = (float)screenWidth;
	viewport.Height = (float)screenHeight;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	//����Ʈ ����
	m_deviceContext->RSSetViewports(1, &viewport);

	//���� ���(projection) ����
	fieldOfView = (float)XM_PIDIV4;
	screenAspect = (float)screenWidth / (float)screenHeight;

	//3D �������� �ϱ� ���� projection(����) ��Ʈ���� ����
	m_projectionMatrix = XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, screenNear, screenDepth);

	//world ����� �׵����(I-���)�� �ʱ�ȭ
	m_worldMatrix = XMMatrixIdentity();

	//2D �������� ���� ���� ���� ����� ����
	m_orthoMatrix = XMMatrixOrthographicLH((float)screenWidth, (float)screenHeight, screenNear, screenDepth);

	return true;
}

void D3DClass::Shutdown()
{
	//���α׷� ���� �� ������ �������� ����, �׷��� ������ ���� ü�� �ÿ� ���� �߻�
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

	//���۸� ���� ���� ����
	color[0] = red;
	color[1] = green;
	color[2] = blue;
	color[3] = alpha;

	//�� ���۸� ����
	m_deviceContext->ClearRenderTargetView(m_renderTargetView, color);

	//���� ���۸� ����
	m_deviceContext->ClearDepthStencilView(m_depthStencillView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	return;
}

void D3DClass::EndScene()
{
	//�������� �ϼ��Ǹ� ����۸� ���
	if (m_vsync_enabled)
	{
		//ȭ�� refresh(���ΰ�ħ)���� ����
		m_swapChain->Present(1, 0);
	}
	else
	{
		//�ִ��� ������ ���
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

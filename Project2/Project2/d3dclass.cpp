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
	//DXGI�� ����ϱ� ���� �ʿ��� ���� �ڿ��� ������ �������̽�
	IDXGIAdapter* adapter;
	//DXGI���� �׷���/���÷��� ���ñ���� �����ϴ� �������̽�
	IDXGIOutput* adapterOutput;
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
	//2D�ؽ�ó ����ü
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
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

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
}
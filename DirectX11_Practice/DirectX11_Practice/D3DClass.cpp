#include "stdafx.h"
#include "D3DClass.h"

D3DClass::D3DClass()
{

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
	//HRESULT -> ���� ���� �� ��� ������ ��Ÿ��
	//bool���� ���������� true/false ��� succeeded/failed�� ���
	//32��Ʈ�� signed�� �����̸� COM �������̽��� �Ҽӵ� ��κ��� �Լ����� HRESULT������ ��ȯ���� ����
	HRESULT result;
	//��������ȭ ���¸� ����
	m_vsync_enabled = vsync;

	//DirectX�׷��� �������̽� ���丮�� ����
	IDXGIFactory* factory = nullptr;
	result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
	if (FAILED(result))
	{
		return false;
	}

	//���丮 ������Ʈ�� ����Ͽ� ù��° �׷��� ī�� �������̽� ����͸� ����
	IDXGIAdapter* adapter = nullptr;
	result = factory->EnumAdapters(0, &adapter);
	if (FAILED(result))
	{
		return false;
	}

	//���(�����)�� ���� ù��° ����͸� ����
	IDXGIOutput* adapterOutput = nullptr;
	result = adapter->EnumOutputs(0, &adapterOutput);
	if (FAILED(result))
	{
		return false;
	}

	//���(�����)�� ���� DXGI_FORMAT_R8G8B8A8_UNORM ǥ�� ���Ŀ� �´� ��� ���� �����´�
	//DXGI_FORMAT_R8G8B8A8_UNORM -> ä�δ� 8��Ʈ�� �����ϴ� 4�� ��������� 32��Ʈ ��ȣ ���� ����ȭ�� ���� ����
}
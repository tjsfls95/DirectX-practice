#pragma once

#ifndef _D3DCLASS_H_
#define _D3DCLASS_H_

//���̺귯�� ��ũ
#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"d3dcompiler.lib")

#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>

using namespace DirectX;

//warning C4316 ó����
template<size_t Alignment>
class AlingedAllocationPolicy
{
public:
	static void* operator new(size_t size)
	{
		return _aligned_malloc(size, Alignment);
	}

	static void operator delete(void* memory)
	{
		_aligned_free(memory);
	}
};




//d3dx10math.h�� �ֽŹ��������� ������� �ʱ� ������
//DirectXMath.h�� ����ؾ���

class D3DClass
{
public:
	D3DClass();
	D3DClass(const D3DClass&);
	~D3DClass();

	bool Initialize(int, int, bool, HWND, bool, float, float);
	void Shutdown();

	void BeginScene(float, float, float, float);
	void EndScene();

	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetDeviceContext();
	
	//XMFLOAT4X4 XMMATRIX ����?
	void GetProjectionMatrix(XMMATRIX&);
	void GetWorldMatrix(XMMATRIX&);
	void GetOrthoMatrix(XMMATRIX&);

	void GetVideoCardInfo(char*, int&);

private:
	bool m_vsync_enabled = false;
	int m_videoCardMemory = 0;
	char m_videoCardDescription[128] = { 0, };
	IDXGISwapChain* m_swapChain = nullptr;
	//DXGI�� ����ü�� ����� ����
	ID3D11Device* m_device = nullptr;
	//Device ��� ���� ���˰� �ڿ� �Ҵ�
	ID3D11DeviceContext* m_deviceContext = nullptr;
	//������ ����� �����ϰ�, �ڿ��� �׷��� ���������ο� �Ҵ��ϰ�
	//GPU�� ������ ����� �����ϴµ��� ���δ�
	ID3D11RenderTargetView* m_renderTargetView = nullptr;
	//���۸� ����Ÿ������ �����ϱ� ���ؼ���
	//������ �ۼ��� ����Ÿ�ٺ並 ����̽��� ����Ÿ������ ����
	ID3D11Texture2D* m_depthStencillBuffer = nullptr;
	//2D �ؽ���(2���� �׷���) �������̽�
	ID3D11DepthStencilState* m_depthStencillState = nullptr;
	//Depth���ۿ� Stencil������ ���� ����
	ID3D11DepthStencilView* m_depthStencillView = nullptr;
	//Depth���ۿ� Stencil������
	ID3D11RasterizerState* m_rasterState = nullptr;
	//Reasterize ���� ���� ����
	XMMATRIX m_projectionMatrix;
	XMMATRIX m_worldMatrix;
	XMMATRIX m_orthoMatrix;
	//4x4¥�� ��Ʈ����
};



#endif // !_D3DCLASS_H_


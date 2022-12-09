#pragma once

#ifndef _D3DCLASS_H_
#define _D3DCLASS_H_

//���̺귯�� ��ũ
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"d3dx11.lib")
#pragma comment(lib,"d3dx10.lib")

#include<dxgi.h>
#include<d3dcommon.h>
#include<d3d11.h>
#include<DirectXMath.h>
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

	void GetProjectionMatrix(D3DXMATRIX&);
	void GetWorldMatrix(D3DXMATRIX&);
	void GetOrthoMatrix(D3DXMATRIX&);

	void GetVideoCardInfo(char*, int&);

private:
	bool m_vsync_enabled;
	int m_videoCardMemory;
	char m_videoCardDescription[128];
	IDXGISwapChain* m_swapChain;
	//DXGI�� ����ü�� ����� ����
	ID3D11Device* m_device;
	//Device ��� ���� ���˰� �ڿ� �Ҵ�
	ID3D11DeviceContext* m_deviceContext;
	//������ ����� �����ϰ�, �ڿ��� �׷��� ���������ο� �Ҵ��ϰ�
	//GPU�� ������ ����� �����ϴµ��� ���δ�
	ID3D11RenderTargetView* m_renderTargetView;
	//���۸� ����Ÿ������ �����ϱ� ���ؼ���
	//������ �ۼ��� ����Ÿ�ٺ並 ����̽��� ����Ÿ������ ����
	ID3D11Texture2D* m_depthStencillBuffer;
	//2D �ؽ���(2���� �׷���) �������̽�
	ID3D11DepthStencilState* m_depthStencillState;
	//Depth���ۿ� Stencil������ ���� ����
	ID3D11DepthStencilView* m_depthStencillView;
	//Depth���ۿ� Stencil������
	ID3D11RasterizerState* m_rasterState;
	//Reasterize ���� ���� ����
	DirectX::XMFLOAT4X4 m_projectionMatrix;
	DirectX::XMFLOAT4X4 m_worldMatrix;
	DirectX::XMFLOAT4X4 m_orthoMatrix;
	//4x4¥�� ��Ʈ����
};



#endif // !_D3DCLASS_H_


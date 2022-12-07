#pragma once

#ifndef _D3DCLASS_H_
#define _D3DCLASS_H_

//라이브러리 링크
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"d3dx11.lib")
#pragma comment(lib,"d3dx10.lib")

#include<dxgi.h>
#include<d3dcommon.h>
#include<d3d11.h>
#include<DirectXMath.h>
//d3dx10math.h는 최신버전에서는 사용하지 않기 때문에
//DirectXMath.h를 사용해야함

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
	ID3D11Device* m_device;
	ID3D11DeviceContext* m_deviceContext;
	ID3D11RenderTargetView* m_renderTargetView;
	ID3D11Texture2D* m_depthStencillBuffer;
	ID3D11DepthStencilState* m_depthStencillState;
	ID3D11DepthStencilView* m_depthStencillView;
	ID3D11RasterizerState* m_rasterState;
	XMFLOAT4X4 m_projectionMatrix;
};



#endif // !_D3DCLASS_H_


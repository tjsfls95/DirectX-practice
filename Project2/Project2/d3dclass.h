#pragma once

#ifndef _D3DCLASS_H_
#define _D3DCLASS_H_

//라이브러리 링크
#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"d3dcompiler.lib")

#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>

using namespace DirectX;

//warning C4316 처리용
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
	
	//XMFLOAT4X4 XMMATRIX 차이?
	void GetProjectionMatrix(XMMATRIX&);
	void GetWorldMatrix(XMMATRIX&);
	void GetOrthoMatrix(XMMATRIX&);

	void GetVideoCardInfo(char*, int&);

private:
	bool m_vsync_enabled = false;
	int m_videoCardMemory = 0;
	char m_videoCardDescription[128] = { 0, };
	IDXGISwapChain* m_swapChain = nullptr;
	//DXGI의 스왑체인 기능을 지원
	ID3D11Device* m_device = nullptr;
	//Device 기능 지원 점검과 자원 할당
	ID3D11DeviceContext* m_deviceContext = nullptr;
	//렌더링 대상을 설정하고, 자원을 그래픽 파이프라인에 할당하고
	//GPU가 렌더링 명령을 지시하는데에 쓰인다
	ID3D11RenderTargetView* m_renderTargetView = nullptr;
	//버퍼를 렌더타겟으로 설정하기 위해서는
	//버퍼의 작성된 렌더타겟뷰를 디바이스의 렌더타겟으로 설정
	ID3D11Texture2D* m_depthStencillBuffer = nullptr;
	//2D 텍스쳐(2차원 그래픽) 인터페이스
	ID3D11DepthStencilState* m_depthStencillState = nullptr;
	//Depth버퍼와 Stencil버퍼의 설정 세팅
	ID3D11DepthStencilView* m_depthStencillView = nullptr;
	//Depth버퍼와 Stencil버퍼의
	ID3D11RasterizerState* m_rasterState = nullptr;
	//Reasterize 버퍼 설정 세팅
	XMMATRIX m_projectionMatrix;
	XMMATRIX m_worldMatrix;
	XMMATRIX m_orthoMatrix;
	//4x4짜리 매트릭스
};



#endif // !_D3DCLASS_H_


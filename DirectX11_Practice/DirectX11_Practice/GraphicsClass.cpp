#include "stdafx.h"
#include "D3DClass.h"
#include "GraphicsClass.h"

GraphicsClass::GraphicsClass()
{

}

GraphicsClass::GraphicsClass(const GraphicsClass& other)
{

}

GraphicsClass::~GraphicsClass()
{

}

bool GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	//Direct3D 오브젝트 생성
	m_Direct3D = (D3DClass*)_aligned_malloc(sizeof(D3DClass), 16);
	if (!m_Direct3D)
	{
		return false;
	}

	//Direct3D 오브젝트 초기화
	bool result = m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize DIrect3D.", L"Error", MB_OK);
		return false;
	}

	return true;
}

void GraphicsClass::Shutdown()
{
	//Direct3D 오브젝트 반환
	if (m_Direct3D)
	{
		m_Direct3D->Shutdown();
		_aligned_free(m_Direct3D);
		m_Direct3D = 0;
	}
}

bool GraphicsClass::Frame()
{
	//그래픽 렌더링 처리
	return Render();
}

bool GraphicsClass::Render()
{
	//씬을 그리기 위해 버퍼를 지움
	m_Direct3D->BeginScene(0.5f, 0.5f, 0.5f, 1.0f);

	//버퍼의 내용을 화면에 출력함
	m_Direct3D->EndScene();

	return true;
}
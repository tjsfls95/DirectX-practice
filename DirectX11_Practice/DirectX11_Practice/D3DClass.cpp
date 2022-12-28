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
	//HRESULT -> 오류 조건 및 경고 조건을 나타냄
	//bool형과 마찬가지로 true/false 대신 succeeded/failed를 사용
	//32비트의 signed형 정수이며 COM 인터페이스에 소속된 대부분의 함수들은 HRESULT형태의 반환값을 가짐
	HRESULT result;
	//수직동기화 상태를 저장
	m_vsync_enabled = vsync;

	//DirectX그래픽 인터페이스 팩토리를 생성
	IDXGIFactory* factory = nullptr;
	result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
	if (FAILED(result))
	{
		return false;
	}

	//팩토리 오브젝트를 사용하여 첫번째 그래픽 카드 인터페이스 어댑터를 생성
	IDXGIAdapter* adapter = nullptr;
	result = factory->EnumAdapters(0, &adapter);
	if (FAILED(result))
	{
		return false;
	}

	//출력(모니터)에 대한 첫번째 어댑터를 지정
	IDXGIOutput* adapterOutput = nullptr;
	result = adapter->EnumOutputs(0, &adapterOutput);
	if (FAILED(result))
	{
		return false;
	}

	//출력(모니터)에 대한 DXGI_FORMAT_R8G8B8A8_UNORM 표시 형식에 맞는 모드 수를 가져온다
	//DXGI_FORMAT_R8G8B8A8_UNORM -> 채널당 8비트를 지원하는 4개 구성요소의 32비트 부호 없는 정규화된 정수 형식
}
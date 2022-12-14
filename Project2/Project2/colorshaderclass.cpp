#include "colorshaderclass.h"
ColorShaderClass::ColorShaderClass()
{
	m_vertexShader = 0;
	m_pixelShader = 0;
	m_layout = 0;
	m_matrixBuffer = 0;
}

ColorShaderClass::ColorShaderClass(const ColorShaderClass& other)
{

}

ColorShaderClass::~ColorShaderClass()
{

}

bool ColorShaderClass::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result;

	//그냥 매개변수로 넣으면 호환문제로 오류발생
	WCHAR vs[] = L"../Project2/color.hlsl";
	WCHAR ps[] = L"../Project2/color_p.hlsl";

	//버텍스와 픽셀 쉐이더 초기화
	result = InitializeShader(device, hwnd, 
		vs,
		ps);
	if (!result)
	{
		return false;
	}

	return true;
}

void ColorShaderClass::Shutdown()
{
	//버텍스 및 쉐이더와 관련된 객체를 종료
	ShutdownShader();
}

bool ColorShaderClass::Render(ID3D11DeviceContext* deviceContext,
	int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
	XMMATRIX projectionMatrix)
{
	bool result;

	//렌더링에 사용할 쉐이더 매개변수 설정
	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix);
	if (!result)
	{
		return false;
	}

	//쉐이더와 함께 준비된 버퍼를 렌더링
	RenderShader(deviceContext, indexCount);

	return true;
}

bool ColorShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename)
{
	HRESULT result;
	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	unsigned int numElements;
	D3D11_BUFFER_DESC matrixBufferDesc;

	//null값으로 사용할 이 함수에 쓰이는 포인터를 초기화
	errorMessage = nullptr;
	vertexShaderBuffer = nullptr;
	pixelShaderBuffer = nullptr;

	//버텍스 쉐이더 코드를 컴파일
	result = D3DCompileFromFile(vsFilename, NULL, NULL,
		"ColorVertexShader", "vs_5_0",
		D3D10_SHADER_ENABLE_STRICTNESS,
		0, &vertexShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		//쉐이더 컴파일을 실패한다면 에러 메세지를 출력
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
		}
		else
		{
			MessageBox(hwnd, vsFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}

	//픽셀 쉐이더 코드 컴파일
	result = D3DCompileFromFile(psFilename, NULL, NULL,
		"ColorPixelShader", "ps_5_0",
		D3D10_SHADER_ENABLE_STRICTNESS, 0,
		&pixelShaderBuffer, &errorMessage);

}
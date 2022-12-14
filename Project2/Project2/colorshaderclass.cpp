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

	//�׳� �Ű������� ������ ȣȯ������ �����߻�
	WCHAR vs[] = L"../Project2/color.hlsl";
	WCHAR ps[] = L"../Project2/color_p.hlsl";

	//���ؽ��� �ȼ� ���̴� �ʱ�ȭ
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
	//���ؽ� �� ���̴��� ���õ� ��ü�� ����
	ShutdownShader();
}

bool ColorShaderClass::Render(ID3D11DeviceContext* deviceContext,
	int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
	XMMATRIX projectionMatrix)
{
	bool result;

	//�������� ����� ���̴� �Ű����� ����
	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix);
	if (!result)
	{
		return false;
	}

	//���̴��� �Բ� �غ�� ���۸� ������
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

	//null������ ����� �� �Լ��� ���̴� �����͸� �ʱ�ȭ
	errorMessage = nullptr;
	vertexShaderBuffer = nullptr;
	pixelShaderBuffer = nullptr;

	//���ؽ� ���̴� �ڵ带 ������
	result = D3DCompileFromFile(vsFilename, NULL, NULL,
		"ColorVertexShader", "vs_5_0",
		D3D10_SHADER_ENABLE_STRICTNESS,
		0, &vertexShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		//���̴� �������� �����Ѵٸ� ���� �޼����� ���
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

	//�ȼ� ���̴� �ڵ� ������
	result = D3DCompileFromFile(psFilename, NULL, NULL,
		"ColorPixelShader", "ps_5_0",
		D3D10_SHADER_ENABLE_STRICTNESS, 0,
		&pixelShaderBuffer, &errorMessage);

}
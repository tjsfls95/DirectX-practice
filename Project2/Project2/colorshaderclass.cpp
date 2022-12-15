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
	if (FAILED(result))
	{
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
		}
		else
		{
			MessageBox(hwnd, psFilename, L"Missing Shader File", MB_OK);
		}
	}

	//���ۿ� ���ؽ� ���̴� ����
	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
	if (FAILED(result))
	{
		return false;
	}

	//���ۿ� �ȼ� ���̴� ����
	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
	if (FAILED(result))
	{
		return false;
	}

	//���̴��� �Է��ϴ� ������ ���̾ƿ��� �����Ѵ�
	//���̴��� ��Ŭ���� �ȿ� �ִ� ���ؽ�Ÿ�� ����ü�� 
	//���Ͽ� ��ġ�ؾ��Ѵ�

	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "COLOR";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	//���̾ƿ����� ������� ������ �����´�
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	//���ؽ� �Է� ���̾ƿ��� ����
	result = device->CreateInputLayout(polygonLayout, numElements,
		vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(),
		&m_layout);
	if (FAILED(result))
	{
		return false;
	}

	//�� �̻� �ʿ� ������ ���ؽ� ���̴� ���ۿ� �ȼ� ���̴� ���� ����
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	//���ؽ� ���̴��� �ִ� ������Ļ������ ����ü�� �����Ѵ�
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	//��� ���� �����͸� �����Ͽ� �� Ŭ�������� ���ؽ� ���̴� ��� ���ۿ� �����ϵ��� �Ѵ�
	result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

void ColorShaderClass::ShutdownShader()
{
	//��� ��� ���۸� ����
	if (m_matrixBuffer)
	{
		m_matrixBuffer->Release();
		m_matrixBuffer = 0;
	}

	//���̾ƿ� ����
	if (m_layout)
	{
		m_layout->Release();
		m_layout = 0;
	}

	//�ȼ� ���̴� ����
	if (m_pixelShader)
	{
		m_pixelShader->Release();
		m_pixelShader = 0;
	}

	//���ؽ� ���̴� ����
	if (m_vertexShader)
	{
		m_vertexShader->Release();
		m_vertexShader = 0;
	}
}

void ColorShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFIlename)
{
	char* compileErrors;
	unsigned long bufferSize, i;
	ofstream fout;

	//���� �޼����� �ؽ�Ʈ ���� �����͸� �����´�
	compileErrors = (char*)(errorMessage->GetBufferPointer());

	//�޼��� ���̸� �����´�
	bufferSize = errorMessage->GetBufferSize();

	//���� �޼����� �ۼ��� ������ ����
	fout.open("shader-error.txt");

	//���� �޼��� �ۼ�
	for (i = 0; i < bufferSize; i++)
	{
		fout << compileErrors[i];
	}

	//���� ����
	fout.close();

	//���� �޼��� ����
	errorMessage->Release();
	errorMessage = 0;

	//������ ������ ����� ������ �ؽ�Ʈ�� ���� �� �ֵ���
	//ȭ�鿡 �����ִ� �˾� �޼����� �ۼ�
	MessageBox(hwnd, L"Error compiling shader.Check shader-error.txt for message", shaderFIlename, MB_OK);
}

bool ColorShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext,
	XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	unsigned int bufferNumber;

	//����� transpose�Ͽ� ���̴��� ����Ҽ� �ְ� �Ѵ�
	worldMatrix = XMMatrixTranspose(worldMatrix);
	viewMatrix = XMMatrixTranspose(viewMatrix);
	projectionMatrix = XMMatrixTranspose(projectionMatrix);

	//��� ������ ������ �� �� �ֵ��� �����Ѵ�
	result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0,
		&mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	//��� ������ ������ �����͸� �����´�
	dataPtr = (MatrixBufferType*)mappedResource.pData;

	//������� ���� ��� ����
	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;

	//��� ���� ����
	deviceContext->Unmap(m_matrixBuffer, 0);

	//���ؽ� ���̴��� ������� �������� �����Ѵ�
	bufferNumber = 0;

	//���ؽ� ���̴��� ������۸� �ٲﰪ���� ����
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

	return true;
}




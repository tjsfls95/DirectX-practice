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

	//버퍼에 버텍스 쉐이더 생성
	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
	if (FAILED(result))
	{
		return false;
	}

	//버퍼에 픽셀 쉐이더 생성
	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
	if (FAILED(result))
	{
		return false;
	}

	//쉐이더에 입력하는 데이터 레이아웃을 설정한다
	//쉐이더와 모델클래스 안에 있는 버텍스타입 구조체와 
	//비교하여 일치해야한다

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

	//레이아웃에서 구성요소 갯수를 가져온다
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	//버텍스 입력 레이아웃을 생성
	result = device->CreateInputLayout(polygonLayout, numElements,
		vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(),
		&m_layout);
	if (FAILED(result))
	{
		return false;
	}

	//더 이상 필요 없으면 버텍스 쉐이더 버퍼와 픽셀 쉐이더 버퍼 해제
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	//버텍스 쉐이더에 있는 동적행렬상수버퍼 구조체를 설정한다
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	//상수 버퍼 포인터를 생성하여 이 클래스에서 버텍스 쉐이더 상수 버퍼에 접근하도록 한다
	result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

void ColorShaderClass::ShutdownShader()
{
	//행렬 상수 버퍼를 해제
	if (m_matrixBuffer)
	{
		m_matrixBuffer->Release();
		m_matrixBuffer = 0;
	}

	//레이아웃 해제
	if (m_layout)
	{
		m_layout->Release();
		m_layout = 0;
	}

	//픽셀 쉐이더 해제
	if (m_pixelShader)
	{
		m_pixelShader->Release();
		m_pixelShader = 0;
	}

	//버텍스 쉐이더 해제
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

	//에러 메세지의 텍스트 버퍼 포인터를 가져온다
	compileErrors = (char*)(errorMessage->GetBufferPointer());

	//메세지 길이를 가져온다
	bufferSize = errorMessage->GetBufferSize();

	//에러 메세지를 작성할 파일을 연다
	fout.open("shader-error.txt");

	//에러 메세지 작성
	for (i = 0; i < bufferSize; i++)
	{
		fout << compileErrors[i];
	}

	//파일 종료
	fout.close();

	//에러 메세지 해제
	errorMessage->Release();
	errorMessage = 0;

	//컴파일 에러가 생기면 유저가 텍스트를 읽을 수 있도록
	//화면에 보여주는 팝업 메세지를 작성
	MessageBox(hwnd, L"Error compiling shader.Check shader-error.txt for message", shaderFIlename, MB_OK);
}

bool ColorShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext,
	XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	unsigned int bufferNumber;

	//행렬을 transpose하여 쉐이더에 사용할수 있게 한다
	worldMatrix = XMMatrixTranspose(worldMatrix);
	viewMatrix = XMMatrixTranspose(viewMatrix);
	projectionMatrix = XMMatrixTranspose(projectionMatrix);

	//상수 버퍼의 내용을 쓸 수 있도로 고정한다
	result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0,
		&mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	//상수 버퍼의 데이터 포인터를 가져온다
	dataPtr = (MatrixBufferType*)mappedResource.pData;

	//상수버퍼 안의 행렬 복사
	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;

	//상수 버퍼 해제
	deviceContext->Unmap(m_matrixBuffer, 0);

	//버텍스 쉐이더의 상수버퍼 포지션을 설정한다
	bufferNumber = 0;

	//버텍스 쉐이더의 상수버퍼를 바뀐값으로 변경
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

	return true;
}




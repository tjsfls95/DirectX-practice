//warning C4316에러 처리
//XMMATRIX는 16바이트 align을 할당받아야하는데 Visual Studio에 내장되어 사용되는 new/delete의 경우 8바이트 align임

#pragma once

template<size_t Alignment>
class AlignedAllocationPolicy
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

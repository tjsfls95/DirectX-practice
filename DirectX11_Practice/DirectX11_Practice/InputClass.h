//윈도우 입력 사용부분
//SystemClass:MessageHandler 함수로부터 입력을 받아 키보드의 사용자 입력을 처리
//입력 오브젝트는 키보드 배열에 각 키의 상태를 저장
//요청을 받으면 특정 키가 눌러졌는지 호출 함수에 전달

#pragma once

class InputClass
{
public:
	InputClass();
	InputClass(const InputClass&);
	~InputClass();

	void Initialize();

	void KeyDown(unsigned int);
	void KeyUp(unsigned int);

	bool IsKeyDown(unsigned int);

private:
	bool m_keys[256];
};

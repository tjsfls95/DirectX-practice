//������ �Է� ���κ�
//SystemClass:MessageHandler �Լ��κ��� �Է��� �޾� Ű������ ����� �Է��� ó��
//�Է� ������Ʈ�� Ű���� �迭�� �� Ű�� ���¸� ����
//��û�� ������ Ư�� Ű�� ���������� ȣ�� �Լ��� ����

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

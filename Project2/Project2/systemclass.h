#pragma once

#ifndef  _SYSTEMCLASS_H_
#define _SYSTEMCLASS_H_

#define WIN32_LEAN_AND_MEAN

#include <Windows.h>

#include<stdlib.h>
#include<malloc.h>
#include<memory.h>
#include<tchar.h>

#include "inputclass.h"
#include "graphicsclass.h"

class SystemClass
{
public:
	SystemClass();
	SystemClass(const SystemClass&);
	~SystemClass();

	bool Initialize();
	void Shutdown();
	void Run();

	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);
	//HWND�� ������ �ڵ�� �ü���� �ڿ��� �Ҵ��ϴ� ��(����)
	//UINT�� ���� ����
	//WPARAM�� ���ν����� ���޵Ǵ� �Ű�����
	//LPARAM�� ���ν����� ���޵Ǵ� �Ű�����(�����Ͱ�)
private:
	bool Frame();
	void InitializeWindows(int&, int&);
	void ShutdownWindows();

private:
	LPCWSTR m_applicationName;
	HINSTANCE m_hinstance;
	HWND m_hwnd;

	//Ŭ���� ����
	InputClass* m_Input = nullptr;
	GraphicsClass* m_Graphics = nullptr;
};

static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

static SystemClass* ApplicationHandle = 0;

#endif
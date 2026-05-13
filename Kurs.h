
// Kurs.h: главный файл заголовка для приложения PROJECT_NAME
//

#pragma once

#ifndef __AFXWIN_H__
	#error "включить pch.h до включения этого файла в PCH"
#endif

#include "resource.h"		// основные символы


// CKursApp:
// Сведения о реализации этого класса: Kurs.cpp
//

class CKursApp : public CWinApp
{
public:
	CKursApp();

// Переопределение
public:
	virtual BOOL InitInstance();

// Реализация

	DECLARE_MESSAGE_MAP()
};

extern CKursApp theApp;

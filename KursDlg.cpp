
// KursDlg.cpp: файл реализации
//

#include "pch.h"
#include "framework.h"
#include "Kurs.h"
#include "KursDlg.h"
#include "afxdialogex.h"
#include <afxdlgs.h>
#include <algorithm>
#include <cmath>
#include <limits>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace
{
	constexpr UINT IDC_EDIT_A = 1000;
	constexpr UINT IDC_EDIT_LEFT = 1001;
	constexpr UINT IDC_EDIT_RIGHT = 1002;
	constexpr UINT IDC_EDIT_STEPS = 1003;
	constexpr UINT IDC_BUTTON_CALCULATE = 1004;
	constexpr UINT IDC_BUTTON_SAVE_BMP = 1005;
	constexpr UINT IDC_RESULT_LABEL = 1006;
	constexpr double kRequiredAccuracyPercent = 0.01;
	constexpr int kMaxIntegrationSteps = 16777216;
}


// Диалоговое окно CAboutDlg используется для описания сведений о приложении

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Данные диалогового окна
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // поддержка DDX/DDV

// Реализация
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// Диалоговое окно CKursDlg



CKursDlg::CKursDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_KURS_DIALOG, pParent)
	, m_a(1.0)
	, m_left(0.0)
	, m_right(3.14159265358979323846)
	, m_startSteps(16)
	, m_finalSteps(0)
	, m_integral(0.0)
	, m_lastChangePercent(0.0)
	, m_hasResult(false)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CKursDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CKursDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_CALCULATE, &CKursDlg::OnCalculateClicked)
	ON_BN_CLICKED(IDC_BUTTON_SAVE_BMP, &CKursDlg::OnSaveBmpClicked)
	ON_EN_CHANGE(IDC_EDIT_A, &CKursDlg::OnParameterChanged)
END_MESSAGE_MAP()


// Обработчики сообщений CKursDlg

BOOL CKursDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Добавление пункта "О программе..." в системное меню.

	// IDM_ABOUTBOX должен быть в пределах системной команды.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Задает значок для этого диалогового окна.  Среда делает это автоматически,
	//  если главное окно приложения не является диалоговым
	SetIcon(m_hIcon, TRUE); // Крупный значок
	SetIcon(m_hIcon, FALSE); // Мелкий значок

	SetWindowText(L"Интеграл и график функции");
	SetWindowPos(nullptr, 0, 0, 900, 620, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
	CreateUiControls();
	Recalculate(false);

	return TRUE;  // возврат значения TRUE, если фокус не передан элементу управления
}

void CKursDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// При добавлении кнопки свертывания в диалоговое окно нужно воспользоваться приведенным ниже кодом,
//  чтобы нарисовать значок.  Для приложений MFC, использующих модель документов или представлений,
//  это автоматически выполняется рабочей областью.

void CKursDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // контекст устройства для рисования

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Выравнивание значка по центру клиентского прямоугольника
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Нарисуйте значок
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CPaintDC dc(this);
		DrawGraph(dc);
	}
}

// Система вызывает эту функцию для получения отображения курсора при перемещении
//  свернутого окна.
HCURSOR CKursDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CKursDlg::CreateUiControls()
{
	const int controlsToHide[] = { IDC_STATIC, IDOK, IDCANCEL };
	for (int controlId : controlsToHide)
	{
		CWnd* control = GetDlgItem(controlId);
		if (control != nullptr)
		{
			control->ShowWindow(SW_HIDE);
		}
	}

	CreateWindowW(L"STATIC", L"Функция: y = a * sin(x) + cos(a * x)", WS_CHILD | WS_VISIBLE,
		20, 18, 360, 22, m_hWnd, nullptr, AfxGetInstanceHandle(), nullptr);
	CreateWindowW(L"STATIC", L"Параметр a:", WS_CHILD | WS_VISIBLE,
		20, 52, 110, 22, m_hWnd, nullptr, AfxGetInstanceHandle(), nullptr);
	CreateWindowW(L"STATIC", L"Левая граница:", WS_CHILD | WS_VISIBLE,
		20, 86, 110, 22, m_hWnd, nullptr, AfxGetInstanceHandle(), nullptr);
	CreateWindowW(L"STATIC", L"Правая граница:", WS_CHILD | WS_VISIBLE,
		20, 120, 110, 22, m_hWnd, nullptr, AfxGetInstanceHandle(), nullptr);
	CreateWindowW(L"STATIC", L"Начальные шаги:", WS_CHILD | WS_VISIBLE,
		20, 154, 110, 22, m_hWnd, nullptr, AfxGetInstanceHandle(), nullptr);

	m_editA.Create(WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL, CRect(140, 50, 250, 74), this, IDC_EDIT_A);
	m_editLeft.Create(WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL, CRect(140, 84, 250, 108), this, IDC_EDIT_LEFT);
	m_editRight.Create(WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL, CRect(140, 118, 250, 142), this, IDC_EDIT_RIGHT);
	m_editSteps.Create(WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL, CRect(140, 152, 250, 176), this, IDC_EDIT_STEPS);

	m_editA.SetWindowTextW(L"1.0");
	m_editLeft.SetWindowTextW(L"0.0");
	m_editRight.SetWindowTextW(L"3.1415926535");
	m_editSteps.SetWindowTextW(L"16");

	m_calculateButton.Create(L"Рассчитать", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		CRect(20, 195, 140, 225), this, IDC_BUTTON_CALCULATE);
	m_saveButton.Create(L"Сохранить BMP", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		CRect(155, 195, 295, 225), this, IDC_BUTTON_SAVE_BMP);
	m_resultLabel.Create(L"", WS_CHILD | WS_VISIBLE | SS_LEFT,
		CRect(20, 245, 330, 440), this, IDC_RESULT_LABEL);
}

bool CKursDlg::ReadParameters(bool showErrors)
{
	CString text;
	wchar_t* end = nullptr;

	m_editA.GetWindowTextW(text);
	double a = wcstod(text, &end);
	if (end == static_cast<LPCWSTR>(text) || !std::isfinite(a))
	{
		if (showErrors) AfxMessageBox(L"Введите корректный числовой параметр a.", MB_ICONWARNING);
		return false;
	}

	m_editLeft.GetWindowTextW(text);
	double left = wcstod(text, &end);
	if (end == static_cast<LPCWSTR>(text) || !std::isfinite(left))
	{
		if (showErrors) AfxMessageBox(L"Введите корректную левую границу интегрирования.", MB_ICONWARNING);
		return false;
	}

	m_editRight.GetWindowTextW(text);
	double right = wcstod(text, &end);
	if (end == static_cast<LPCWSTR>(text) || !std::isfinite(right) || right == left)
	{
		if (showErrors) AfxMessageBox(L"Введите корректную правую границу, отличную от левой.", MB_ICONWARNING);
		return false;
	}

	m_editSteps.GetWindowTextW(text);
	int startSteps = _wtoi(text);
	if (startSteps < 1)
	{
		if (showErrors) AfxMessageBox(L"Начальное количество шагов должно быть положительным целым числом.", MB_ICONWARNING);
		return false;
	}

	m_a = a;
	m_left = left;
	m_right = right;
	m_startSteps = startSteps;
	return true;
}

double CKursDlg::FunctionValue(double x) const
{
	return m_a * std::sin(x) + std::cos(m_a * x);
}

double CKursDlg::IntegrateTrapezoid(int steps) const
{
	const double h = (m_right - m_left) / steps;
	double sum = 0.5 * (FunctionValue(m_left) + FunctionValue(m_right));
	for (int i = 1; i < steps; ++i)
	{
		sum += FunctionValue(m_left + h * i);
	}
	return sum * h;
}

bool CKursDlg::Recalculate(bool showErrors)
{
	if (!ReadParameters(showErrors))
	{
		return false;
	}

	int steps = m_startSteps;
	double previous = IntegrateTrapezoid(steps);
	m_lastChangePercent = std::numeric_limits<double>::infinity();

	while (steps <= kMaxIntegrationSteps / 2)
	{
		steps *= 2;
		double current = IntegrateTrapezoid(steps);
		const double denominator = std::max(std::fabs(current), 1.0e-12);
		m_lastChangePercent = std::fabs(current - previous) / denominator * 100.0;
		previous = current;
		if (m_lastChangePercent <= kRequiredAccuracyPercent)
		{
			break;
		}
	}

	m_integral = previous;
	m_finalSteps = steps;
	m_hasResult = true;
	UpdateResultText();
	InvalidateRect(GetGraphRect(), TRUE);
	return true;
}

void CKursDlg::UpdateResultText()
{
	CString text;
	text.Format(L"Интеграл на [%.6g; %.6g]:\r\n%.10g\r\n\r\nПодобранное число шагов: %d\r\nИзменение при удвоении шагов: %.6g %%\r\nТребование: не более %.4g %%",
		m_left, m_right, m_integral, m_finalSteps, m_lastChangePercent, kRequiredAccuracyPercent);
	m_resultLabel.SetWindowTextW(text);
}

CRect CKursDlg::GetGraphRect() const
{
	CRect client;
	GetClientRect(&client);
	return CRect(350, 30, client.right - 25, client.bottom - 45);
}

void CKursDlg::BuildGraphPoints(const CRect& graphRect)
{
	m_graphPoints.clear();
	if (graphRect.Width() <= 1 || graphRect.Height() <= 1)
	{
		return;
	}

	const int sampleCount = std::max(graphRect.Width(), 2);
	std::vector<double> values(sampleCount);
	double minY = std::numeric_limits<double>::infinity();
	double maxY = -std::numeric_limits<double>::infinity();

	for (int i = 0; i < sampleCount; ++i)
	{
		const double t = static_cast<double>(i) / (sampleCount - 1);
		const double x = m_left + (m_right - m_left) * t;
		const double y = FunctionValue(x);
		values[i] = y;
		minY = std::min(minY, y);
		maxY = std::max(maxY, y);
	}

	if (std::fabs(maxY - minY) < 1.0e-12)
	{
		maxY += 1.0;
		minY -= 1.0;
	}

	m_graphPoints.reserve(sampleCount);
	for (int i = 0; i < sampleCount; ++i)
	{
		const double xPart = static_cast<double>(i) / (sampleCount - 1);
		const double yPart = (values[i] - minY) / (maxY - minY);
		const int px = graphRect.left + static_cast<int>(xPart * graphRect.Width());
		const int py = graphRect.bottom - static_cast<int>(yPart * graphRect.Height());
		m_graphPoints.emplace_back(px, py);
	}
}

void CKursDlg::DrawGraph(CDC& dc)
{
	const CRect graphRect = GetGraphRect();
	dc.FillSolidRect(graphRect, RGB(255, 255, 255));
	dc.Rectangle(graphRect);

	CString caption;
	caption.Format(L"График y = %.6g * sin(x) + cos(%.6g * x)", m_a, m_a);
	dc.TextOutW(graphRect.left, graphRect.top - 22, caption);

	if (!m_hasResult)
	{
		return;
	}

	BuildGraphPoints(graphRect);
	if (m_graphPoints.size() < 2)
	{
		return;
	}

	CPen gridPen(PS_DOT, 1, RGB(210, 210, 210));
	CPen* oldPen = dc.SelectObject(&gridPen);
	for (int i = 1; i < 4; ++i)
	{
		const int x = graphRect.left + graphRect.Width() * i / 4;
		const int y = graphRect.top + graphRect.Height() * i / 4;
		dc.MoveTo(x, graphRect.top);
		dc.LineTo(x, graphRect.bottom);
		dc.MoveTo(graphRect.left, y);
		dc.LineTo(graphRect.right, y);
	}

	CPen graphPen(PS_SOLID, 2, RGB(20, 90, 210));
	dc.SelectObject(&graphPen);
	dc.Polyline(m_graphPoints.data(), static_cast<int>(m_graphPoints.size()));
	dc.SelectObject(oldPen);
}

void CKursDlg::OnCalculateClicked()
{
	Recalculate(true);
}

void CKursDlg::OnSaveBmpClicked()
{
	if (!Recalculate(true))
	{
		return;
	}

	RedrawWindow(GetGraphRect(), nullptr, RDW_INVALIDATE | RDW_UPDATENOW);

	CFileDialog dialog(FALSE, L"bmp", L"graph.bmp", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		L"BMP files (*.bmp)|*.bmp|All files (*.*)|*.*||", this);
	if (dialog.DoModal() == IDOK)
	{
		if (!SaveGraphToBmp(dialog.GetPathName()))
		{
			AfxMessageBox(L"Не удалось сохранить график в BMP-файл.", MB_ICONERROR);
		}
	}
}

void CKursDlg::OnParameterChanged()
{
	if (m_editA.GetSafeHwnd() != nullptr)
	{
		Recalculate(false);
	}
}

bool CKursDlg::SaveGraphToBmp(const CString& fileName)
{
	const CRect graphRect = GetGraphRect();
	const int width = graphRect.Width();
	const int height = graphRect.Height();
	if (width <= 0 || height <= 0)
	{
		return false;
	}

	CClientDC windowDc(this);
	CDC memoryDc;
	if (!memoryDc.CreateCompatibleDC(&windowDc))
	{
		return false;
	}

	CBitmap bitmap;
	if (!bitmap.CreateCompatibleBitmap(&windowDc, width, height))
	{
		return false;
	}

	CBitmap* oldBitmap = memoryDc.SelectObject(&bitmap);
	memoryDc.FillSolidRect(0, 0, width, height, RGB(255, 255, 255));
	memoryDc.BitBlt(0, 0, width, height, &windowDc, graphRect.left, graphRect.top, SRCCOPY);
	memoryDc.SelectObject(oldBitmap);

	BITMAPINFOHEADER bih = {};
	bih.biSize = sizeof(BITMAPINFOHEADER);
	bih.biWidth = width;
	bih.biHeight = height;
	bih.biPlanes = 1;
	bih.biBitCount = 24;
	bih.biCompression = BI_RGB;

	const DWORD rowSize = ((width * bih.biBitCount + 31) / 32) * 4;
	const DWORD imageSize = rowSize * height;
	std::vector<BYTE> pixels(imageSize);

	BITMAPINFO bi = {};
	bi.bmiHeader = bih;
	if (GetDIBits(windowDc.GetSafeHdc(), static_cast<HBITMAP>(bitmap.GetSafeHandle()), 0, height,
		pixels.data(), &bi, DIB_RGB_COLORS) == 0)
	{
		return false;
	}

	CFile file;
	if (!file.Open(fileName, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary))
	{
		return false;
	}

	BITMAPFILEHEADER bfh = {};
	bfh.bfType = 0x4D42;
	bfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	bfh.bfSize = bfh.bfOffBits + imageSize;

	file.Write(&bfh, sizeof(bfh));
	file.Write(&bih, sizeof(bih));
	file.Write(pixels.data(), imageSize);
	return true;
}

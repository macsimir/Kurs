// KursDlg.cpp: реализация готового диалогового приложения.
//

#include "pch.h"
#include "framework.h"
#include "Kurs.h"
#include "KursDlg.h"
#include "afxdialogex.h"
#include <afxdlgs.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace
{
	constexpr int kMargin = 18;
	constexpr int kLabelWidth = 118;
	constexpr int kEditWidth = 194;
	constexpr int kRowHeight = 24;
	constexpr int kLeftColumnX = 18;
	constexpr int kRightColumnX = 360;
	constexpr int kFirstRowY = 58;

	CString FormatScore(double score)
	{
		CString text;
		text.Format(_T("%.1f"), score);
		return text;
	}
}

// Диалоговое окно CAboutDlg используется для описания сведений о приложении

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
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
	ON_BN_CLICKED(IDC_BUTTON_CALCULATE, &CKursDlg::OnCalculate)
	ON_BN_CLICKED(IDC_BUTTON_RESET, &CKursDlg::OnReset)
	ON_BN_CLICKED(IDC_BUTTON_EXPORT, &CKursDlg::OnExportReport)
END_MESSAGE_MAP()

BOOL CKursDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		CString strAboutMenu;
		VERIFY(strAboutMenu.LoadString(IDS_ABOUTBOX));
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);
	SetWindowText(_T("Kurs — контроль готовности курсовой работы"));
	MoveWindow(100, 100, 760, 540);

	CreateInterface();
	SetDefaultValues();

	CalculationResult result;
	if (BuildCalculation(result))
	{
		UpdateSummary(result);
		UpdateRecommendations(result);
	}

	return TRUE;
}

void CKursDlg::CreateInterface()
{
	m_titleFont.CreatePointFont(160, _T("Segoe UI Semibold"));
	m_resultFont.CreatePointFont(110, _T("Segoe UI Semibold"));

	m_titleLabel.Create(_T("Планировщик и калькулятор оценки курсовой работы"), WS_CHILD | WS_VISIBLE | SS_LEFT,
		CRect(kMargin, 16, 720, 44), this, IDC_STATIC_TITLE);
	m_titleLabel.SetFont(&m_titleFont);

	const CString labels[] =
	{
		_T("Студент:"),
		_T("Тема:"),
		_T("Руководитель:"),
		_T("Срок сдачи:"),
		_T("Готовность (%):"),
		_T("Теория (0-100):"),
		_T("Практика (0-100):"),
		_T("Отчёт (0-100):"),
		_T("Защита (0-100):"),
		_T("Итог:"),
		_T("Прогресс:"),
		_T("Рекомендации:"),
		_T("Вес: теория 25%, практика 30%, отчёт 25%, защита 20%")
	};

	for (int i = 0; i < 9; ++i)
	{
		const int x = i < 5 ? kLeftColumnX : kRightColumnX;
		const int y = kFirstRowY + (i < 5 ? i : i - 5) * 36;
		m_labels[i].Create(labels[i], WS_CHILD | WS_VISIBLE | SS_LEFT,
			CRect(x, y + 4, x + kLabelWidth, y + kRowHeight), this, IDC_STATIC);
	}

	m_studentEdit.Create(WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
		CRect(kLeftColumnX + kLabelWidth, kFirstRowY, kLeftColumnX + kLabelWidth + kEditWidth, kFirstRowY + kRowHeight), this, IDC_EDIT_STUDENT);
	m_topicEdit.Create(WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
		CRect(kLeftColumnX + kLabelWidth, kFirstRowY + 36, kLeftColumnX + kLabelWidth + kEditWidth, kFirstRowY + 36 + kRowHeight), this, IDC_EDIT_TOPIC);
	m_supervisorEdit.Create(WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
		CRect(kLeftColumnX + kLabelWidth, kFirstRowY + 72, kLeftColumnX + kLabelWidth + kEditWidth, kFirstRowY + 72 + kRowHeight), this, IDC_EDIT_SUPERVISOR);
	m_deadlineEdit.Create(WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
		CRect(kLeftColumnX + kLabelWidth, kFirstRowY + 108, kLeftColumnX + kLabelWidth + kEditWidth, kFirstRowY + 108 + kRowHeight), this, IDC_EDIT_DEADLINE);
	m_progressEdit.Create(WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | ES_NUMBER,
		CRect(kLeftColumnX + kLabelWidth, kFirstRowY + 144, kLeftColumnX + kLabelWidth + kEditWidth, kFirstRowY + 144 + kRowHeight), this, IDC_EDIT_PROGRESS);

	m_theoryEdit.Create(WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | ES_NUMBER,
		CRect(kRightColumnX + kLabelWidth, kFirstRowY, kRightColumnX + kLabelWidth + kEditWidth, kFirstRowY + kRowHeight), this, IDC_EDIT_THEORY);
	m_practiceEdit.Create(WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | ES_NUMBER,
		CRect(kRightColumnX + kLabelWidth, kFirstRowY + 36, kRightColumnX + kLabelWidth + kEditWidth, kFirstRowY + 36 + kRowHeight), this, IDC_EDIT_PRACTICE);
	m_reportEdit.Create(WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | ES_NUMBER,
		CRect(kRightColumnX + kLabelWidth, kFirstRowY + 72, kRightColumnX + kLabelWidth + kEditWidth, kFirstRowY + 72 + kRowHeight), this, IDC_EDIT_REPORT);
	m_defenseEdit.Create(WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | ES_NUMBER,
		CRect(kRightColumnX + kLabelWidth, kFirstRowY + 108, kRightColumnX + kLabelWidth + kEditWidth, kFirstRowY + 108 + kRowHeight), this, IDC_EDIT_DEFENSE);

	m_calculateButton.Create(_T("Рассчитать"), WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
		CRect(kRightColumnX, kFirstRowY + 144, kRightColumnX + 112, kFirstRowY + 172), this, IDC_BUTTON_CALCULATE);
	m_resetButton.Create(_T("Сбросить"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		CRect(kRightColumnX + 124, kFirstRowY + 144, kRightColumnX + 236, kFirstRowY + 172), this, IDC_BUTTON_RESET);
	m_exportButton.Create(_T("Сохранить отчёт"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		CRect(kRightColumnX + 248, kFirstRowY + 144, kRightColumnX + 382, kFirstRowY + 172), this, IDC_BUTTON_EXPORT);

	m_labels[9].Create(labels[9], WS_CHILD | WS_VISIBLE | SS_LEFT,
		CRect(kMargin, 250, kMargin + 80, 272), this, IDC_STATIC);
	m_summaryLabel.Create(_T(""), WS_CHILD | WS_VISIBLE | SS_LEFT | SS_NOPREFIX,
		CRect(94, 246, 724, 288), this, IDC_STATIC_SUMMARY);
	m_summaryLabel.SetFont(&m_resultFont);

	m_labels[10].Create(labels[10], WS_CHILD | WS_VISIBLE | SS_LEFT,
		CRect(kMargin, 300, kMargin + 80, 322), this, IDC_STATIC);
	m_progressBar.Create(WS_CHILD | WS_VISIBLE | PBS_SMOOTH,
		CRect(94, 300, 724, 322), this, IDC_PROGRESS_READY);
	m_progressBar.SetRange(0, 100);

	m_labels[11].Create(labels[11], WS_CHILD | WS_VISIBLE | SS_LEFT,
		CRect(kMargin, 338, 160, 360), this, IDC_STATIC);
	m_recommendationList.Create(WS_CHILD | WS_VISIBLE | WS_BORDER | LBS_NOINTEGRALHEIGHT,
		CRect(kMargin, 362, 724, 452), this, IDC_LIST_RECOMMENDATIONS);

	m_labels[12].Create(labels[12], WS_CHILD | WS_VISIBLE | SS_LEFT,
		CRect(kMargin, 462, 540, 484), this, IDC_STATIC);
	m_closeButton.Create(_T("Закрыть"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		CRect(622, 458, 724, 488), this, IDCANCEL);
}

void CKursDlg::SetDefaultValues()
{
	m_studentEdit.SetWindowText(_T("Иванов И.И."));
	m_topicEdit.SetWindowText(_T("Информационная система учёта курсовых работ"));
	m_supervisorEdit.SetWindowText(_T("Петров П.П."));
	m_deadlineEdit.SetWindowText(_T("31.05.2026"));
	m_progressEdit.SetWindowText(_T("80"));
	m_theoryEdit.SetWindowText(_T("85"));
	m_practiceEdit.SetWindowText(_T("90"));
	m_reportEdit.SetWindowText(_T("82"));
	m_defenseEdit.SetWindowText(_T("88"));
}

bool CKursDlg::ReadNumber(CEdit& edit, int minimum, int maximum, int& value, const CString& fieldName) const
{
	CString text;
	edit.GetWindowText(text);
	text.Trim();

	if (text.IsEmpty())
	{
		MessageBox(_T("Заполните поле: ") + fieldName, _T("Проверка данных"), MB_ICONWARNING);
		edit.SetFocus();
		return false;
	}

	value = _ttoi(text);
	if (value < minimum || value > maximum)
	{
		CString message;
		message.Format(_T("Поле \"%s\" должно быть в диапазоне от %d до %d."), fieldName.GetString(), minimum, maximum);
		MessageBox(message, _T("Проверка данных"), MB_ICONWARNING);
		edit.SetFocus();
		edit.SetSel(0, -1);
		return false;
	}

	return true;
}

bool CKursDlg::BuildCalculation(CalculationResult& result)
{
	CString student;
	m_studentEdit.GetWindowText(student);
	student.Trim();
	if (student.IsEmpty())
	{
		MessageBox(_T("Укажите ФИО студента."), _T("Проверка данных"), MB_ICONWARNING);
		m_studentEdit.SetFocus();
		return false;
	}

	if (!ReadNumber(m_progressEdit, 0, 100, result.progress, _T("Готовность")) ||
		!ReadNumber(m_theoryEdit, 0, 100, result.theory, _T("Теория")) ||
		!ReadNumber(m_practiceEdit, 0, 100, result.practice, _T("Практика")) ||
		!ReadNumber(m_reportEdit, 0, 100, result.report, _T("Отчёт")) ||
		!ReadNumber(m_defenseEdit, 0, 100, result.defense, _T("Защита")))
	{
		return false;
	}

	result.finalScore = result.theory * 0.25 + result.practice * 0.30 + result.report * 0.25 + result.defense * 0.20;

	if (result.finalScore >= 85.0)
	{
		result.grade = _T("отлично");
	}
	else if (result.finalScore >= 70.0)
	{
		result.grade = _T("хорошо");
	}
	else if (result.finalScore >= 55.0)
	{
		result.grade = _T("удовлетворительно");
	}
	else
	{
		result.grade = _T("требуется доработка");
	}

	if (result.progress >= 90 && result.finalScore >= 85.0)
	{
		result.status = _T("работа практически готова к защите");
	}
	else if (result.progress >= 70 && result.finalScore >= 70.0)
	{
		result.status = _T("можно выходить на предзащиту после финальной проверки");
	}
	else if (result.progress >= 50)
	{
		result.status = _T("нужно усилить слабые разделы до допуска");
	}
	else
	{
		result.status = _T("необходим срочный план доработки");
	}

	return true;
}

void CKursDlg::UpdateSummary(const CalculationResult& result)
{
	CString summary;
	summary.Format(_T("%s балла — %s; %s."), FormatScore(result.finalScore).GetString(), result.grade.GetString(), result.status.GetString());
	m_summaryLabel.SetWindowText(summary);
	m_progressBar.SetPos(result.progress);
}

void CKursDlg::UpdateRecommendations(const CalculationResult& result)
{
	m_recommendationList.ResetContent();

	if (result.theory < 70)
	{
		m_recommendationList.AddString(_T("Усилить теоретическую часть: добавить источники, определения и выводы по главам."));
	}
	if (result.practice < 70)
	{
		m_recommendationList.AddString(_T("Доработать практическую часть: сценарии, расчёты, тестовые данные или прототип."));
	}
	if (result.report < 70)
	{
		m_recommendationList.AddString(_T("Проверить оформление отчёта: структура, список литературы, рисунки и приложения."));
	}
	if (result.defense < 70)
	{
		m_recommendationList.AddString(_T("Подготовить защиту: презентацию на 7–10 слайдов и ответы на вопросы комиссии."));
	}
	if (result.progress < 80)
	{
		m_recommendationList.AddString(_T("Составить недельный план: ежедневно закрывать один конкретный пункт до дедлайна."));
	}
	if (m_recommendationList.GetCount() == 0)
	{
		m_recommendationList.AddString(_T("Критичных замечаний нет: провести финальную вычитку и репетицию доклада."));
		m_recommendationList.AddString(_T("Сохранить отчёт и приложить его к курсовой работе как лист контроля готовности."));
	}
}

CString CKursDlg::BuildReportText(const CalculationResult& result) const
{
	CString student, topic, supervisor, deadline;
	m_studentEdit.GetWindowText(student);
	m_topicEdit.GetWindowText(topic);
	m_supervisorEdit.GetWindowText(supervisor);
	m_deadlineEdit.GetWindowText(deadline);

	CString report;
	report.Format(
		_T("Отчёт о готовности курсовой работы\r\n")
		_T("====================================\r\n")
		_T("Студент: %s\r\n")
		_T("Тема: %s\r\n")
		_T("Руководитель: %s\r\n")
		_T("Срок сдачи: %s\r\n\r\n")
		_T("Готовность: %d%%\r\n")
		_T("Теория: %d\r\n")
		_T("Практика: %d\r\n")
		_T("Отчёт: %d\r\n")
		_T("Защита: %d\r\n")
		_T("Итоговый балл: %s\r\n")
		_T("Оценка: %s\r\n")
		_T("Статус: %s\r\n\r\n")
		_T("Рекомендации:\r\n"),
		student.GetString(), topic.GetString(), supervisor.GetString(), deadline.GetString(),
		result.progress, result.theory, result.practice, result.report, result.defense,
		FormatScore(result.finalScore).GetString(), result.grade.GetString(), result.status.GetString());

	for (int i = 0; i < m_recommendationList.GetCount(); ++i)
	{
		CString item;
		m_recommendationList.GetText(i, item);
		report += _T("- ") + item + _T("\r\n");
	}

	return report;
}

void CKursDlg::OnCalculate()
{
	CalculationResult result;
	if (!BuildCalculation(result))
	{
		return;
	}

	UpdateSummary(result);
	UpdateRecommendations(result);
}

void CKursDlg::OnReset()
{
	SetDefaultValues();
	OnCalculate();
}

void CKursDlg::OnExportReport()
{
	CalculationResult result;
	if (!BuildCalculation(result))
	{
		return;
	}

	UpdateSummary(result);
	UpdateRecommendations(result);

	CFileDialog dialog(FALSE, _T("txt"), _T("kurs_report.txt"), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		_T("Текстовые файлы (*.txt)|*.txt|Все файлы (*.*)|*.*||"), this);
	if (dialog.DoModal() != IDOK)
	{
		return;
	}

	CStdioFile file;
	if (!file.Open(dialog.GetPathName(), CFile::modeCreate | CFile::modeWrite | CFile::typeText))
	{
		MessageBox(_T("Не удалось создать файл отчёта."), _T("Сохранение"), MB_ICONERROR);
		return;
	}

	file.WriteString(BuildReportText(result));
	file.Close();
	MessageBox(_T("Отчёт успешно сохранён."), _T("Сохранение"), MB_ICONINFORMATION);
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

void CKursDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this);
		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

HCURSOR CKursDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

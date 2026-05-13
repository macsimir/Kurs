
// KursDlg.h: файл заголовка
//

#pragma once

#include <vector>


// Диалоговое окно CKursDlg
class CKursDlg : public CDialogEx
{
// Создание
public:
	CKursDlg(CWnd* pParent = nullptr); // стандартный конструктор

// Данные диалогового окна
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_KURS_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX); // поддержка DDX/DDV


// Реализация
protected:
	HICON m_hIcon;
	CEdit m_editA;
	CEdit m_editLeft;
	CEdit m_editRight;
	CEdit m_editSteps;
	CStatic m_resultLabel;
	CButton m_calculateButton;
	CButton m_saveButton;

	double m_a;
	double m_left;
	double m_right;
	int m_startSteps;
	int m_finalSteps;
	double m_integral;
	double m_lastChangePercent;
	bool m_hasResult;
	std::vector<CPoint> m_graphPoints;

	// Созданные функции схемы сообщений
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnCalculateClicked();
	afx_msg void OnSaveBmpClicked();
	afx_msg void OnParameterChanged();
	DECLARE_MESSAGE_MAP()

private:
	void CreateUiControls();
	bool ReadParameters(bool showErrors);
	double FunctionValue(double x) const;
	double IntegrateTrapezoid(int steps) const;
	bool Recalculate(bool showErrors);
	void UpdateResultText();
	CRect GetGraphRect() const;
	void DrawGraph(CDC& dc);
	void BuildGraphPoints(const CRect& graphRect);
	bool SaveGraphToBmp(const CString& fileName);
};

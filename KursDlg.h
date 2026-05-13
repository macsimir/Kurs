// KursDlg.h: интерфейс главного диалогового окна приложения.
//

#pragma once

#include <array>

// Диалоговое окно CKursDlg
class CKursDlg : public CDialogEx
{
// Создание
public:
	CKursDlg(CWnd* pParent = nullptr);

// Данные диалогового окна
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_KURS_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

// Реализация
protected:
	HICON m_hIcon;
	CFont m_titleFont;
	CFont m_resultFont;
	std::array<CStatic, 13> m_labels;
	CStatic m_titleLabel;
	CStatic m_summaryLabel;
	CEdit m_studentEdit;
	CEdit m_topicEdit;
	CEdit m_supervisorEdit;
	CEdit m_deadlineEdit;
	CEdit m_progressEdit;
	CEdit m_theoryEdit;
	CEdit m_practiceEdit;
	CEdit m_reportEdit;
	CEdit m_defenseEdit;
	CButton m_calculateButton;
	CButton m_resetButton;
	CButton m_exportButton;
	CButton m_closeButton;
	CProgressCtrl m_progressBar;
	CListBox m_recommendationList;

	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnCalculate();
	afx_msg void OnReset();
	afx_msg void OnExportReport();
	DECLARE_MESSAGE_MAP()

private:
	struct CalculationResult
	{
		int progress = 0;
		int theory = 0;
		int practice = 0;
		int report = 0;
		int defense = 0;
		double finalScore = 0.0;
		CString grade;
		CString status;
	};

	void CreateInterface();
	void SetDefaultValues();
	void UpdateRecommendations(const CalculationResult& result);
	void UpdateSummary(const CalculationResult& result);
	bool ReadNumber(CEdit& edit, int minimum, int maximum, int& value, const CString& fieldName) const;
	bool BuildCalculation(CalculationResult& result);
	CString BuildReportText(const CalculationResult& result) const;
};

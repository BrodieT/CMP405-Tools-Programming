#pragma once
#include "afxdialogex.h"
#include "resource.h"
#include "afxwin.h"
#include "SceneObject.h"
#include <vector>

#include "ToolMain.h"
class TerrainEditor : public CDialogEx
{

	DECLARE_DYNAMIC(TerrainEditor)

public:
	TerrainEditor(CWnd* pParent, std::vector<SceneObject>* SceneGraph);   // modal // takes in out scenegraph in the constructor
	TerrainEditor(CWnd* pParent = NULL);
	virtual ~TerrainEditor();
	void SetObjectData(ToolMain* tM, HWND* windowHandle);	//passing in pointers to the data the class will operate on.

	bool isActive = false;
	void Die() { End(); };
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg void End();		//kill the dialogue

	std::vector<SceneObject>* m_sceneGraph;
	int* m_currentSelection;
	ToolMain* m_toolMain;
	HWND* hwnd;

	DECLARE_MESSAGE_MAP()

public:
	// Control variable for more efficient access of the listbox
	virtual BOOL OnInitDialog() override;
	virtual void PostNcDestroy();

	void UpdateTerrainData();
	CEdit m_heightVal;
	CEdit m_brushWidth;

	CString m_FlatTxt;
	CString m_RaiseTxt;
	CString m_LowerTxt;
	CString m_SmoothTxt;
	CString m_SquareBrush;
	CString m_CircleBrush;

	CComboBox m_brushSelection;
	CComboBox m_editModeSelection;

	bool DoesCStringContain(CString text);
	void UpdateEditor();
	afx_msg void GenerateTerrain();

};

INT_PTR CALLBACK TEditProc(HWND   hwndDlg, UINT   uMsg, WPARAM wParam, LPARAM lParam);
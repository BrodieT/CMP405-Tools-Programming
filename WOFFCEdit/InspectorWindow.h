#pragma once
#include "afxdialogex.h"
#include "resource.h"
#include "afxwin.h"
#include "SceneObject.h"
#include <vector>

#include "ToolMain.h"
class InspectorWindow : public CDialogEx
{

	DECLARE_DYNAMIC(InspectorWindow)

public:
	InspectorWindow(CWnd* pParent, std::vector<SceneObject>* SceneGraph);   // modal // takes in out scenegraph in the constructor
	InspectorWindow(CWnd* pParent = NULL);
	virtual ~InspectorWindow();
	void SetObjectData(std::vector<SceneObject>* SceneGraph, int* Selection, ToolMain* tM, HWND* windowHandle);	//passing in pointers to the data the class will operate on.

	bool isActive = false;
	void Die() { End(); };
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg void End();		//kill the dialogue

	std::vector<SceneObject>* m_sceneGraph;
	int* m_currentSelection;
	ToolMain* toolMain;
	HWND* hwnd;

	DECLARE_MESSAGE_MAP()

public:
	// Control variable for more efficient access of the listbox
	virtual BOOL OnInitDialog() override;
	virtual void PostNcDestroy();
	
	CEdit Transform[10];

	void UpdateGameObject();

	void UpdateInspector();
	bool DoesCStringContain(CString str);
	void SetSelectedObjPos();
	void SetSelectedObjRote();
	void SetSelectedObjScale();
	void SetSelectedObjName();
	
};

INT_PTR CALLBACK InspectProc(HWND   hwndDlg, UINT   uMsg, WPARAM wParam, LPARAM lParam);
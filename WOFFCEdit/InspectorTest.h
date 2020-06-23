#pragma once
#include "afxdialogex.h"
#include "resource.h"
#include "afxwin.h"
#include "SceneObject.h"
#include <vector>

#include "ToolMain.h"
class InspectorTest : public CDialogBar
{

	DECLARE_DYNAMIC(InspectorTest)

public:
	InspectorTest(CWnd* pParent, std::vector<SceneObject>* SceneGraph);   // modal // takes in out scenegraph in the constructor
	InspectorTest(CWnd* pParent = NULL);
	virtual ~InspectorTest();
	void SetObjectData(std::vector<SceneObject>* SceneGraph, int* Selection, ToolMain* tM, HWND* windowHandle);	//passing in pointers to the data the class will operate on.
	void UpdateGameObject();

	void Make(CWnd* papa);
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
	//virtual BOOL OnInitDialog() override;
	virtual void PostNcDestroy();

	CEdit Name;
	CEdit PosX;
	CEdit PosY;
	CEdit PosZ;
	CEdit RoteX;
	CEdit RoteY;
	CEdit RoteZ;
	CEdit ScaleX;
	CEdit ScaleY;
	CEdit ScaleZ;


	void UpdateInspector();
	bool DoesCStringContain(CString str);

};

INT_PTR CALLBACK InspectProc(HWND   hwndDlg, UINT   uMsg, WPARAM wParam, LPARAM lParam);
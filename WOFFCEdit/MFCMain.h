#pragma once

#include <afxwin.h> 
#include <afxext.h>
#include <afx.h>
#include "pch.h"
#include "Game.h"
#include "ToolMain.h"
#include "resource.h"
#include "MFCFrame.h"
#include "SelectDialogue.h"
#include "InspectorWindow.h"
#include "TerrainEditor.h"
#include "InspectorTest.h"
class MFCMain : public CWinApp 
{
public:
	MFCMain();
	~MFCMain();
	BOOL InitInstance();
	int  Run();

private:

	CMyFrame * m_frame;	//handle to the frame where all our UI is
	HWND m_toolHandle;	//Handle to the MFC window
	ToolMain m_ToolSystem;	//Instance of Tool System that we interface to. 
	CRect WindowRECT;	//Window area rectangle. 
	SelectDialogue m_ToolSelectDialogue;			//for modeless dialogue, declare it here
	InspectorWindow m_InspectorDialogue;
	TerrainEditor m_TerrainEditorDialogue;
	//InspectorTest m_InspectorTest2;
	CDialogBar m_InspectorTest;

	int m_width;		
	int m_height;
	
	
	//Interface funtions for menu and toolbar etc requires
	afx_msg void MenuFileQuit();
	afx_msg void MenuFileSaveTerrain();
	afx_msg void MenuEditSelect();
	afx_msg	void SaveScene();

	afx_msg void ShowInspector();
	afx_msg void ShowTerrainEditor();



	afx_msg void NewObject() { m_ToolSystem.Instantiate(); };
	afx_msg void DupeObject() { m_ToolSystem.DuplicateObject(); };
	afx_msg void RemoveObject() { m_ToolSystem.RemoveObject(); };


	afx_msg void Undo() { m_ToolSystem.Undo(); };
	afx_msg void Redo() { m_ToolSystem.Redo(); };

	


	DECLARE_MESSAGE_MAP()	// required macro for message map functionality  One per class
};

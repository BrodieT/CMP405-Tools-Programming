#include "MFCMain.h"
#include "resource.h"


BEGIN_MESSAGE_MAP(MFCMain, CWinApp)
	ON_COMMAND(ID_FILE_QUIT, &MFCMain::MenuFileQuit)
	ON_COMMAND(ID_FILE_SAVETERRAIN, &MFCMain::SaveScene)
	ON_COMMAND(ID_EDIT_NEW, &MFCMain::NewObject)
	ON_COMMAND(ID_EDIT_DUPLICATE, &MFCMain::DupeObject)
	ON_COMMAND(ID_EDIT_REMOVE, &MFCMain::RemoveObject)

	ON_COMMAND(ID_EDIT_SELECT, &MFCMain::MenuEditSelect)
	ON_COMMAND(ID_SAVEBUTTON,	&MFCMain::SaveScene)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_TOOL, &CMyFrame::OnUpdatePage)
	ON_COMMAND(ID_WINDOW_INSPECTOR, &MFCMain::ShowInspector)
	ON_COMMAND(ID_WINDOW_TERRAINEDITOR, &MFCMain::ShowTerrainEditor)
	ON_BN_CLICKED(IDC_UNDO, &MFCMain::Undo)
	ON_BN_CLICKED(IDC_REDO, &MFCMain::Redo)
	ON_BN_CLICKED(IDC_INSPECTOR_TOGGLE, &MFCMain::ShowInspector)
	ON_BN_CLICKED(IDC_TERRAIN_TOGGLE, &MFCMain::ShowTerrainEditor)
END_MESSAGE_MAP()

BOOL MFCMain::InitInstance()
{
	//instanciate the mfc frame
	m_frame = new CMyFrame();
	m_pMainWnd = m_frame;

	m_frame->Create(	NULL,
					_T("World Of Flim-Flam Craft Editor"),
					WS_OVERLAPPEDWINDOW,
					CRect(5, 5, 800, 600),
					NULL,
					NULL,
					0,
					NULL
				);

	//show and set the window to run and update. 
	m_frame->ShowWindow(SW_SHOW);
	m_frame->UpdateWindow();


	//get the rect from the MFC window so we can get its dimensions
	m_toolHandle = m_frame->m_DirXView.GetSafeHwnd();				//handle of directX child window
	m_frame->m_DirXView.GetClientRect(&WindowRECT);
	m_width		= WindowRECT.Width();
	m_height	= WindowRECT.Height();


	m_ToolSystem.onActionInitialise(m_toolHandle, m_width, m_height);
	

	return TRUE;
}

int MFCMain::Run()
{
	MSG msg;
	BOOL bGotMsg;

	PeekMessage(&msg, NULL, 0U, 0U, PM_NOREMOVE);

	while (WM_QUIT != msg.message)
	{
		if (true)
		{
			bGotMsg = (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE) != 0);
		}
		else
		{
			bGotMsg = (GetMessage(&msg, NULL, 0U, 0U) != 0);
		}

		if (bGotMsg)
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			m_ToolSystem.UpdateInput(&msg);
		}
		else
		{	
			int ID = m_ToolSystem.getCurrentSelectionID();
			std::wstring statusString = L"Selected Object: " + std::to_wstring(ID);
			m_ToolSystem.Tick(&msg);

			//send current object ID to status bar in The main frame
			m_frame->m_wndStatusBar.SetPaneText(1, statusString.c_str(), 1);	
		}

		//Update the inspector when there is something to update
		//provided it is active
		if (m_ToolSystem.m_InspectorUpdate && m_InspectorDialogue.isActive)
		{
			m_InspectorDialogue.UpdateInspector();
			m_ToolSystem.m_InspectorUpdate = false;
		}
	}

	

	return (int)msg.wParam;
}

void MFCMain::MenuFileQuit()
{
	//will post message to the message thread that will exit the application normally
	PostQuitMessage(0);
}

void MFCMain::MenuFileSaveTerrain()
{
}

void MFCMain::MenuEditSelect()
{
	//SelectDialogue m_ToolSelectDialogue(NULL, &m_ToolSystem.m_sceneGraph);		//create our dialoguebox //modal constructor
	//m_ToolSelectDialogue.DoModal();	// start it up modal

	//modeless dialogue must be declared in the class.   If we do local it will go out of scope instantly and destroy itself
	m_ToolSelectDialogue.Create(IDD_DIALOG1);	//Start up modeless
	m_ToolSelectDialogue.ShowWindow(SW_SHOW);	//show modeless
	m_ToolSelectDialogue.SetObjectData(&m_ToolSystem.m_sceneGraph, &m_ToolSystem.m_selectedObject);
}

//This function toggles the terrain editor dialogue
void MFCMain::ShowTerrainEditor()
{
	if (m_TerrainEditorDialogue.isActive)
	{
		m_ToolSystem.ToggleTerrainEdit();
		m_TerrainEditorDialogue.Die();
	}
	else
	{
		//modeless dialogue must be declared in the class.   If we do local it will go out of scope instantly and destroy itself
		m_TerrainEditorDialogue.Create(IDD_TERRAIN_EDITOR);	//Start up modeless
		m_TerrainEditorDialogue.ShowWindow(SW_SHOW);	//show modeless
		m_TerrainEditorDialogue.SetObjectData(&m_ToolSystem, &m_toolHandle);
		m_TerrainEditorDialogue.UpdateEditor();
		m_ToolSystem.ToggleTerrainEdit();

	}
}

//This function toggles the inspector dialogue
void MFCMain::ShowInspector()
{
	if (m_InspectorDialogue.isActive)
	{
		m_InspectorDialogue.Die();
	}
	else
	{
		//modeless dialogue must be declared in the class.   If we do local it will go out of scope instantly and destroy itself
		m_InspectorDialogue.Create(IDD_INSPECTOR);	//Start up modeless
		m_InspectorDialogue.ShowWindow(SW_SHOW);	//show modeless
		RECT mainR;
		m_pMainWnd->GetClientRect(&mainR);
		RECT inspR;
		m_InspectorDialogue.GetClientRect(&inspR);


		//m_InspectorDialogue.SetWindowPos(m_pMainWnd, mainR.right - (inspR.right - inspR.left), mainR.top, mainR.right - (inspR.right - inspR.left), mainR.top + (inspR.bottom - inspR.top), SWP_NOREPOSITION);
		m_InspectorDialogue.SetObjectData(&m_ToolSystem.m_sceneGraph, &m_ToolSystem.m_selectedObject, &m_ToolSystem, &m_toolHandle);
		m_InspectorDialogue.UpdateInspector();
	}
}

//This function saves both terrain and objects
void MFCMain::SaveScene()
{
	m_ToolSystem.onActionSave();	
	m_ToolSystem.onActionSaveTerrain();
}


MFCMain::MFCMain()
{
}


MFCMain::~MFCMain()
{
}

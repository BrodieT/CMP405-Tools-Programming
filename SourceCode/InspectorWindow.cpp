
#include "stdafx.h"
#include "InspectorWindow.h"


IMPLEMENT_DYNAMIC(InspectorWindow, CDialogEx)

//Message map.  Just like MFCMAIN.cpp.  This is where we catch button presses etc and point them to a handy dandy method.
BEGIN_MESSAGE_MAP(InspectorWindow, CDialogEx)
	//ON_COMMAND(IDOK, &InspectorWindow::End)					//ok button
	//ON_BN_CLICKED(IDOK, &InspectorWindow::OnBnClickedOk)
	ON_BN_CLICKED(IDC_UPDATEOBJECT, &InspectorWindow::UpdateGameObject)
	
END_MESSAGE_MAP()


InspectorWindow::InspectorWindow(CWnd* pParent, std::vector<SceneObject>* SceneGraph)		//constructor used in modal
	: CDialogEx(IDD_INSPECTOR, pParent)
{
	m_sceneGraph = SceneGraph;
}

InspectorWindow::InspectorWindow(CWnd* pParent)			//constructor used in modeless
	: CDialogEx(IDD_INSPECTOR, pParent)
{
}

InspectorWindow::~InspectorWindow()
{
}

///pass through pointers to the data in the tool we want to manipulate
void InspectorWindow::SetObjectData(std::vector<SceneObject>* SceneGraph, int* selection, ToolMain* tM, HWND* windowHandle)
{
	isActive = true;
	m_sceneGraph = SceneGraph;
	m_currentSelection = selection;
	toolMain = tM;
	hwnd = windowHandle;
}



void InspectorWindow::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_NAME, Transform[0]);
	DDX_Control(pDX, IDC_PosX, Transform[1]);
	DDX_Control(pDX, IDC_PosY, Transform[2]);
	DDX_Control(pDX, IDC_PosZ, Transform[3]);
	DDX_Control(pDX, IDC_RotX, Transform[4]);
	DDX_Control(pDX, IDC_RotY, Transform[5]);
	DDX_Control(pDX, IDC_RotZ, Transform[6]);
	DDX_Control(pDX, IDC_ScaleX, Transform[7]);
	DDX_Control(pDX, IDC_ScaleY, Transform[8]);
	DDX_Control(pDX, IDC_ScaleZ, Transform[9]);


}

void InspectorWindow::End()
{
	isActive = false;
	DestroyWindow();	//destory the window properly.  INcluding the links and pointers created.  THis is so the dialogue can start again. 
}


BOOL InspectorWindow::OnInitDialog()
{
	CDialogEx::OnInitDialog();


	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void InspectorWindow::PostNcDestroy()
{
}

//This function will update the game object based on the contents of the edit controls in the inspector
void InspectorWindow::UpdateGameObject()
{

	//Store a temporary reference to the scene graph
	std::vector<SceneObject> temp;
	temp = *m_sceneGraph;

	//Initialise the selected index
	int index = -1;

	//Find the currently selected object's index in the scene graph
	for (int i = 0; i < temp.size(); i++)
	{
		if (temp.at(i).ID == *m_currentSelection)
		{
			index = i;
			break;
		}
	}

	//If a valid object was found
	if (index != -1)
	{
		//Assign the name parameter (seperated from the rest as it does not need to be checked for non-numerical characters)
		CString nameTxt;
		Transform[0].GetWindowTextW(nameTxt);
		std::string s(CW2A(nameTxt.GetString()));
		temp.at(index).name = s;

		//Loop through the remaining 9 edit controls in the transform
		for (int j = 1; j < 10; j++)
		{
			CString text;

			//Get the text contained in the edit control
			if (Transform[j])
			{
				Transform[j].GetWindowTextW(text);
			}

			//If a number can be extracted, assign to the appropriate parameter
			if (DoesCStringContain(text))
			{
				if (_ttof(text))
				{
					switch (j)
					{
					case 1:
						temp.at(index).posX = _ttof(text);
						break;
					case 2:
						temp.at(index).posY = _ttof(text);
						break;
					case 3:
						temp.at(index).posZ = _ttof(text);
						break;
					case 4:
						temp.at(index).rotX = _ttof(text);
						break;
					case 5:
						temp.at(index).rotY = _ttof(text);
						break;
					case 6:
						temp.at(index).rotZ = _ttof(text);
						break;
					case 7:
						temp.at(index).scaX = _ttof(text);
						break;
					case 8:
						temp.at(index).scaY = _ttof(text);
						break;
					case 9:
						temp.at(index).scaZ = _ttof(text);
						break;
					}
				}
			}
		}



	//Update the toolMain scene graph
	*m_sceneGraph = temp;
	toolMain->UpdateDisplayList();
	UpdateInspector();
	toolMain->StoreBackup();
	}

}


void InspectorWindow::UpdateInspector()
{
	int index = -1;

	//Find the currently selected object
	for (int i = 0; i < m_sceneGraph->size(); i++)
	{
		if (m_sceneGraph->at(i).ID == *m_currentSelection)
		{
			index = i;
			break;
		}
	}

	//If the currently selected object is valid
	if (index != -1)
	{
		//Loop through and update the edit control boxes in the inspector window
		for (int j = 0; j < 10; j++)
		{
			CString text;
			switch (j)
			{
			case 0:
				text = m_sceneGraph->at(index).name.c_str();
				break;
			case 1:
				text.Format(_T("%.2f"), m_sceneGraph->at(index).posX);
				break;
			case 2:
				text.Format(_T("%.2f"), m_sceneGraph->at(index).posY);
				break;
			case 3:
				text.Format(_T("%.2f"), m_sceneGraph->at(index).posZ);
				break;
			case 4:
				text.Format(_T("%.2f"), m_sceneGraph->at(index).rotX);
				break;
			case 5:
				text.Format(_T("%.2f"), m_sceneGraph->at(index).rotY);
				break;
			case 6:
				text.Format(_T("%.2f"), m_sceneGraph->at(index).rotZ);
				break;
			case 7:
				text.Format(_T("%.2f"), m_sceneGraph->at(index).scaX);
				break;
			case 8:
				text.Format(_T("%.2f"), m_sceneGraph->at(index).scaY);
				break;
			case 9:
				text.Format(_T("%.2f"), m_sceneGraph->at(index).scaZ);
				break;
			}
						
			Transform[j].SetWindowTextW(text);
		}
	}
	else
	{
		for (int j = 0; j < 10; j++)
		{
			CString text;
			text = "";
			Transform[j].SetWindowTextW(text);
		}
	}

}

bool InspectorWindow::DoesCStringContain(CString str)
{
	CString characters("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ123456789!£$%^&*()_-<,>:;?/|\'@[{]}");

	if (str.FindOneOf(characters) != -1)
	{
		return true;
	}
	else
	{
		return false;
	}
}


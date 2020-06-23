#include "InspectorTest.h"

#include "stdafx.h"


IMPLEMENT_DYNAMIC(InspectorTest, CDialogBar)

//Message map.  Just like MFCMAIN.cpp.  This is where we catch button presses etc and point them to a handy dandy method.
BEGIN_MESSAGE_MAP(InspectorTest, CDialogBar)
	//ON_COMMAND(IDOK, &InspectorTest::End)					//ok button
	//ON_BN_CLICKED(IDOK, &InspectorTest::OnBnClickedOk)
	ON_BN_CLICKED(IDC_UPDATEOBJECT, &InspectorTest::UpdateGameObject)
END_MESSAGE_MAP()


InspectorTest::InspectorTest(CWnd* pParent, std::vector<SceneObject>* SceneGraph)		//constructor used in modal
	: CDialogBar()
{
	
	m_sceneGraph = SceneGraph;
}

InspectorTest::InspectorTest(CWnd* pParent)			//constructor used in modeless
	: CDialogBar()
{
	//Create(pParent, IDD_DOCKED_INSPECTOR, CBRS_ALIGN_RIGHT, IDD_DOCKED_INSPECTOR);

}

InspectorTest::~InspectorTest()
{
}

///pass through pointers to the data in the tool we want to manipulate
void InspectorTest::SetObjectData(std::vector<SceneObject>* SceneGraph, int* selection, ToolMain* tM, HWND* windowHandle)
{
	m_sceneGraph = SceneGraph;
	m_currentSelection = selection;
	toolMain = tM;
	hwnd = windowHandle;
}

void InspectorTest::Make(CWnd* papa)
{
	Create(papa, IDD_DOCKED_INSPECTOR, CBRS_ALIGN_RIGHT, IDD_DOCKED_INSPECTOR);
}

void InspectorTest::DoDataExchange(CDataExchange* pDX)
{
	CDialogBar::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_PosX, PosX);
	
	
	/*DDX_Control(pDX, IDC_PosY, PosY);
	DDX_Control(pDX, IDC_PosZ, PosZ);

	DDX_Control(pDX, IDC_RotX, RoteX);
	DDX_Control(pDX, IDC_RotY, RoteY);
	DDX_Control(pDX, IDC_RotZ, RoteZ);

	DDX_Control(pDX, IDC_ScaleX, ScaleX);
	DDX_Control(pDX, IDC_ScaleY, ScaleY);
	DDX_Control(pDX, IDC_ScaleZ, ScaleZ);

	DDX_Control(pDX, IDC_Name, Name);*/

}

void InspectorTest::End()
{
	DestroyWindow();	//destory the window properly.  INcluding the links and pointers created.  THis is so the dialogue can start again. 
}


void InspectorTest::PostNcDestroy()
{
}



void InspectorTest::UpdateInspector()
{
	int index = 0;

	//Find the currently selected object
	for (int i = 0; i < m_sceneGraph->size(); i++)
	{
		if (m_sceneGraph->at(i).ID == *m_currentSelection)
		{
			index = i;
			break;
		}
	}

	CString posXTxt;
	posXTxt.Format(_T("%.2f"), m_sceneGraph->at(index).posX);
	PosX.SetWindowTextW(posXTxt);

	CString posYTxt;
	posYTxt.Format(_T("%.2f"), m_sceneGraph->at(index).posY);
	PosY.SetWindowTextW(posYTxt);

	CString posZTxt;
	posZTxt.Format(_T("%.2f"), m_sceneGraph->at(index).posZ);
	PosZ.SetWindowTextW(posZTxt);

	CString rotXTxt;
	rotXTxt.Format(_T("%.2f"), m_sceneGraph->at(index).rotX);
	RoteX.SetWindowTextW(rotXTxt);

	CString rotYTxt;
	rotYTxt.Format(_T("%.2f"), m_sceneGraph->at(index).rotY);
	RoteY.SetWindowTextW(rotYTxt);

	CString rotZTxt;
	rotZTxt.Format(_T("%.2f"), m_sceneGraph->at(index).rotZ);
	RoteZ.SetWindowTextW(rotZTxt);

	CString scaXTxt;
	scaXTxt.Format(_T("%.2f"), m_sceneGraph->at(index).scaX);
	ScaleX.SetWindowTextW(scaXTxt);

	CString scaYTxt;
	scaYTxt.Format(_T("%.2f"), m_sceneGraph->at(index).scaY);
	ScaleY.SetWindowTextW(scaYTxt);

	CString scaZTxt;
	scaZTxt.Format(_T("%.2f"), m_sceneGraph->at(index).scaZ);
	ScaleZ.SetWindowTextW(scaZTxt);

	CString nameTxt(m_sceneGraph->at(index).name.c_str());
	Name.SetWindowTextW(nameTxt);

}

void InspectorTest::UpdateGameObject()
{
	//UpdateData();

	if (PosX)
	{
		CString PosXTxt;
		PosX.GetWindowTextW(PosXTxt);
	}
	//if (PosX)
	//{

	//}
	////Find the currently selected object index
	//std::vector<SceneObject> temp;
	//temp = *m_sceneGraph;
	//int index = 0;

	////Find the currently selected object
	//for (int i = 0; i < temp.size(); i++)
	//{
	//	if (temp.at(i).ID == *m_currentSelection)
	//	{
	//		index = i;
	//		break;
	//	}
	//}

	//CString PosXTxt;
	//CString PosYTxt;
	//CString PosZTxt;

	//if (PosX)
	//{
	//	PosX.GetWindowTextW(PosXTxt);
	//}

	//if (PosY)
	//{
	//	PosY.GetWindowTextW(PosYTxt);
	//}

	//if (PosZ)
	//{
	//	PosZ.GetWindowTextW(PosZTxt);
	//}


	//if (DoesCStringContain(PosXTxt))
	//{
	//	if (_ttof(PosXTxt))
	//	{
	//		temp.at(index).posX = _ttof(PosXTxt);
	//	}
	//}
	//else
	//{
	//	temp.at(index).posX = 0;
	//}

	//if (DoesCStringContain(PosYTxt))
	//{
	//	if (_ttof(PosYTxt))
	//	{
	//		temp.at(index).posY = _ttof(PosYTxt);
	//	}
	//}
	//else
	//{
	//	temp.at(index).posY = 0;
	//}


	//if (DoesCStringContain(PosZTxt))
	//{
	//	if (_ttof(PosZTxt))
	//	{
	//		temp.at(index).posZ = _ttof(PosZTxt);
	//	}
	//}
	//else
	//{
	//	temp.at(index).posZ = 0;
	//}


	//CString RotXTxt;
	//CString RotYTxt;
	//CString RotZTxt;

	//if (RoteX)
	//{
	//	RoteX.GetWindowTextW(RotXTxt);
	//}

	//if (RoteY)
	//{
	//	RoteY.GetWindowTextW(RotYTxt);
	//}

	//if (RoteZ)
	//{
	//	RoteZ.GetWindowTextW(RotZTxt);
	//}

	//int rX = temp.at(index).rotX;


	//if (DoesCStringContain(RotXTxt))
	//{
	//	if (_ttof(RotXTxt))
	//	{
	//		rX = _ttof(RotXTxt);

	//		temp.at(index).rotX = rX;
	//	}
	//}
	//else
	//{
	//	temp.at(index).rotX = 0;
	//}

	//if (DoesCStringContain(RotYTxt))
	//{
	//	if (_ttof(RotYTxt))
	//	{
	//		temp.at(index).rotY = _ttof(RotYTxt);
	//	}
	//}
	//else
	//{
	//	temp.at(index).rotY = 0;
	//}


	//if (DoesCStringContain(RotZTxt))
	//{
	//	if (_ttof(RotZTxt))
	//	{
	//		temp.at(index).rotZ = _ttof(RotZTxt);
	//	}
	//}
	//else
	//{
	//	temp.at(index).rotZ = 0;
	//}

	//CString ScaleXTxt;
	//CString ScaleYTxt;
	//CString ScaleZTxt;

	//if (ScaleX)
	//{
	//	ScaleX.GetWindowTextW(ScaleXTxt);
	//}

	//if (ScaleY)
	//{
	//	ScaleY.GetWindowTextW(ScaleYTxt);
	//}

	//if (ScaleZ)
	//{
	//	ScaleZ.GetWindowTextW(ScaleZTxt);
	//}


	//if (DoesCStringContain(ScaleXTxt))
	//{
	//	if (_ttof(ScaleXTxt))
	//	{
	//		temp.at(index).scaX = _ttof(ScaleXTxt);
	//	}
	//}
	//else
	//{
	//	temp.at(index).scaX = 0;
	//}

	//if (DoesCStringContain(ScaleYTxt))
	//{
	//	if (_ttof(ScaleYTxt))
	//	{
	//		temp.at(index).scaY = _ttof(ScaleYTxt);
	//	}
	//}
	//else
	//{
	//	temp.at(index).scaY = 0;
	//}


	//if (DoesCStringContain(ScaleZTxt))
	//{
	//	if (_ttof(ScaleXTxt))
	//	{
	//		temp.at(index).scaZ = _ttof(ScaleZTxt);
	//	}
	//}
	//else
	//{
	//	temp.at(index).scaZ = 0;
	//}

	//CString GNameTxt;

	//if (Name)
	//{
	//	Name.GetWindowTextW(GNameTxt);
	//}

	//std::string s(CW2A(GNameTxt.GetString()));

	//temp.at(index).name = s;

	////Update the toolMain scene graph
	//*m_sceneGraph = temp;
	//toolMain->UpdateDisplayList();
//	UpdateInspector();

}

bool InspectorTest::DoesCStringContain(CString str)
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

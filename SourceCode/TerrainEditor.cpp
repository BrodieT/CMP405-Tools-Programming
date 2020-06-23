#include "TerrainEditor.h"

#include "stdafx.h"


IMPLEMENT_DYNAMIC(TerrainEditor, CDialogEx)

//Message map.  Just like MFCMAIN.cpp.  This is where we catch button presses etc and point them to a handy dandy method.
BEGIN_MESSAGE_MAP(TerrainEditor, CDialogEx)
	//ON_BN_CLICKED(IDC_PositionConfirm, &TerrainEditor::SetSelectedObjPos)
	//ON_BN_CLICKED(IDC_MANIPTERRAIN, &TerrainEditor::ManipulateTerrain)
	ON_CBN_SELCHANGE(IDC_TOOLTYPESELECT, &TerrainEditor::UpdateTerrainData)
	ON_CBN_SELCHANGE(IDC_BRUSHTYPESELECT, &TerrainEditor::UpdateTerrainData)
	ON_EN_CHANGE(IDC_HEIGHTVAL, &TerrainEditor::UpdateTerrainData)
	ON_EN_CHANGE(IDC_BRUSHWIDTH, &TerrainEditor::UpdateTerrainData)
END_MESSAGE_MAP()


TerrainEditor::TerrainEditor(CWnd* pParent, std::vector<SceneObject>* SceneGraph)		//constructor used in modal
	: CDialogEx(IDD_TERRAIN_EDITOR, pParent)
{
	m_sceneGraph = SceneGraph;
}

TerrainEditor::TerrainEditor(CWnd* pParent)			//constructor used in modeless
	: CDialogEx(IDD_TERRAIN_EDITOR, pParent)
{
}

TerrainEditor::~TerrainEditor()
{
}

///pass through pointers to the data in the tool we want to manipulate
void TerrainEditor::SetObjectData(ToolMain* tM, HWND* windowHandle)
{
	isActive = true;
	m_toolMain = tM;
	hwnd = windowHandle;
}



void TerrainEditor::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_HEIGHTVAL, m_heightVal);
	DDX_Control(pDX, IDC_BRUSHWIDTH, m_brushWidth);
	DDX_Control(pDX, IDC_TOOLTYPESELECT, m_editModeSelection);
	DDX_Control(pDX, IDC_BRUSHTYPESELECT, m_brushSelection);


	//Initialise the contents of the brush type dropdown
	m_SquareBrush = "Square";
	m_brushSelection.AddString(m_SquareBrush);
	/*CircleBrush = "Circle";
	brushSelection.AddString(CircleBrush);*/

	m_brushSelection.SetCurSel(0);

	//Initialise the contents of the manipulation type dropdown
	m_FlatTxt = "Flatten Terrain";
	m_editModeSelection.AddString(m_FlatTxt);

	m_RaiseTxt = "Raise Terrain";
	m_editModeSelection.AddString(m_RaiseTxt);

	m_LowerTxt = "Lower Terrain";
	m_editModeSelection.AddString(m_LowerTxt);
	
	m_SmoothTxt = "Smooth Terrain";
	m_editModeSelection.AddString(m_SmoothTxt);
	
	m_editModeSelection.SetCurSel(0);

}


//This function extracts data from the editor dialogue's input fields
//and transfers them to the appropriate classes
void TerrainEditor::UpdateTerrainData()
{
	int Width = 0;
	int Type = 0;
	int Shape = 0;
	float Height = 0.0f;

	CString currentEditModeLabel;
	m_editModeSelection.GetLBText(m_editModeSelection.GetCurSel(), currentEditModeLabel);

	if (currentEditModeLabel == m_FlatTxt)
	{
		Type = 0;
	}

	if (currentEditModeLabel == m_RaiseTxt)
	{
		Type = 1;
	}

	if (currentEditModeLabel == m_LowerTxt)
	{
		Type = 2;
	}

	if (currentEditModeLabel == m_SmoothTxt)
	{
		Type = 3;
	}

	CString currentBrushShapeLabel;
	m_brushSelection.GetLBText(m_brushSelection.GetCurSel(), currentBrushShapeLabel);
	
	if (currentBrushShapeLabel == m_SquareBrush)
	{
		Shape = 0;
	}

	CString widthTxt;
	m_brushWidth.GetWindowTextW(widthTxt);

	if (DoesCStringContain(widthTxt))
	{
		if (_ttof(widthTxt))
		{
			Width = _ttof(widthTxt);
		}
	}
	
	CString heightTxt;
	m_heightVal.GetWindowTextW(heightTxt);

	if (DoesCStringContain(heightTxt))
	{
		if (_ttof(heightTxt))
		{
			Height = _ttof(heightTxt);
		}
	}
		
	
	m_toolMain->SetTerrainData(Type, Shape, Width, Height);
	//UpdateEditor();

}

void TerrainEditor::End()
{
	isActive = false;
	DestroyWindow();	//destory the window properly.  INcluding the links and pointers created.  THis is so the dialogue can start again. 
}


BOOL TerrainEditor::OnInitDialog()
{
	CDialogEx::OnInitDialog();


	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void TerrainEditor::PostNcDestroy()
{
}

//This function is used to update the contents of the terrain editor window
//to ensure the most up to date information is displayed
void TerrainEditor::UpdateEditor()
{
	
	CString w;
	w.Format(_T("%i"), m_toolMain->GetBrushData().x);
	m_brushWidth.SetWindowTextW(w);
	
	CString x;
	x.Format(_T("%i"), m_toolMain->GetTerrainHval());
	m_heightVal.SetWindowTextW(x);
	//editModeSelection.SetCurSel(0);
}

//This function is used when translating a string into a float to check if there are any characters init
//to prevent values being set to null or 0 when invalid charcters are inputted
bool TerrainEditor::DoesCStringContain(CString str)
{
	CString characters("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ123456789!£$%^&*()_-<,>:;?/|\'@[{]}.");

	if (str.FindOneOf(characters) != -1)
	{
		return true;
	}
	else
	{
		return false;
	}
}

//NB Unused
//Intended to be used if a generate terrain button was present for randmoising the terrain
void TerrainEditor::GenerateTerrain()
{
	m_toolMain->GenerateNewTerrain();
}

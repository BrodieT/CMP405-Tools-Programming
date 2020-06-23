#pragma once

#include <afxext.h>
#include "pch.h"
#include "Game.h"
#include "sqlite3.h"
#include "SceneObject.h"
#include "InputCommands.h"
#include <vector>


class ToolMain
{
public: //methods
	ToolMain();
	~ToolMain();

	//onAction - These are the interface to MFC
	int		getCurrentSelectionID();										//returns the selection number of currently selected object so that It can be displayed.
	void	onActionInitialise(HWND handle, int width, int height);			//Passes through handle and hieght and width and initialises DirectX renderer and SQL LITE
	void	onActionLoad();													//load the current chunk
	afx_msg	void	onActionSave();											//save the current chunk
	afx_msg void	onActionSaveTerrain();									//save chunk geometry

	void	Tick(MSG *msg);
	void	UpdateInput(MSG *msg);

	//Function to allow the building of the display list in Game using the scene graph from anywhere with access to toolMain
	void UpdateDisplayList() { m_d3dRenderer.BuildDisplayList(&m_sceneGraph); };

	//Stores the current scene data as a backup
	void StoreBackup();

	//Used for undo/redo processes
	void Undo();
	void Redo();


	//Used for object manipulation/creation
	void Instantiate();
	void RemoveObject();
	void DuplicateObject();
	
#
public:	//variables
	std::vector<SceneObject>    m_sceneGraph;	//our scenegraph storing all the objects in the current chunk
	ChunkObject					m_chunk;		//our landscape chunk
	int m_selectedObject = -1;						//ID of current Selection


	DirectX::XMFLOAT2 GetTerrainTarget();
	DirectX::XMFLOAT2 GetBrushData();
	void ToggleTerrainEdit() { m_d3dRenderer.m_TerrainEditModeActive = !m_d3dRenderer.m_TerrainEditModeActive; };
	bool GetTerrainEditMode() { return m_d3dRenderer.m_TerrainEditModeActive; };
	float GetTerrainHval() {  return m_d3dRenderer.GetTerrain()->m_heightValue; };
	void SetTerrainData(int editMode, int brushType, int brushWidth, int heightVal) { m_d3dRenderer.SetTerrainData(editMode, brushType, brushWidth, heightVal); };
	bool m_InspectorUpdate = false;
	void GenerateNewTerrain() { m_d3dRenderer.GetTerrain()->GenerateHeightmap(); };
	InputCommands m_toolInputCommands;		//input commands that we want to use and possibly pass over to the renderer

private:	//methods
	//Update the undo/redo vector
	void UpdateVectors();

private:	//variables
	HWND	m_toolHandle;		//Handle to the  window
	Game	m_d3dRenderer;		//Instance of D3D rendering system for our tool
	CRect	WindowRECT;		//Window area rectangle. 
	char	m_keyArray[256];
	sqlite3 *m_databaseConnection;	//sqldatabase handle

	int m_width;		//dimensions passed to directX
	int m_height;
	int m_currentChunk;			//the current chunk of thedatabase that we are operating on.  Dictates loading and saving. 
	
	//The size of the move zone at the edge of the windo for camera rotation with mouse
	float moveZone = 150.0f;
	//The current index in the backups vector
	int m_BackupsId = 0;
	struct SceneData
	{
		std::vector<SceneObject> sceneObjects;
		BYTE hMap[TERRAINRESOLUTION * TERRAINRESOLUTION];

		SceneData& operator=(const SceneData& rhs)
		{
			sceneObjects = rhs.sceneObjects;
			memcpy(hMap, rhs.hMap, sizeof(hMap));
			return *this;
		}
	};
	//Backup vector for storing data for undo/redo
	std::vector<SceneData> m_BackupSceneData;

	//bool Ctrl = false;
};

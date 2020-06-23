#include "ToolMain.h"
#include "resource.h"
#include <vector>
#include <sstream>


//ToolMain Class
ToolMain::ToolMain()
{

	m_currentChunk = 0;		//default value
	m_selectedObject = 0;	//initial selection ID
	m_sceneGraph.clear();	//clear the vector for the scenegraph
	m_databaseConnection = NULL;

	//Init input commands
	m_toolInputCommands.forward		= false;
	m_toolInputCommands.back		= false;
	m_toolInputCommands.left		= false;
	m_toolInputCommands.right		= false;
	m_toolInputCommands.moveUp		= false;
	m_toolInputCommands.moveDown	= false;

	m_toolInputCommands.mouse_LB_Down = false;
	m_toolInputCommands.mouse_RB_Down = false;
	m_toolInputCommands.mouse_X = 0;
	m_toolInputCommands.mouse_Y = 0;
}


ToolMain::~ToolMain()
{
	sqlite3_close(m_databaseConnection);		//close the database connection
}

//This function returns the currently selected object ID
int ToolMain::getCurrentSelectionID()
{
	return m_selectedObject;
}

void ToolMain::onActionInitialise(HWND handle, int width, int height)
{

	//window size, handle etc for directX
	m_width		= width;
	m_height	= height;
	
	
	GetClientRect(handle, &WindowRECT);
	
	m_d3dRenderer.Initialize(handle, m_width, m_height);

	//database connection establish
	int rc;
	rc = sqlite3_open_v2("database/test.db",&m_databaseConnection, SQLITE_OPEN_READWRITE, NULL);

	if (rc) 
	{
		TRACE("Can't open database");
		//if the database cant open. Perhaps a more catastrophic error would be better here
	}
	else 
	{
		TRACE("Opened database successfully");
	}

	onActionLoad();

	//Init the backups data for undo/redo
	m_BackupsId = 0;
	SceneData sD;
	sD.sceneObjects = m_sceneGraph;
	memcpy(sD.hMap, m_d3dRenderer.GetTerrain()->m_heightMap, sizeof(sD.hMap));
	m_BackupSceneData.push_back(sD);
}

void ToolMain::onActionLoad()
{
	//load current chunk and objects into lists
	if (!m_sceneGraph.empty())		//is the vector empty
	{
		m_sceneGraph.clear();		//if not, empty it
	}

	//SQL
	int rc;
	char *sqlCommand;
	char *ErrMSG = 0;
	sqlite3_stmt *pResults;								//results of the query
	sqlite3_stmt *pResultsChunk;

	//OBJECTS IN THE WORLD
	//prepare SQL Text
	sqlCommand = "SELECT * from Objects";				//sql command which will return all records from the objects table.
	//Send Command and fill result object
	rc = sqlite3_prepare_v2(m_databaseConnection, sqlCommand, -1, &pResults, 0 );
	
	//loop for each row in results until there are no more rows.  ie for every row in the results. We create and object
	while (sqlite3_step(pResults) == SQLITE_ROW)
	{	
		SceneObject newSceneObject;
		newSceneObject.ID = sqlite3_column_int(pResults, 0);
		newSceneObject.chunk_ID = sqlite3_column_int(pResults, 1);
		newSceneObject.model_path		= reinterpret_cast<const char*>(sqlite3_column_text(pResults, 2));
		newSceneObject.tex_diffuse_path = reinterpret_cast<const char*>(sqlite3_column_text(pResults, 3));
		newSceneObject.posX = sqlite3_column_double(pResults, 4);
		newSceneObject.posY = sqlite3_column_double(pResults, 5);
		newSceneObject.posZ = sqlite3_column_double(pResults, 6);
		newSceneObject.rotX = sqlite3_column_double(pResults, 7);
		newSceneObject.rotY = sqlite3_column_double(pResults, 8);
		newSceneObject.rotZ = sqlite3_column_double(pResults, 9);
		newSceneObject.scaX = sqlite3_column_double(pResults, 10);
		newSceneObject.scaY = sqlite3_column_double(pResults, 11);
		newSceneObject.scaZ = sqlite3_column_double(pResults, 12);
		newSceneObject.render = sqlite3_column_int(pResults, 13);
		newSceneObject.collision = sqlite3_column_int(pResults, 14);
		newSceneObject.collision_mesh = reinterpret_cast<const char*>(sqlite3_column_text(pResults, 15));
		newSceneObject.collectable = sqlite3_column_int(pResults, 16);
		newSceneObject.destructable = sqlite3_column_int(pResults, 17);
		newSceneObject.health_amount = sqlite3_column_int(pResults, 18);
		newSceneObject.editor_render = sqlite3_column_int(pResults, 19);
		newSceneObject.editor_texture_vis = sqlite3_column_int(pResults, 20);
		newSceneObject.editor_normals_vis = sqlite3_column_int(pResults, 21);
		newSceneObject.editor_collision_vis = sqlite3_column_int(pResults, 22);
		newSceneObject.editor_pivot_vis = sqlite3_column_int(pResults, 23);
		newSceneObject.pivotX = sqlite3_column_double(pResults, 24);
		newSceneObject.pivotY = sqlite3_column_double(pResults, 25);
		newSceneObject.pivotZ = sqlite3_column_double(pResults, 26);
		newSceneObject.snapToGround = sqlite3_column_int(pResults, 27);
		newSceneObject.AINode = sqlite3_column_int(pResults, 28);
		newSceneObject.audio_path = reinterpret_cast<const char*>(sqlite3_column_text(pResults, 29));
		newSceneObject.volume = sqlite3_column_double(pResults, 30);
		newSceneObject.pitch = sqlite3_column_double(pResults, 31);
		newSceneObject.pan = sqlite3_column_int(pResults, 32);
		newSceneObject.one_shot = sqlite3_column_int(pResults, 33);
		newSceneObject.play_on_init = sqlite3_column_int(pResults, 34);
		newSceneObject.play_in_editor = sqlite3_column_int(pResults, 35);
		newSceneObject.min_dist = sqlite3_column_double(pResults, 36);
		newSceneObject.max_dist = sqlite3_column_double(pResults, 37);
		newSceneObject.camera = sqlite3_column_int(pResults, 38);
		newSceneObject.path_node = sqlite3_column_int(pResults, 39);
		newSceneObject.path_node_start = sqlite3_column_int(pResults, 40);
		newSceneObject.path_node_end = sqlite3_column_int(pResults, 41);
		newSceneObject.parent_id = sqlite3_column_int(pResults, 42);
		newSceneObject.editor_wireframe = sqlite3_column_int(pResults, 43);
		newSceneObject.name = reinterpret_cast<const char*>(sqlite3_column_text(pResults, 44));

		newSceneObject.light_type = sqlite3_column_int(pResults, 45);
		newSceneObject.light_diffuse_r = sqlite3_column_double(pResults, 46);
		newSceneObject.light_diffuse_g = sqlite3_column_double(pResults, 47);
		newSceneObject.light_diffuse_b = sqlite3_column_double(pResults, 48);
		newSceneObject.light_specular_r = sqlite3_column_double(pResults, 49);
		newSceneObject.light_specular_g = sqlite3_column_double(pResults, 50);
		newSceneObject.light_specular_b = sqlite3_column_double(pResults, 51);
		newSceneObject.light_spot_cutoff = sqlite3_column_double(pResults, 52);
		newSceneObject.light_constant = sqlite3_column_double(pResults, 53);
		newSceneObject.light_linear = sqlite3_column_double(pResults, 54);
		newSceneObject.light_quadratic = sqlite3_column_double(pResults, 55);
	

		//send completed object to scenegraph
		m_sceneGraph.push_back(newSceneObject);
	}

	//THE WORLD CHUNK
	//prepare SQL Text
	sqlCommand = "SELECT * from Chunks";				//sql command which will return all records from  chunks table. There is only one tho.
														//Send Command and fill result object
	rc = sqlite3_prepare_v2(m_databaseConnection, sqlCommand, -1, &pResultsChunk, 0);


	sqlite3_step(pResultsChunk);
	m_chunk.ID = sqlite3_column_int(pResultsChunk, 0);
	m_chunk.name = reinterpret_cast<const char*>(sqlite3_column_text(pResultsChunk, 1));
	m_chunk.chunk_x_size_metres = sqlite3_column_int(pResultsChunk, 2);
	m_chunk.chunk_y_size_metres = sqlite3_column_int(pResultsChunk, 3);
	m_chunk.chunk_base_resolution = sqlite3_column_int(pResultsChunk, 4);
	m_chunk.heightmap_path = reinterpret_cast<const char*>(sqlite3_column_text(pResultsChunk, 5));
	m_chunk.tex_diffuse_path = reinterpret_cast<const char*>(sqlite3_column_text(pResultsChunk, 6));
	m_chunk.tex_splat_alpha_path = reinterpret_cast<const char*>(sqlite3_column_text(pResultsChunk, 7));
	m_chunk.tex_splat_1_path = reinterpret_cast<const char*>(sqlite3_column_text(pResultsChunk, 8));
	m_chunk.tex_splat_2_path = reinterpret_cast<const char*>(sqlite3_column_text(pResultsChunk, 9));
	m_chunk.tex_splat_3_path = reinterpret_cast<const char*>(sqlite3_column_text(pResultsChunk, 10));
	m_chunk.tex_splat_4_path = reinterpret_cast<const char*>(sqlite3_column_text(pResultsChunk, 11));
	m_chunk.render_wireframe = sqlite3_column_int(pResultsChunk, 12);
	m_chunk.render_normals = sqlite3_column_int(pResultsChunk, 13);
	m_chunk.tex_diffuse_tiling = sqlite3_column_int(pResultsChunk, 14);
	m_chunk.tex_splat_1_tiling = sqlite3_column_int(pResultsChunk, 15);
	m_chunk.tex_splat_2_tiling = sqlite3_column_int(pResultsChunk, 16);
	m_chunk.tex_splat_3_tiling = sqlite3_column_int(pResultsChunk, 17);
	m_chunk.tex_splat_4_tiling = sqlite3_column_int(pResultsChunk, 18);


	//Process REsults into renderable
	m_d3dRenderer.BuildDisplayList(&m_sceneGraph);
	//build the renderable chunk 
	m_d3dRenderer.BuildDisplayChunk(&m_chunk);

}

void ToolMain::onActionSave()
{
	//SQL
	int rc;
	char *sqlCommand;
	char *ErrMSG = 0;
	sqlite3_stmt *pResults;								//results of the query
	

	//OBJECTS IN THE WORLD Delete them all
	//prepare SQL Text
	sqlCommand = "DELETE FROM Objects";	 //will delete the whole object table.   Slightly risky but hey.
	rc = sqlite3_prepare_v2(m_databaseConnection, sqlCommand, -1, &pResults, 0);
	sqlite3_step(pResults);

	//Populate with our new objects
	std::wstring sqlCommand2;
	int numObjects = m_sceneGraph.size();	//Loop thru the scengraph.

	for (int i = 0; i < numObjects; i++)
	{
		std::stringstream command;
		command << "INSERT INTO Objects " 
			<<"VALUES(" << m_sceneGraph.at(i).ID << ","
			<< m_sceneGraph.at(i).chunk_ID  << ","
			<< "'" << m_sceneGraph.at(i).model_path <<"'" << ","
			<< "'" << m_sceneGraph.at(i).tex_diffuse_path << "'" << ","
			<< m_sceneGraph.at(i).posX << ","
			<< m_sceneGraph.at(i).posY << ","
			<< m_sceneGraph.at(i).posZ << ","
			<< m_sceneGraph.at(i).rotX << ","
			<< m_sceneGraph.at(i).rotY << ","
			<< m_sceneGraph.at(i).rotZ << ","
			<< m_sceneGraph.at(i).scaX << ","
			<< m_sceneGraph.at(i).scaY << ","
			<< m_sceneGraph.at(i).scaZ << ","
			<< m_sceneGraph.at(i).render << ","
			<< m_sceneGraph.at(i).collision << ","
			<< "'" << m_sceneGraph.at(i).collision_mesh << "'" << ","
			<< m_sceneGraph.at(i).collectable << ","
			<< m_sceneGraph.at(i).destructable << ","
			<< m_sceneGraph.at(i).health_amount << ","
			<< m_sceneGraph.at(i).editor_render << ","
			<< m_sceneGraph.at(i).editor_texture_vis << ","
			<< m_sceneGraph.at(i).editor_normals_vis << ","
			<< m_sceneGraph.at(i).editor_collision_vis << ","
			<< m_sceneGraph.at(i).editor_pivot_vis << ","
			<< m_sceneGraph.at(i).pivotX << ","
<< m_sceneGraph.at(i).pivotY << ","
<< m_sceneGraph.at(i).pivotZ << ","
<< m_sceneGraph.at(i).snapToGround << ","
<< m_sceneGraph.at(i).AINode << ","
<< "'" << m_sceneGraph.at(i).audio_path << "'" << ","
<< m_sceneGraph.at(i).volume << ","
<< m_sceneGraph.at(i).pitch << ","
<< m_sceneGraph.at(i).pan << ","
<< m_sceneGraph.at(i).one_shot << ","
<< m_sceneGraph.at(i).play_on_init << ","
<< m_sceneGraph.at(i).play_in_editor << ","
<< m_sceneGraph.at(i).min_dist << ","
<< m_sceneGraph.at(i).max_dist << ","
<< m_sceneGraph.at(i).camera << ","
<< m_sceneGraph.at(i).path_node << ","
<< m_sceneGraph.at(i).path_node_start << ","
<< m_sceneGraph.at(i).path_node_end << ","
<< m_sceneGraph.at(i).parent_id << ","
<< m_sceneGraph.at(i).editor_wireframe << ","
<< "'" << m_sceneGraph.at(i).name << "'" << ","

<< m_sceneGraph.at(i).light_type << ","
<< m_sceneGraph.at(i).light_diffuse_r << ","
<< m_sceneGraph.at(i).light_diffuse_g << ","
<< m_sceneGraph.at(i).light_diffuse_b << ","
<< m_sceneGraph.at(i).light_specular_r << ","
<< m_sceneGraph.at(i).light_specular_g << ","
<< m_sceneGraph.at(i).light_specular_b << ","
<< m_sceneGraph.at(i).light_spot_cutoff << ","
<< m_sceneGraph.at(i).light_constant << ","
<< m_sceneGraph.at(i).light_linear << ","
<< m_sceneGraph.at(i).light_quadratic

<< ")";
std::string sqlCommand2 = command.str();
rc = sqlite3_prepare_v2(m_databaseConnection, sqlCommand2.c_str(), -1, &pResults, 0);
sqlite3_step(pResults);
	}
	MessageBox(NULL, L"Objects Saved", L"Notification", MB_OK);
}

void ToolMain::onActionSaveTerrain()
{
	m_d3dRenderer.SaveDisplayChunk(&m_chunk);
}

void ToolMain::Tick(MSG *msg)
{

	//Perform mouse picking if RMB is pressed
	if (m_toolInputCommands.mouse_LB_Down)
	{
		//Store the previous ID so that if an invalid value is returned from the picking function
		//no change is made
		int m = m_selectedObject;
		
		//Call the picking function
		m_selectedObject = m_d3dRenderer.MousePicking();

		if (m_selectedObject == NULL || m_selectedObject == -1)
		{
			m_selectedObject = m;
		}
		
		//If a new object is selected update the inspector
		if (m != m_selectedObject)
		{
			m_InspectorUpdate = true;
		}

		//If a change is made to the terrain store a backup
		if (m_d3dRenderer.GetTerrain()->m_TerrainEditMade)
		{
			StoreBackup();
			m_d3dRenderer.GetTerrain()->m_TerrainEditMade = false;
		}

		//Set the mouse button to false
		m_toolInputCommands.mouse_LB_Down = false;
	}

	//Renderer Update Call
	m_d3dRenderer.Tick(&m_toolInputCommands);
}

//Returns the brush data for updating the editor dialogue elsewhere
DirectX::XMFLOAT2 ToolMain::GetBrushData()
{
	return DirectX::XMFLOAT2(m_d3dRenderer.GetTerrain()->m_currentBrush.width, m_d3dRenderer.GetTerrain()->m_currentBrush.shape);
}


void ToolMain::UpdateInput(MSG * msg)
{

	switch (msg->message)
	{
		//Global inputs,  mouse position and keys etc
	case WM_KEYDOWN:
		m_keyArray[msg->wParam] = true;		
		break;

	case WM_KEYUP:
		m_keyArray[msg->wParam] = false;
		break;

	case WM_MOUSEMOVE:
		//Store the previous mouse position, can be used for detecting click-and-drags
		m_toolInputCommands.mouse_Prev_X = m_toolInputCommands.mouse_X;
		m_toolInputCommands.mouse_Prev_Y = m_toolInputCommands.mouse_Y;

		//Update the mouse position in input commands with the current position on screen
		m_toolInputCommands.mouse_X = GET_X_LPARAM(msg->lParam);
		m_toolInputCommands.mouse_Y = GET_Y_LPARAM(msg->lParam);

		//Ensure the mouse is within the bounds of the screen
		if (m_toolInputCommands.mouse_X >= WindowRECT.right ||
			m_toolInputCommands.mouse_X <= WindowRECT.left ||
			m_toolInputCommands.mouse_Y <= WindowRECT.top ||
			m_toolInputCommands.mouse_Y >= WindowRECT.bottom)
		{
			m_toolInputCommands.mouse_RB_Down = false;
		}

		//Check the mouse position to determine if it lies within the move zone at the left or right of the window for rotation
		if (m_toolInputCommands.mouse_X > (WindowRECT.right - moveZone) && m_toolInputCommands.mouse_X < WindowRECT.right)
		{
			m_toolInputCommands.rotRight = true;
		}
		else
		{
			m_toolInputCommands.rotRight = false;
		}

		if (m_toolInputCommands.mouse_X < (WindowRECT.left + moveZone) && m_toolInputCommands.mouse_X > WindowRECT.left)
		{
			m_toolInputCommands.rotLeft = true;
		}
		else
		{
			m_toolInputCommands.rotLeft = false;
		}

		//Check the mouse position to determine if it lies within the move zone at the top or bottom of the window for rotation
		if (m_toolInputCommands.mouse_Y >(WindowRECT.bottom - moveZone) && m_toolInputCommands.mouse_Y < WindowRECT.bottom)
		{
			m_toolInputCommands.rotDown = true;
		}
		else
		{
			m_toolInputCommands.rotDown = false;
		}

		if (m_toolInputCommands.mouse_Y < (WindowRECT.top + moveZone) && m_toolInputCommands.mouse_Y > WindowRECT.top)
		{
			m_toolInputCommands.rotUp = true;
		}
		else
		{
			m_toolInputCommands.rotUp = false;
		}
		break;

	case WM_LBUTTONDOWN:
		//mouse button down,  you will probably need to check when its up too
		//set some flag for the mouse button in inputcommands
		m_toolInputCommands.mouse_LB_Down = true;
		break;
	case WM_LBUTTONUP:
		m_toolInputCommands.mouse_LB_Down = false;
		break;
	case WM_RBUTTONDOWN:
		m_toolInputCommands.mouse_RB_Down = true;
		break;
	case WM_RBUTTONUP:
		m_toolInputCommands.mouse_RB_Down = false;
		break;
	}

	//WASD movement for the camera
	if (m_keyArray['W'] && !GetAsyncKeyState(VK_CONTROL))
	{
		m_toolInputCommands.forward = true;
	}
	else m_toolInputCommands.forward = false;
	
	if (m_keyArray['S'] && !GetAsyncKeyState(VK_CONTROL))
	{
		m_toolInputCommands.back = true;
	}
	else m_toolInputCommands.back = false;
	if (m_keyArray['A'] && !GetAsyncKeyState(VK_CONTROL))
	{
		m_toolInputCommands.left = true;
	}
	else m_toolInputCommands.left = false;

	if (m_keyArray['D'] && !GetAsyncKeyState(VK_CONTROL))
	{
		m_toolInputCommands.right = true;
	}
	else m_toolInputCommands.right = false;

	if (m_keyArray['E'] && !GetAsyncKeyState(VK_CONTROL))
	{
		m_toolInputCommands.moveUp = true;
	}
	else m_toolInputCommands.moveUp = false;

	if (m_keyArray['Q'] && !GetAsyncKeyState(VK_CONTROL))
	{
		m_toolInputCommands.moveDown = true;
	}
	else m_toolInputCommands.moveDown = false;

#pragma region KeyboardShortcuts
	//Create a new object
	if (GetAsyncKeyState(VK_CONTROL) && m_keyArray['A'])
	{
		m_keyArray['A'] = false;
		Instantiate();
	}

	//Delete the currently selected object
	if (GetAsyncKeyState(VK_CONTROL) && m_keyArray['X'])
	{
		m_keyArray['X'] = false;
		RemoveObject();
	}

	//Duplicate the currently selected object
	if (GetAsyncKeyState(VK_CONTROL) && m_keyArray['D'])
	{
		m_keyArray['D'] = false;
		DuplicateObject();
	}

	//Undo Shortcut
	if (GetAsyncKeyState(VK_CONTROL) && m_keyArray['Z'])
	{
		m_keyArray['Z'] = false;
		Undo();
	}

	//Undo Shortcut
	if (GetAsyncKeyState(VK_CONTROL) && m_keyArray['Y'])
	{
		m_keyArray['Y'] = false;
		Redo();
	}

	//Focus the camera on the currently selected object
	if (GetAsyncKeyState(VK_CONTROL) && m_keyArray['F'])
	{
		m_keyArray['F'] = false;
		int targetIndex = 0;
		//Find the currently selected object
		for (int i = 0; i < m_sceneGraph.size(); i++)
		{
			if (m_sceneGraph.at(i).ID == m_selectedObject)
			{
				targetIndex = i;
			}
		}
		m_d3dRenderer.m_camera.FocusCamera(DirectX::XMFLOAT3(m_sceneGraph[targetIndex].posX, m_sceneGraph[targetIndex].posY, m_sceneGraph[targetIndex].posZ),
								DirectX::XMFLOAT3(m_sceneGraph[targetIndex].scaX/2, m_sceneGraph[targetIndex].scaY/2, m_sceneGraph[targetIndex].scaZ/2));
	}

	//Save Shortcut
	if (GetAsyncKeyState(VK_CONTROL) && m_keyArray['S'])
	{
		m_keyArray['S'] = false;
		onActionSave();
		onActionSaveTerrain();
	}
#pragma endregion

}


//This function will duplicate the currently selected object and place the duplicate 5 units above the original
void ToolMain::DuplicateObject()
{
	//Create a new empty scene object for the duplicate
	SceneObject newObject;
	int newID = 0;
	int targetIndex = 0;

	//Find the currently selected object
	for (int i = 0; i < m_sceneGraph.size(); i++)
	{
		if (m_sceneGraph.at(i).ID == m_selectedObject)
		{
			targetIndex = i;
		}
		//Find the largest index currently in the scene
		newID = max(newID, m_sceneGraph[i].ID);
	}

	//Assign the new object with the values from the selected one
	newObject = m_sceneGraph.at(targetIndex);

	//Add 1 to the max ID found
	newObject.ID = newID + 1;
	newObject.posY += 5;

	m_sceneGraph.push_back(newObject);


	m_selectedObject = newObject.ID;
	m_d3dRenderer.BuildDisplayList(&m_sceneGraph);
	StoreBackup();

}

//This function will remove the selected object from the scene
void ToolMain::RemoveObject()
{
	if (m_selectedObject != -1)
	{
		//Create a new vector scene graph without the currently selected object
		std::vector<SceneObject> newScenGraph;
		for (int i = 0; i < m_sceneGraph.size(); i++)
		{
			if (m_sceneGraph.at(i).ID != m_selectedObject)
			{
				newScenGraph.push_back(m_sceneGraph.at(i));
			}
		}
		//Overwrite scene graph with new vector with object removed
		m_sceneGraph.clear();
		m_sceneGraph = newScenGraph;

		//Set selected object to -1
		m_selectedObject = -1;

		//Rebuild display list
		m_d3dRenderer.BuildDisplayList(&m_sceneGraph);
		StoreBackup();

	//	MessageBox(NULL, L"Object Removed", L"Notification", MB_OK);
	}
	else
	{
		MessageBox(NULL, L"Unable to remove object. No object selected.", L"Error", MB_OK);
	}
}

//This function will create a new scene object
void ToolMain::Instantiate()
{
	//Create an empty scene object
	SceneObject newObject;
	sqlite3_stmt* pResults;								//results of the query
	int rc;

	int newID = 0;
	for (int i = 0; i < m_sceneGraph.size(); i++)
	{
		newID = max(newID, m_sceneGraph[i].ID);
	}



	newObject.ID = newID + 1;
	newObject.chunk_ID = 0;
	newObject.model_path = "database/data/placeholder.cmo";
	newObject.tex_diffuse_path = "database/data/placeholder.dds";
	newObject.posX = 0.0f;
	newObject.posY = 0.0f;
	newObject.posZ = 0.0f;
	newObject.rotX = 0.0f;
	newObject.rotY = 0.0f;
	newObject.rotZ = 0.0f;
	newObject.scaX = 1;
	newObject.scaY = 1;
	newObject.scaZ = 1;
	newObject.render = false;
	newObject.collision = false;
	newObject.collision_mesh = "";
	newObject.collectable = false;
	newObject.destructable = false;
	newObject.health_amount = 0;
	newObject.editor_render = true;
	newObject.editor_texture_vis = true;
	newObject.editor_normals_vis = false;
	newObject.editor_collision_vis = false;
	newObject.editor_pivot_vis = false;
	newObject.pivotX = 0.0f;
	newObject.pivotY = 0.0f;
	newObject.pivotZ = 0.0f;
	newObject.snapToGround = false;
	newObject.AINode = false;
	newObject.audio_path = "";
	newObject.volume = 0.0f;
	newObject.pitch = 0.0f;
	newObject.pan = 0.0f;
	newObject.one_shot = false;
	newObject.play_on_init = false;
	newObject.play_in_editor = false;
	newObject.min_dist = 0.0;
	newObject.max_dist = 0.0;
	newObject.camera = false;
	newObject.path_node = false;
	newObject.path_node_start = false;
	newObject.path_node_end = false;
	newObject.parent_id = 0;
	newObject.editor_wireframe = false;
	newObject.name = "New GameObject";
	newObject.light_type = 1;
	newObject.light_diffuse_r = 2.0f;
	newObject.light_diffuse_g = 3.0f;
	newObject.light_diffuse_b = 4.0f;
	newObject.light_specular_r = 5.0f;
	newObject.light_specular_g = 6.0f;
	newObject.light_specular_b = 7.0f;
	newObject.light_spot_cutoff = 8.0f;
	newObject.light_constant = 9.0f;
	newObject.light_linear = 0.0f;
	newObject.light_quadratic = 1.0f;

	m_sceneGraph.push_back(newObject);
	m_selectedObject = newObject.ID;

	//FOCUS CAMERA


	m_d3dRenderer.BuildDisplayList(&m_sceneGraph);

	StoreBackup();

	//MessageBox(NULL, L"Object Instantiated", L"Notification", MB_OK);

}


//This function updates the backupData vector where appropriate
void ToolMain::UpdateVectors()
{
	//If an action is being performed and something has been undone
	//Remove all entries after the current index
	if (m_BackupsId != m_BackupSceneData.size() - 1)
	{
		std::vector<SceneData> temp;

		for (int i = 0; i < m_BackupsId; i++)
		{
			temp.push_back(m_BackupSceneData[i]);
		}
		m_BackupSceneData.clear();

	
		m_BackupSceneData = temp;
	}
	else if (m_BackupSceneData.size() > 10) //Otherwise if the size of the vector exceeds the limit, remove all prior entires
	{
		std::vector<SceneData> temp;

		for (int i = m_BackupSceneData.size() - 10; i < m_BackupSceneData.size(); i++)
		{
			temp.push_back(m_BackupSceneData[i]);
		}
		m_BackupSceneData.clear();
		m_BackupSceneData = temp;
		m_BackupsId = m_BackupSceneData.size() - 1;
	}
}

//This function updates the backup vector and increments the id
void ToolMain::StoreBackup()
{
	UpdateVectors();
	m_BackupsId++;

	SceneData sD;
	sD.sceneObjects = m_sceneGraph;
	memcpy(sD.hMap, m_d3dRenderer.GetTerrain()->m_heightMap, sizeof(sD.hMap));
	m_BackupSceneData.push_back(sD);
}

//This function undoes a previous action
void ToolMain::Undo()
{
	if (m_BackupsId > 0)
	{
		m_BackupsId--;
		m_sceneGraph.clear();
		m_sceneGraph = m_BackupSceneData[m_BackupsId].sceneObjects;
		memcpy(m_d3dRenderer.GetTerrain()->m_heightMap, m_BackupSceneData[m_BackupsId].hMap, sizeof(m_d3dRenderer.GetTerrain()->m_heightMap));
		UpdateDisplayList();
		m_d3dRenderer.GetTerrain()->UpdateTerrain();
	}
	else
	{
		//No changes to revert
	}

	
}

//This function redoes an undoe action
void ToolMain::Redo()
{
	if (m_BackupsId < m_BackupSceneData.size() - 1)
	{
		m_BackupsId++;
		m_sceneGraph.clear();
		m_sceneGraph = m_BackupSceneData[m_BackupsId].sceneObjects;
		memcpy(m_d3dRenderer.GetTerrain()->m_heightMap, m_BackupSceneData[m_BackupsId].hMap, sizeof(m_d3dRenderer.GetTerrain()->m_heightMap));
		UpdateDisplayList();
		m_d3dRenderer.GetTerrain()->UpdateTerrain();

	}
	else
	{
		//MessageBox(NULL, L"No changes to redo.", L"Error", MB_OK);
	}
}
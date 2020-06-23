#include <string>
#include "DisplayChunk.h"
#include "Game.h"


using namespace DirectX;
using namespace DirectX::SimpleMath;

DisplayChunk::DisplayChunk()
{
	//terrain size in meters. note that this is hard coded here, we COULD get it from the terrain chunk along with the other info from the tool if we want to be more flexible.
	m_terrainSize = 512;
	m_terrainHeightScale = 0.25;  //convert our 0-256 terrain to 64
	m_textureCoordStep = 1.0 / (TERRAINRESOLUTION-1);	//-1 becuase its split into chunks. not vertices.  we want tthe last one in each row to have tex coord 1
	m_terrainPositionScalingFactor = m_terrainSize / (TERRAINRESOLUTION-1);
}

DisplayChunk::~DisplayChunk()
{
}

//This function uses the ray data from Game to detect for a ray-triangle intersect throughout the terrain
void DisplayChunk::PickTerrain(DirectX::XMFLOAT3 A, DirectX::XMFLOAT3 B)
{
	std::vector<XMINT2> vertsOnLine;

	//Loop through the terrain
	for (int i = 0; i < TERRAINRESOLUTION - 1; i++)
	{
		for (int j = 0; j < TERRAINRESOLUTION - 1; j++)
		{
			//Store the surrounding vertices that make up the current quad
			Vector3 BL = m_terrainGeometry[i][j].position;
			Vector3 BR = m_terrainGeometry[i+1][j].position;
			Vector3 TL = m_terrainGeometry[i][j+ 1].position;
			Vector3 TR = m_terrainGeometry[i+ 1][j+1].position;

			//Check for an intersection with both triangles
			//And add the vertices that make up that triangle to the list 

			if (RayTriangleIntersect(A, B,BL, TR, TL))
			{
				vertsOnLine.push_back(XMINT2(i, j));
				vertsOnLine.push_back(XMINT2(i, j + 1));
				vertsOnLine.push_back(XMINT2(i+1, j+1));
			}
			
			if (RayTriangleIntersect(A, B, BL, BR, TR))
			{
				vertsOnLine.push_back(XMINT2(i, j));
				vertsOnLine.push_back(XMINT2(i + 1, j+1));
				vertsOnLine.push_back(XMINT2(i + 1, j));
			}
			
		}
	}

	//If the terrain was intersected by the ray
	if (vertsOnLine.size() > 0)
	{
		XMINT2 index = XMINT2(0,0);
		Vector3 origin = A;
		float dist = 10000000;
		//Loop through intersected indices and find the closest one
		for (int x = 0; x < vertsOnLine.size(); x++)
		{
			dist = std::min( dist, origin.Distance(origin, m_terrainGeometry[vertsOnLine[x].x][vertsOnLine[x].y].position));

			if (dist == origin.Distance(origin, m_terrainGeometry[vertsOnLine[x].x][vertsOnLine[x].y].position))
			{
				index = vertsOnLine[x];
			}
		}

		//Set the selected vertex to the closest index
		m_SelectedVertX = index.x;
		m_SelectedVertY = index.y;
		
		//Apply the selected technique to the terrain
		switch (m_selectedTechnique)
		{
		case 0:
			FlattenTerrain(m_heightValue);
			m_TerrainEditMade = true;
			break;
		case 1:
			RaiseOrLowerTerrain(true);
			m_TerrainEditMade = true;
			break;
		case 2:
			RaiseOrLowerTerrain(false);
			m_TerrainEditMade = true;
			break;
		case 3:
			SmoothTerrain();
			m_TerrainEditMade = true;
			break;
		default:
			MessageBox(NULL, L"Invalid Terrain Manipulation Technique Selected", L"Error", MB_OK);
			break;
		}
		UpdateHeightMap();
	}
}

//Function unused
//Intended to calculate indices for a circle brush
std::vector<XMINT2> DisplayChunk::InitBrushIndices()
{	
	int mapSize = TERRAINRESOLUTION;

	const int radius = 4;
	std::vector<XMINT2> erosionBrushIndices;
	std::vector<float> erosionBrushWeights[TERRAINRESOLUTION * TERRAINRESOLUTION];

	int xOffsets[radius * radius * 4];
	int yOffsets[radius * radius * 4];
	float weights[radius * radius * 4];

	int addIndex = 0;
	float weightSum = 0;
	for (int i = 0; i < TERRAINRESOLUTION * TERRAINRESOLUTION; i++)
	{
		int centreX = i % mapSize;
		int centreY = i / mapSize;

		if (centreY <= radius || centreY >= mapSize - radius || centreX <= radius - 1 || centreX >= mapSize - radius)
		{
			weightSum = 0;
			addIndex = 0;
			for (int y = -radius; y <= radius; y++)
			{
				for (int x = -radius; x <= radius; x++)
				{
					float sqrDist = x * x + y * y;
					if (sqrDist < radius * radius)
					{
						int coordX = centreX + x;
						int coordY = centreY + y;

						if (coordX >= 0 && coordX < mapSize && coordY >= 0 && coordY < mapSize)
						{
							float weight = 1 - sqrt(sqrDist) / radius;
							weightSum += weight;
							weights[addIndex] = weight;
							xOffsets[addIndex] = x;
							yOffsets[addIndex] = y;
							addIndex++;

						}
					}
				}
			}
		}
		int numEntries = addIndex;
		for (int j = 0; j < numEntries; j++)
		{
			int ind = (yOffsets[j] + centreY)* mapSize + xOffsets[j] + centreX;
			erosionBrushIndices.push_back(XMINT2(xOffsets[j] + centreX, yOffsets[j] + centreY));
			erosionBrushWeights[i].push_back(weights[j] / weightSum);
		}

	}

	return erosionBrushIndices;
}

//https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-rendering-a-triangle/moller-trumbore-ray-triangle-intersection
//http://dl.booktolearn.com/ebooks2/computer/graphics/9781138627000_Real_Time_Rendering_4th_Edition_5726.pdf
//This function detects for a ray-triangle intersect
bool DisplayChunk::RayTriangleIntersect(Vector3 origin, Vector3 direction, Vector3 v0, Vector3 v1, Vector3 v2)
{
	float t, u, v;

	Vector3 v0v1 = v1 - v0;
	Vector3 v0v2 = v2 - v0;
	Vector3 pvec = direction.Cross(v0v2);
	float det = v0v1.Dot(pvec);

	if (det < 0.1)
	{
		return false;
	}

	float invDet = 1 / det;

	Vector3 tvec = origin - v0;
	u = tvec.Dot(pvec) * invDet;
	if (u < 0 || u > 1)
	{
		return false;
	}

	Vector3 qvec = tvec.Cross(v0v1);
	v = direction.Dot(qvec) * invDet;
	if (v < 0 || v > 1)
	{
		return false;
	}

	t = v0v2.Dot(qvec) * invDet;

	//compare vector tuv with each vertex values
	//the closest distance is the target vertex

	return true;

}

void DisplayChunk::PopulateChunkData(ChunkObject * SceneChunk)
{
	m_name = SceneChunk->name;
	m_chunk_x_size_metres = SceneChunk->chunk_x_size_metres;
	m_chunk_y_size_metres = SceneChunk->chunk_y_size_metres;
	m_chunk_base_resolution = SceneChunk->chunk_base_resolution;
	m_heightmap_path = SceneChunk->heightmap_path;
	m_tex_diffuse_path = SceneChunk->tex_diffuse_path;
	m_tex_splat_alpha_path = SceneChunk->tex_splat_alpha_path;
	m_tex_splat_1_path = SceneChunk->tex_splat_1_path;
	m_tex_splat_2_path = SceneChunk->tex_splat_2_path;
	m_tex_splat_3_path = SceneChunk->tex_splat_3_path;
	m_tex_splat_4_path = SceneChunk->tex_splat_4_path;
	m_render_wireframe = SceneChunk->render_wireframe;
	m_render_normals = SceneChunk->render_normals;
	m_tex_diffuse_tiling = SceneChunk->tex_diffuse_tiling;
	m_tex_splat_1_tiling = SceneChunk->tex_splat_1_tiling;
	m_tex_splat_2_tiling = SceneChunk->tex_splat_2_tiling;
	m_tex_splat_3_tiling = SceneChunk->tex_splat_3_tiling;
	m_tex_splat_4_tiling = SceneChunk->tex_splat_4_tiling;
}

void DisplayChunk::RenderBatch(std::shared_ptr<DX::DeviceResources>  DevResources)
{
	auto context = DevResources->GetD3DDeviceContext();

	m_terrainEffect->Apply(context);
	context->IASetInputLayout(m_terrainInputLayout.Get());

	m_batch->Begin();
	for (size_t i = 0; i < TERRAINRESOLUTION-1; i++)	//looping through QUADS.  so we subtrack one from the terrain array or it will try to draw a quad starting with the last vertex in each row. Which wont work
	{
		for (size_t j = 0; j < TERRAINRESOLUTION-1; j++)//same as above
		{
			m_batch->DrawQuad(m_terrainGeometry[i][j], m_terrainGeometry[i][j+1], m_terrainGeometry[i+1][j+1], m_terrainGeometry[i+1][j]); //bottom left bottom right, top right top left.
		}
	}
	m_batch->End();
}

void DisplayChunk::InitialiseBatch()
{
	//build geometry for our terrain array
	//iterate through all the vertices of our required resolution terrain.
	int index = 0;

	for (size_t i = 0; i < TERRAINRESOLUTION; i++)
	{
		for (size_t j = 0; j < TERRAINRESOLUTION; j++)
		{
			index = (TERRAINRESOLUTION * i) + j;
			m_terrainGeometry[i][j].position =			Vector3(j*m_terrainPositionScalingFactor-(0.5*m_terrainSize), (float)(m_heightMap[index])*m_terrainHeightScale, i*m_terrainPositionScalingFactor-(0.5*m_terrainSize));	//This will create a terrain going from -64->64.  rather than 0->128.  So the center of the terrain is on the origin
			m_terrainGeometry[i][j].normal =			Vector3(0.0f, 1.0f, 0.0f);						//standard y =up
			m_terrainGeometry[i][j].textureCoordinate =	Vector2(((float)m_textureCoordStep*j)*m_tex_diffuse_tiling, ((float)m_textureCoordStep*i)*m_tex_diffuse_tiling);				//Spread tex coords so that its distributed evenly across the terrain from 0-1
			
		}
	}
	CalculateTerrainNormals();
	
}

void DisplayChunk::LoadHeightMap(std::shared_ptr<DX::DeviceResources>  DevResources)
{
	auto device = DevResources->GetD3DDevice();
	auto devicecontext = DevResources->GetD3DDeviceContext();

	//load in heightmap .raw
	FILE *pFile = NULL;

	// Open The File In Read / Binary Mode.

	pFile = fopen(m_heightmap_path.c_str(), "rb");
	// Check To See If We Found The File And Could Open It
	if (pFile == NULL)
	{
		// Display Error Message And Stop The Function
		MessageBox(NULL, L"Can't Find The Height Map!", L"Error", MB_OK);
		return;
	}

	// Here We Load The .RAW File Into Our pHeightMap Data Array
	// We Are Only Reading In '1', And The Size Is (Width * Height)
	fread(m_heightMap, 1, TERRAINRESOLUTION*TERRAINRESOLUTION, pFile);

	fclose(pFile);

	//load in texture diffuse
	
	//load the diffuse texture
	std::wstring texturewstr = StringToWCHART(m_tex_diffuse_path);
	HRESULT rs;	
	rs = CreateDDSTextureFromFile(device, texturewstr.c_str(), NULL, &m_texture_diffuse);	//load tex into Shader resource	view and resource
	
	//setup terrain effect
	m_terrainEffect = std::make_unique<BasicEffect>(device);
	m_terrainEffect->EnableDefaultLighting();
	m_terrainEffect->SetLightingEnabled(true);
	m_terrainEffect->SetTextureEnabled(true);
	m_terrainEffect->SetTexture(m_texture_diffuse);

	void const* shaderByteCode;
	size_t byteCodeLength;

	m_terrainEffect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);

	//setup batch
	DX::ThrowIfFailed(
		device->CreateInputLayout(VertexPositionNormalTexture::InputElements,
			VertexPositionNormalTexture::InputElementCount,
			shaderByteCode,
			byteCodeLength,
			m_terrainInputLayout.GetAddressOf())
		);

	m_batch = std::make_unique<PrimitiveBatch<VertexPositionNormalTexture>>(devicecontext);

	
}

void DisplayChunk::SaveHeightMap()
{
/*	for (size_t i = 0; i < TERRAINRESOLUTION*TERRAINRESOLUTION; i++)
	{
		m_heightMap[i] = 0;
	}*/

	FILE *pFile = NULL;

	// Open The File In Read / Binary Mode.
	pFile = fopen(m_heightmap_path.c_str(), "wb+");;
	// Check To See If We Found The File And Could Open It
	if (pFile == NULL)
	{
		// Display Error Message And Stop The Function
		MessageBox(NULL, L"Can't Find The Height Map!", L"Error", MB_OK);
		return;
	}

	fwrite(m_heightMap, 1, TERRAINRESOLUTION*TERRAINRESOLUTION, pFile);
	fclose(pFile);
	
}

void DisplayChunk::UpdateTerrain()
{
	//all this is doing is transferring the height from the heigtmap into the terrain geometry.
	int index;
	for (size_t i = 0; i < TERRAINRESOLUTION; i++)
	{
		for (size_t j = 0; j < TERRAINRESOLUTION; j++)
		{
			index = (TERRAINRESOLUTION * i) + j;
			m_terrainGeometry[i][j].position.y = (float)(m_heightMap[index])*m_terrainHeightScale;	
		}
	}
	CalculateTerrainNormals();
}

//This function operates similarly to the update terrain function
//But instead updates the heightmap based on the geometry rather than the other way around
void DisplayChunk::UpdateHeightMap()
{
	//all this is doing is transferring the height from the heigtmap into the terrain geometry.
	int index;
	for (size_t i = 0; i < TERRAINRESOLUTION; i++)
	{
		for (size_t j = 0; j < TERRAINRESOLUTION; j++)
		{
			index = (TERRAINRESOLUTION * i) + j;
			(m_heightMap[index]) =  m_terrainGeometry[i][j].position.y / m_terrainHeightScale;
		}
	}
	CalculateTerrainNormals();
}

//This function is used to set the brush type from outwith this class
void DisplayChunk::SetBrushType(brushShape shape, int width)
{
	m_currentBrush.shape = shape;
	m_currentBrush.width = width;
}

//This function will flatten the terrain within the brush bounds to the set height value
void DisplayChunk::FlattenTerrain(float flattenHeight)
{
	//Ensure a valid index is selected
	if (m_SelectedVertX == -1 || m_SelectedVertY == -1)
	{
		return;
	}
	
	//Calculate upper and lower bounds of the brush
	int lowerX = m_SelectedVertX - m_currentBrush.width / 2;
	int lowerY = m_SelectedVertY - m_currentBrush.width / 2;
	int upperX = m_SelectedVertX + m_currentBrush.width / 2;
	int upperY = m_SelectedVertY + m_currentBrush.width / 2; 

	//Clamp the bounds of the brush to ensure they all fall within the terrain bounds
	if (upperX >= TERRAINRESOLUTION)
	{
		upperX = TERRAINRESOLUTION;
	}

	if (upperX <= 0)
	{
		upperX = 0;
	}

	if (lowerX >= TERRAINRESOLUTION)
	{
		lowerX = TERRAINRESOLUTION;
	}

	if (lowerX <= 0)
	{
		lowerX = 0;
	}

	if (upperY >= TERRAINRESOLUTION)
	{
		upperY = TERRAINRESOLUTION;
	}

	if (upperY <= 0)
	{
		upperY = 0;
	}

	if (lowerY >= TERRAINRESOLUTION)
	{
		lowerY = TERRAINRESOLUTION;
	}

	if (lowerY <= 0)
	{
		lowerY = 0;
	}

	//Loop from lower bounds to upper and set all vertices' y pos to the height value
	for (int i = lowerX; i < upperX; i++)
	{
		for (int j = lowerY; j < upperY; j++)
		{
			m_terrainGeometry[i][j].position.y = flattenHeight;			
		}
	}

}

//This function is used to raise or lower the terrain depending on the bool passed in
void DisplayChunk::RaiseOrLowerTerrain(bool raise)
{
	int multiplier = 1;
	//If raise is false, set the multiplier value to -1, 
	//so when the value is added at the end it will be a negative, thus lowering the height
	if (!raise)
	{
		multiplier = -1;
	}

	if (m_currentBrush.shape == SQUARE)
	{
		//Calculate bounds of the brush
		int lowerX = m_SelectedVertX - m_currentBrush.width / 2;
		int lowerY = m_SelectedVertY - m_currentBrush.width / 2;
		int upperX = m_SelectedVertX + m_currentBrush.width / 2;
		int upperY = m_SelectedVertY + m_currentBrush.width / 2;

		//Ensure the bounds fall within the terrain bounds
		if (upperX >= TERRAINRESOLUTION)
		{
			upperX = TERRAINRESOLUTION;
		}

		if (upperX <= 0)
		{
			upperX = 0;
		}

		if (lowerX >= TERRAINRESOLUTION)
		{
			lowerX = TERRAINRESOLUTION;
		}

		if (lowerX <= 0)
		{
			lowerX = 0;
		}

		if (upperY >= TERRAINRESOLUTION)
		{
			upperY = TERRAINRESOLUTION;
		}

		if (upperY <= 0)
		{
			upperY = 0;
		}

		if (lowerY >= TERRAINRESOLUTION)
		{
			lowerY = TERRAINRESOLUTION;
		}

		if (lowerY <= 0)
		{
			lowerY = 0;
		}

		//Loop through brush indices in the terrain
		for (int i = lowerX; i < upperX; i++)
		{
			for (int j = lowerY; j < upperY; j++)
			{
				//Calculate the weight of the current index
				//produces smaller values the further from the selected index (centre of brush)
				int numberOfCuts = (m_currentBrush.width - 1) / 2;
				float weight = 0;

				if (i == m_SelectedVertX && j == m_SelectedVertY)
				{
					weight = 1.0f;
				}
				else
				{
					int step = Vector2::Distance(Vector2(m_SelectedVertX, m_SelectedVertY), Vector2(i, j));

					weight = 1.0f - ((0.9 / numberOfCuts) * step);
				}

				//Raise or lower terrain by calculated amount
				//Or clamp if result is outwith the max/min height 
				if (m_terrainGeometry[i][j].position.y + (m_heightValue * weight * multiplier) <= 0.0f)
				{
					m_terrainGeometry[i][j].position.y = 0.0f;
				}
				else if (m_terrainGeometry[i][j].position.y + (m_heightValue * weight * multiplier) >= 255.0f * m_terrainHeightScale)
				{
					m_terrainGeometry[i][j].position.y = 255.0f * m_terrainHeightScale;
				}
				else
				{
					m_terrainGeometry[i][j].position.y += m_heightValue * weight * multiplier;
				}
			}
		}
	}
	else
	{
		/*std::vector<XMINT2> inds = InitBrushIndices();

		for (int i = 0; i < inds.size(); i++)
		{
			if (m_terrainGeometry[inds[i].x][inds[i].y].position.y + (m_heightValue * multiplier) <= 0.0f)
			{
				m_terrainGeometry[inds[i].x][inds[i].y].position.y = 0.0f;
			}
			else if (m_terrainGeometry[inds[i].x][inds[i].y].position.y + (m_heightValue * multiplier) >= 255.0f * m_terrainHeightScale)
			{
				m_terrainGeometry[inds[i].x][inds[i].y].position.y = 255.0f * m_terrainHeightScale;
			}
			else
			{
				m_terrainGeometry[inds[i].x][inds[i].y].position.y += m_heightValue * multiplier;
			}
			
		}*/
	}
}

//This function will smooth the terrain
void DisplayChunk::SmoothTerrain()
{
	//Calculate bounds of brush
	int lowerX = m_SelectedVertX - m_currentBrush.width / 2;
	int lowerY = m_SelectedVertY - m_currentBrush.width / 2;
	int upperX = m_SelectedVertX + m_currentBrush.width / 2;
	int upperY = m_SelectedVertY + m_currentBrush.width / 2;

	//Clamp values to ensure valid indices
	if (upperX >= TERRAINRESOLUTION)
	{
		upperX = TERRAINRESOLUTION;
	}

	if (upperX <= 0)
	{
		upperX = 0;
	}

	if (lowerX >= TERRAINRESOLUTION)
	{
		lowerX = TERRAINRESOLUTION;
	}

	if (lowerX <= 0)
	{
		lowerX = 0;
	}

	if (upperY >= TERRAINRESOLUTION)
	{
		upperY = TERRAINRESOLUTION;
	}

	if (upperY <= 0)
	{
		upperY = 0;
	}

	if (lowerY >= TERRAINRESOLUTION)
	{
		lowerY = TERRAINRESOLUTION;
	}

	if (lowerY <= 0)
	{
		lowerY = 0;
	}


	//Loop through brush indices on terrain
	for (int x = lowerX; x < upperX; x++)
	{
		for (int y = lowerY; y < upperY; y++)
		{
			//Calculate the average height of the 4 surrounding vertices where possible
			float averageHeight = 0.0f;
			int numberOfValues = 0;

			if (x - 1 >= 0)
			{
				if (m_terrainGeometry[x - 1][y].position.y <= 0.1f)
				{
					averageHeight += 0.1f;
				}
				else
				{
					averageHeight += m_terrainGeometry[x - 1][y].position.y;
				}
				numberOfValues++;
			}

			if (x + 1 <= TERRAINRESOLUTION - 1)
			{
				if (m_terrainGeometry[x + 1][y].position.y <= 0.1f)
				{
					averageHeight += 0.1f;
				}
				else
				{
					averageHeight += m_terrainGeometry[x + 1][y].position.y;
				}
				numberOfValues++;
			}

			if (y - 1 >= 0)
			{
				if (m_terrainGeometry[x][y - 1].position.y <= 0.1f)
				{
					averageHeight += 0.1f;
				}
				else
				{
					averageHeight += m_terrainGeometry[x][y - 1].position.y;
				}
				numberOfValues++;
			}

			if (y + 1 <= TERRAINRESOLUTION - 1)
			{
				if (m_terrainGeometry[x][y + 1].position.y <= 0.1f)
				{
					averageHeight += 0.1f;
				}
				else
				{
					averageHeight += m_terrainGeometry[x][y + 1].position.y;
				}
				numberOfValues++;
			}

			averageHeight /= numberOfValues;
			//Set the height of the current vertex to be the average value
			m_terrainGeometry[x][y].position.y = averageHeight;

		}
	}
}

//The following functions are unused as they were cut for time
#pragma region Generation
void DisplayChunk::GenerateHeightmap()
{
	//Set the range values to their starting values defined in the params section
	float minRange = 0;
	float maxRange = 255 * m_terrainHeightScale;

	//stores an average value for square step
	float average = 0;

	//Store the bounds of the current square 
	float startX = 0;
	float startY = 0;
	float endX = TERRAINRESOLUTION - 1;
	float endY = TERRAINRESOLUTION - 1;


	int numberOfSplits = 7;

	//Loop to find the number of splits needed to perform the algorithm
	for (int i = 0; i < INT16_MAX; i++)
	{
		if (pow(2, i) == TERRAINRESOLUTION - 1)
		{
			numberOfSplits = i + 1;
			//std::cout << numberOfSplits << std::endl;
			break;
		}

		if (i >= 150)
		{
			break;
			//std::cout << "Error performing midpoint displacement. Ensure MapSize is equal to (2^n) + 1." << std::endl;
		}
	}

	//Initialise the heights of the 4 outer points
	m_terrainGeometry[(int)startX][(int)startY].position.y = GetRandomHeight(maxRange, minRange);
	m_terrainGeometry[(int)startX][(int)endY].position.y =	 GetRandomHeight(maxRange, minRange);
	m_terrainGeometry[(int)endX][(int)startY].position.y =	 GetRandomHeight(maxRange, minRange);
	m_terrainGeometry[(int)endX][(int)endY].position.y =	 GetRandomHeight(maxRange, minRange);

	//Initialise square counters to track how many squares 
	//in the current section of the terrain
	int squareCount = 1;
	int horizontalSquareCount = 1;
	int verticalSquareCount = 1;

	int iterator = 0;

	int currentWidth = 0;
	int currentHeight = 0;

	while (iterator < numberOfSplits)
	{
		//divides each square into 4 new squares each time
		squareCount = pow(4, iterator);

		//Increment iterator
		iterator++;

		//Reset variables
		startX = 0;
		startY = 0;
		endX = TERRAINRESOLUTION - 1;
		endY = TERRAINRESOLUTION - 1;
		currentWidth = 0;
		currentHeight = 0;

		//Find width and height of the squares
		endX /= sqrt(squareCount);
		endY /= sqrt(squareCount);

		//Get the square counts
		horizontalSquareCount = sqrt(squareCount);
		verticalSquareCount = sqrt(squareCount);

		//Loop through horizontal squares
		for (int i = 0; i < horizontalSquareCount; i++)
		{
			currentWidth += endX;

			//Reset variables
			currentHeight = 0;
			startY = 0;

			//Break if out of bounds
			if (currentWidth > TERRAINRESOLUTION || startX > TERRAINRESOLUTION)
			{
				//std::cout << "Out of Bounds" << std::endl;
				break;
			}

			for (int j = 0; j < verticalSquareCount; j++)
			{
				currentHeight += endY;

				//Break if out of bounds
				if (currentHeight > TERRAINRESOLUTION || startY > TERRAINRESOLUTION)
				{
					//std::cout << "Out of Bounds" << std::endl;

					break;
				}

				//Perform square step to find 4 corners of square
				SquareStep(startX, startY, currentWidth, currentHeight);
				//Perform diamond step to find sides of square
				DiamondStep(startX, startY, currentWidth, currentHeight, maxRange, minRange);

				startY += endY;
			}
			startX += endX;
		}

		maxRange /= 2.0f;
		minRange /= 2.0f;
	}

	UpdateHeightMap();

}

void DisplayChunk::DiamondStep(int startX, int startY, int endX, int endY, float maxRange, float minRange)
{
	int midX, midY;

	//Left Side
	midX = startX;
	midY = GetMiddleIndex(startY, endY);
	m_terrainGeometry[midX][midX].position.y = GetAverage(cornerHeights[0], cornerHeights[1], maxRange, minRange);

	//Bottom Side
	midX = GetMiddleIndex(startX, endX);
	midY = endY;
	m_terrainGeometry[midX][midX].position.y = GetAverage(cornerHeights[1], cornerHeights[2], maxRange, minRange);

	//Right Side
	midX = endX;
	midY = GetMiddleIndex(startY, endY);
	m_terrainGeometry[midX][midX].position.y = GetAverage(cornerHeights[2], cornerHeights[3], maxRange, minRange);

	//Top Side
	midX = GetMiddleIndex(startX, endX);
	midY = startY;
	m_terrainGeometry[midX][midX].position.y = GetAverage(cornerHeights[0], cornerHeights[3], maxRange, minRange);
}

void DisplayChunk::SquareStep(int startX, int startY, int endX, int endY)
{
	//Get the corner values of the square
	cornerHeights[0] = m_terrainGeometry[startX][startY].position.y;
	cornerHeights[1] = m_terrainGeometry[startX][endY].position.y;
	cornerHeights[2] = m_terrainGeometry[endX][endY].position.y;
	cornerHeights[3] = m_terrainGeometry[endX][startY].position.y;

	//Get the middle index in the array
	int midX = GetMiddleIndex(startX, endX);
	int midY = GetMiddleIndex(startY, endY);

	//calculate the average height of the 4 corners
	float avg = (cornerHeights[0] + cornerHeights[1] + cornerHeights[2] + cornerHeights[3]) / 4;

	//assign the midpoint's height to the average value
	m_terrainGeometry[midX][midY].position.y = avg;
}

float DisplayChunk::GetRandomHeight(float max, float min)
{
	return ((float)(rand()) / ((float)RAND_MAX)) * (max - (min)) + (min);
}

int DisplayChunk::GetMiddleIndex(int a, int b)
{
	int mid = (a + b) / 2;
	int remainder = (a + b) % 2;
	if (remainder != 0)
	{
		mid += remainder;
	}
	return mid;
}

float DisplayChunk::GetAverage(float a, float b, float max, float min)
{
	//generate a random value within the terrain height range and combine with the average of the two heights
	float random = GetRandomHeight(max, min);
	return ((a + b) / 2) + random;
}

#pragma endregion

void DisplayChunk::CalculateTerrainNormals()
{
	int index1, index2, index3, index4;
	DirectX::SimpleMath::Vector3 upDownVector, leftRightVector, normalVector;



	for (int i = 0; i<(TERRAINRESOLUTION - 1); i++)
	{
		for (int j = 0; j<(TERRAINRESOLUTION - 1); j++)
		{
			upDownVector.x = (m_terrainGeometry[i + 1][j].position.x - m_terrainGeometry[i - 1][j].position.x);
			upDownVector.y = (m_terrainGeometry[i + 1][j].position.y - m_terrainGeometry[i - 1][j].position.y);
			upDownVector.z = (m_terrainGeometry[i + 1][j].position.z - m_terrainGeometry[i - 1][j].position.z);

			leftRightVector.x = (m_terrainGeometry[i][j - 1].position.x - m_terrainGeometry[i][j + 1].position.x);
			leftRightVector.y = (m_terrainGeometry[i][j - 1].position.y - m_terrainGeometry[i][j + 1].position.y);
			leftRightVector.z = (m_terrainGeometry[i][j - 1].position.z - m_terrainGeometry[i][j + 1].position.z);


			leftRightVector.Cross(upDownVector, normalVector);	//get cross product
			normalVector.Normalize();			//normalise it.

			m_terrainGeometry[i][j].normal = normalVector;	//set the normal for this point based on our result
		}
	}
}

#pragma once
#include "pch.h"
#include "DeviceResources.h"
#include "ChunkObject.h"
//geometric resoltuion - note,  hard coded.
#define TERRAINRESOLUTION 128

class DisplayChunk
{
public:
	DisplayChunk();
	~DisplayChunk();
	void PopulateChunkData(ChunkObject * SceneChunk);
	void RenderBatch(std::shared_ptr<DX::DeviceResources>  DevResources);
	void InitialiseBatch();	//initial setup, base coordinates etc based on scale
	void LoadHeightMap(std::shared_ptr<DX::DeviceResources>  DevResources);
	void SaveHeightMap();			//saves the heigtmap back to file.
	void UpdateTerrain();			//updates the geometry based on the heigtmap
	void GenerateHeightmap();		//creates or alters the heightmap
	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionNormalTexture>>  m_batch;
	std::unique_ptr<DirectX::BasicEffect>       m_terrainEffect;

	ID3D11ShaderResourceView *					m_texture_diffuse;				//diffuse texture
	Microsoft::WRL::ComPtr<ID3D11InputLayout>   m_terrainInputLayout;
	enum brushShape{ CIRCLE, SQUARE };
	struct brushType
	{
		brushShape shape = SQUARE;
		//Width of the square/diameter of the circle
		int width = 5;
	};
	brushType m_currentBrush;
	int m_selectedTechnique = 0;
	float m_heightValue = 0.0f;
	int m_SelectedVertX = TERRAINRESOLUTION/2;
	int m_SelectedVertY = TERRAINRESOLUTION/2;
	bool m_TerrainEditMade = false;


	void PickTerrain(DirectX::XMFLOAT3 A, DirectX::XMFLOAT3 B);
	void SetBrushType(brushShape shape, int width);
	BYTE m_heightMap[TERRAINRESOLUTION*TERRAINRESOLUTION];
	bool RayTriangleIntersect(DirectX::SimpleMath::Vector3 origin, DirectX::SimpleMath::Vector3 direction, DirectX::SimpleMath::Vector3 v0, DirectX::SimpleMath::Vector3 v1, DirectX::SimpleMath::Vector3 v2);
private:
	DirectX::VertexPositionNormalTexture m_terrainGeometry[TERRAINRESOLUTION][TERRAINRESOLUTION];
	void CalculateTerrainNormals();
	void DiamondStep(int startX, int startY, int endX, int endY, float maxRange, float minRange);
	void SquareStep(int startX, int startY, int endX, int endY);
	float GetRandomHeight(float max, float min);
	int GetMiddleIndex(int a, int b);
	float GetAverage(float a, float b, float max, float min);
	void UpdateHeightMap();
	void FlattenTerrain(float flattenHeight);
	void RaiseOrLowerTerrain(bool raise);
	void SmoothTerrain();
	std::vector<DirectX::XMINT2> InitBrushIndices();

	float	m_terrainHeightScale;
	int		m_terrainSize;				//size of terrain in metres
	float	m_textureCoordStep;			//step in texture coordinates between each vertex row / column
	float   m_terrainPositionScalingFactor;	//factor we multiply the position by to convert it from its native resolution( 0- Terrain Resolution) to full scale size in metres dictated by m_Terrainsize

	std::string m_name;
	int m_chunk_x_size_metres;
	int m_chunk_y_size_metres;
	int m_chunk_base_resolution;
	std::string m_heightmap_path;
	std::string m_tex_diffuse_path;
	std::string m_tex_splat_alpha_path;
	std::string m_tex_splat_1_path;
	std::string m_tex_splat_2_path;
	std::string m_tex_splat_3_path;
	std::string m_tex_splat_4_path;
	bool m_render_wireframe;
	bool m_render_normals;
	int m_tex_diffuse_tiling;
	int m_tex_splat_1_tiling;
	int m_tex_splat_2_tiling;
	int m_tex_splat_3_tiling;
	int m_tex_splat_4_tiling;

	//Array of 4 corner heights used when performing midpoint displacement
	float cornerHeights[4];
};


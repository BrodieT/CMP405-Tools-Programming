#pragma once
#include "resource.h"
#include <DirectXMath.h>
#include <d3d11.h>
#include <SimpleMath.h>
#include "InputCommands.h"

class Camera
{
public:
	DirectX::SimpleMath::Vector3 m_camPosition;
	DirectX::SimpleMath::Vector3 m_camOrientation;
	DirectX::SimpleMath::Vector3 m_camLookAt;
	DirectX::SimpleMath::Vector3 m_camLookDirection;
	DirectX::SimpleMath::Vector3 m_camRight;
	DirectX::SimpleMath::Matrix m_view;
	float m_movespeed;
	float m_camRotRate;

	InputCommands* m_InputCommands;
	
	RECT screenDim;
	
	DirectX::SimpleMath::Vector3 GetOrientation() { return m_camOrientation; };
	float moveZone = 150.0f;

	void FocusCamera(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 scale);

	Camera();
	~Camera();
	void Update();

	DirectX::SimpleMath::Matrix GetViewMatrix() 
	{
		return m_view;
	};
private:
};


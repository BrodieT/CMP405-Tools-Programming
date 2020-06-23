#include "Camera.h"
using namespace DirectX;
using namespace SimpleMath;

Camera::Camera() 
{
	//functional
	m_movespeed = 0.30;
	m_camRotRate = 3.0;

	//camera
	m_camPosition.x = 0.0f;
	m_camPosition.y = 3.7f;
	m_camPosition.z = -3.5f;

	m_camOrientation.x = 105;
	m_camOrientation.y = 0;
	m_camOrientation.z = 0;

	m_camLookAt.x = 0.0f;
	m_camLookAt.y = 0.0f;
	m_camLookAt.z = 0.0f;

	m_camLookDirection.x = 0.0f;
	m_camLookDirection.y = 0.0f;
	m_camLookDirection.z = 0.0f;

	m_camRight.x = 0.0f;
	m_camRight.y = 0.0f;
	m_camRight.z = 0.0f;

}

Camera::~Camera()
{

}

void Camera::Update()
{
	Vector3 planarMotionVector = m_camLookDirection;
	planarMotionVector.y = 0.0;

	//Change the cam orientation based on input commands
	//Additional check to ensure the RMB is pressed before rotating
	if (m_InputCommands->mouse_RB_Down)
	{
		if (m_InputCommands->rotRight)
		{
			m_camOrientation.y -= m_camRotRate;
		}
		if (m_InputCommands->rotLeft)
		{
			m_camOrientation.y += m_camRotRate;
		}
		if (m_InputCommands->rotUp)
		{
			m_camOrientation.x -= m_camRotRate;
		}
		if (m_InputCommands->rotDown)
		{
			m_camOrientation.x += m_camRotRate;
		}

		if (m_camOrientation.x >= 179)
		{
			m_camOrientation.x = 179;
		}
		else if (m_camOrientation.x <= 1)
		{
			m_camOrientation.x = 1;
		}

	}
	else
	{
		m_InputCommands->rotLeft = false;
		m_InputCommands->rotRight = false;
		m_InputCommands->rotUp = false;
		m_InputCommands->rotDown = false;
	}

	////create look direction from Euler angles in m_camOrientation
	m_camLookDirection.z = cos((m_camOrientation.y) * 3.1415 / 180) * sin((m_camOrientation.x) * 3.1415 / 180);
	m_camLookDirection.x = sin((m_camOrientation.y) * 3.1415 / 180) * sin((m_camOrientation.x) * 3.1415 / 180);
	m_camLookDirection.y = cos((m_camOrientation.x) * 3.1415 / 180);
	m_camLookDirection.Normalize();


	//create right vector from look Direction
	m_camLookDirection.Cross(Vector3::UnitY, m_camRight);

	//process input and update position
	if (m_InputCommands->forward)
	{
		m_camPosition += m_camLookDirection * m_movespeed;
	}
	if (m_InputCommands->back)
	{
		m_camPosition -= m_camLookDirection * m_movespeed;
	}
	if (m_InputCommands->right)
	{
		m_camPosition += m_camRight * m_movespeed;
	}
	if (m_InputCommands->left)
	{
		m_camPosition -= m_camRight * m_movespeed;
	}
	if (m_InputCommands->moveUp)
	{
		m_camPosition.y += m_movespeed;
	}
	if (m_InputCommands->moveDown)
	{
		m_camPosition.y -= m_movespeed;
	}

	//update lookat point
	m_camLookAt = m_camPosition + m_camLookDirection;

	//apply camera vectors
	m_view = Matrix::CreateLookAt(m_camPosition, m_camLookAt, Vector3::UnitY);

}

//This function will focus the camera on a given point
//The scale is passed in to ensure the size of the object is considered when moving the camera back from the object
void Camera::FocusCamera(XMFLOAT3 position, XMFLOAT3 scale)
{
	m_camOrientation = Vector3(225, 180, 0);
	m_camPosition = position - (XMFLOAT3(0, -5, 5) * scale);
	Update();
}

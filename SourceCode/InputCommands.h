#pragma once

struct InputCommands
{
	bool forward;
	bool back;
	bool right;
	bool left;
	bool rotRight;
	bool rotLeft;
	bool rotUp;
	bool rotDown;
	bool moveUp;
	bool moveDown;

	bool mouseButtonUp;

	//

	bool mouse_LB_Down;
	bool mouse_RB_Down;


	float mouse_X;
	float mouse_Y;

	float mouse_Prev_X;
	float mouse_Prev_Y;

	bool FocusCam;
};

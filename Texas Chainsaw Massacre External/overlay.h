#pragma once
#include "precomp.h"

class overlay
{
public:
	static void menuDrawing();



	static void ApplyTheme();


	static void Render();

	static inline bool Enabled;

	static void drawBox(Vector3 World2Screen);

	static void drawBox(Vector3 World2Screen, float rounding);

	static void drawBox(Vector3 World2Screen, float rounding, ImColor color);

	static void drawBox(Vector3 World2Screen, ImColor color);




	static void drawLine(Vector3 World2Screen);

	static void drawLine(Vector3 World2Screen, Vector3 Origin);


	static void drawText(Vector3 World2Screen, float yOffset, const char* text);

	static void drawText(Vector3 World2Screen, float yOffset, const char* text, ImColor color);

	static void drawTextSmall(Vector3 World2Screen, float yOffset, const char* text);


	static void drawHealth(Vector3 World2Screen, float health);


};
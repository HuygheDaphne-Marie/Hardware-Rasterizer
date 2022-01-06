#pragma once
#include "EMath.h"
#include "ERGBColor.h"

struct IVertex
{
	Elite::FPoint3 position;
	Elite::FPoint2 uv;
	Elite::FVector3 normal;
	Elite::FVector3 tangent;
	Elite::RGBColor color{1.0f, 1.0f, 1.0f};
};

//struct OVertex
//{
//	// Todo
//};

﻿#pragma once
#include <Utils/Engine.h>
#include <Utils/ue4math/ue4math.h>
#include <Utils/ue4math/matrix.h>
#include <d3dx9math.h>
#include <cmath>
//菜鸟基地QQ群：964504044
class VectorHelper
{
public:
	static FVector RotateVector(const FMatrix& RotationMatrix, const FVector& Vec);
	static FVector GetBoneWithRotation(FTransform BoneArray, FTransform ComponentToWorld);
	static FVector2D WorldToScreen(const FVector& WorldLocation);
	static bool MatrixToScreen(const FVector& objectCoord, FVector2D& returnCoord, const float ViewWorld[4][4]);
	static bool WorldToScreen(FVector Position, FVector2D& Screen, D3DMATRIX ViewMatrix);

	static double RadiansToDegrees(double radians) {
		return radians * double(57.29577951308);
	}

	static bool IsInScreen(const FVector2D WorldToScreen);

	static FVector CalculateAngles(FVector D, FVector W) {
		float deltaX = W.X - D.X;
		float deltaY = W.Y - D.Y;
		float deltaZ = W.Z - D.Z;
		FVector angles = FVector(float(RadiansToDegrees(atan2(deltaY, deltaX))), float(RadiansToDegrees(atan2(deltaZ, sqrt(deltaX * deltaX + deltaY * deltaY)))), 0.0f);
		return angles;
	}

	static FRotator CalcAngle(FVector LocalHeadPosition, FVector AimPosition)
	{
		FVector vecDelta((LocalHeadPosition.X - AimPosition.X), (LocalHeadPosition.Y - AimPosition.Y), (LocalHeadPosition.Z - AimPosition.Z));
		float hyp = (float)sqrt(vecDelta.X * vecDelta.X + vecDelta.Y * vecDelta.Y);

		FRotator ViewAngles{ 0 };
		ViewAngles.Pitch = -(float)atan(vecDelta.Z / hyp) * (float)(180.f / 3.1415f);
		ViewAngles.Yaw = (float)atan(vecDelta.Y / vecDelta.X) * (float)(180.f / 3.1415f);
		ViewAngles.Roll = (float)0.f;

		if (vecDelta.X >= 0.f)
			ViewAngles.Yaw += 180.0f;

		return ViewAngles;
	}

	static FRotator ClampAngles(FRotator Rotator)
	{
		FRotator angles{ Rotator.Pitch, Rotator.Yaw, Rotator.Roll };

		while (angles.Yaw < -180.0f)
			angles.Yaw += 360.0f;
		while (angles.Yaw > 180.0f)
			angles.Yaw -= 360.0f;

		if (angles.Pitch < -74.f)
			angles.Pitch = -74.f;
		if (angles.Pitch > 74.f)
			angles.Pitch = 74.f;

		return angles;
	}

	static float SmoothOutYaw(float targetYaw, float currentYaw, float smoothness)
	{
		if (targetYaw > 0.f && currentYaw < 0.f)
		{
			float dist = 180.f - targetYaw + 180.f + currentYaw;
			if (dist < 180.f)
				return currentYaw - dist / smoothness;
		}
		else if (currentYaw > 0.f && targetYaw < 0.f)
		{
			float dist = 180.f - currentYaw + 180.f + targetYaw;
			if (dist < 180.f)
				return currentYaw + dist / smoothness;
		}

		return currentYaw + (targetYaw - currentYaw) / smoothness;
	}
};
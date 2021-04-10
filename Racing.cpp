#include <iostream>

using namespace std;

#include "olcConsoleGameEngine.h"

class Racing : public olcConsoleGameEngine
{
public:
	Racing()
	{
		m_sAppName = L"Console Racing Game";
	}

private:
	float fCarPos = 0.0f;
	float fDistance = 0.0f;
	float fSpeed = 0.0f;

	float fCurvature = 0.0f;

	vector<pair<float, float>> vecTrack;	// curvature, distance

protected:

	virtual bool OnUserCreate()
	{
		vecTrack.push_back(make_pair(0.0f, 10.0f));	// Short section for start/finish line
		vecTrack.push_back(make_pair(0.0f, 200.0f));
		vecTrack.push_back(make_pair(1.0f, 200.0f));
		vecTrack.push_back(make_pair(0.0f, 400.0f));
		vecTrack.push_back(make_pair(-1.0f, 100.0f));
		vecTrack.push_back(make_pair(0.0f, 200.0f));
		vecTrack.push_back(make_pair(-1.0f, 200.0f));
		vecTrack.push_back(make_pair(1.0f, 200.0f));

		return true;
	}

	virtual bool OnUserUpdate(float fElapsedTime)
	{

		if (m_keys[VK_UP].bHeld)
			fSpeed += 2.0f * fElapsedTime;
		else
			fSpeed -= 1.0f * fElapsedTime;

		// Clamp Speed
		if (fSpeed < 0.0f) fSpeed = 0.0f;
		if (fSpeed > 1.0f) fSpeed = 1.0f;

		// Move car along track according to car speed
		fDistance += (70.0f * fSpeed) * fElapsedTime;

		// Get Point on track
		float fOffset = 0;
		int nTrackSection = 0;

		// Find position on track (could optimise)
		while (nTrackSection < vecTrack.size() && fOffset <= fDistance)
		{
			fOffset += vecTrack[nTrackSection].second;
			nTrackSection++;
		}

		float fTargetCurvature = vecTrack[nTrackSection - 1].first;

		float fTrackCurveDiff = (fTargetCurvature - fCurvature) * fElapsedTime * fSpeed;
		fCurvature += fTrackCurveDiff;


		Fill(0, 0, ScreenWidth(), ScreenHeight(), L' ', 0);	// Initialize the screen as an empty space.....

		// Drawing the track
		// The track begins from the middle of the screen vertically 
		for (int y = 0; y < ScreenHeight() / 2; y++)
		{
			for (int x = 0; x < ScreenWidth(); x++)
			{
				// Perspective: current row / half of screen to get percentage
				// y goes down (away from the bottom of screen): perspective goes toward 0: things get smaller
				// y goes up (toward the bottom of screen): perspective goes toward 1: things get bigger
				float fPerspective = (float)y / (ScreenHeight() / 2.0f);
				float fMiddlePoint = 0.5f + fCurvature * powf((1.0f - fPerspective), 3);
				float fRoadWidth = 0.1f + fPerspective * 0.8f;
				float fClipWidth = fRoadWidth * 0.15f;

				fRoadWidth *= 0.5f;

				int nLeftGrass = (fMiddlePoint - fRoadWidth - fClipWidth) * ScreenWidth();
				int nLeftClip = (fMiddlePoint - fRoadWidth) * ScreenWidth();
				int nRightClip = (fMiddlePoint + fRoadWidth) * ScreenWidth();
				int nRightGrass = (fMiddlePoint + fRoadWidth + fClipWidth) * ScreenWidth();

				int nRow = ScreenHeight() / 2 + y;	// Convert posistion to bottom half of the screen

				int nGrassColour = sinf(20.0f * powf(1.0f - fPerspective, 3) + fDistance * 0.1f) > 0.0f ? FG_GREEN : FG_DARK_GREEN;
				int nClipColour = sinf(80.0f * powf(1.0f - fPerspective, 3) + fDistance * 0.1f) > 0.0f ? FG_RED : FG_WHITE;

				if (x >= 0 && x < nLeftGrass)
					Draw(x, nRow, PIXEL_SOLID, nGrassColour);
				if (x >= nLeftGrass && x < nLeftClip)
					Draw(x, nRow, PIXEL_SOLID, nClipColour);
				if (x >= nLeftClip && x < nRightClip)
					Draw(x, nRow, PIXEL_SOLID, FG_GREY);
				if (x >= nRightClip && x < nRightGrass)
					Draw(x, nRow, PIXEL_SOLID, nClipColour);
				if (x >= nRightGrass && x < ScreenWidth())
					Draw(x, nRow, PIXEL_SOLID, nGrassColour);
			}
		}


		// Draw car
		int nCarPos = ScreenWidth() / 2 + ((int)(ScreenWidth() * fCarPos) / 2.0f) - 7;

		DrawStringAlpha(nCarPos, 80, L"   ||****||   ");
		DrawStringAlpha(nCarPos, 81, L"   ||****||   ");
		DrawStringAlpha(nCarPos, 82, L"      ##      ");
		DrawStringAlpha(nCarPos, 83, L"     #$$#     ");
		DrawStringAlpha(nCarPos, 84, L"     #$$#     ");
		DrawStringAlpha(nCarPos, 85, L"|||  ####  |||");
		DrawStringAlpha(nCarPos, 86, L"|||********|||");
		DrawStringAlpha(nCarPos, 87, L"|||  ####  |||");


		return true;
	}

};

int main()
{
	Racing game;
	game.ConstructConsole(160, 100, 8, 8);
	game.Start();

	return 0;
}
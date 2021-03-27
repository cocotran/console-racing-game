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

protected:

	virtual bool OnUserCreate()
	{
		return true;
	}

	virtual bool OnUserUpdate(float fElapsedTime)
	{
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
				float fMiddlePoint = 0.5f;
				float fRoadWidth = 0.1f + fPerspective * 0.8f;
				float fClipWidth = fRoadWidth * 0.15f;

				fRoadWidth *= 0.5f;

				int nLeftGrass = (fMiddlePoint - fRoadWidth - fClipWidth) * ScreenWidth();
				int nLeftClip = (fMiddlePoint - fRoadWidth) * ScreenWidth();
				int nRightClip = (fMiddlePoint + fRoadWidth) * ScreenWidth();
				int nRightGrass = (fMiddlePoint + fRoadWidth + fClipWidth) * ScreenWidth();

				int nRow = ScreenHeight() / 2 + y;	// Convert posistion to bottom half of the screen

				if (x >= 0 && x < nLeftGrass)
					Draw(x, nRow, PIXEL_SOLID, FG_GREEN);
				if (x >= nLeftGrass && x < nLeftClip)
					Draw(x, nRow, PIXEL_SOLID, FG_RED);
				if (x >= nLeftClip && x < nRightClip)
					Draw(x, nRow, PIXEL_SOLID, FG_GREY);
				if (x >= nRightClip && x < nRightGrass)
					Draw(x, nRow, PIXEL_SOLID, FG_RED);
				if (x >= nRightGrass && x < ScreenWidth())
					Draw(x, nRow, PIXEL_SOLID, FG_GREEN);
			}
		}


		// Draw car
		int nCarPos = ScreenWidth() / 2 + ((int)(ScreenWidth() * fCarPos) / 2.0f) - 7;

		DrawStringAlpha(nCarPos, 80, L"   ||####||   ");
		DrawStringAlpha(nCarPos, 81, L"   ||####||   ");
		DrawStringAlpha(nCarPos, 82, L"      ##      ");
		DrawStringAlpha(nCarPos, 83, L"     ####     ");
		DrawStringAlpha(nCarPos, 84, L"     ####     ");
		DrawStringAlpha(nCarPos, 85, L"|||  ####  |||");
		DrawStringAlpha(nCarPos, 86, L"|||########|||");
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
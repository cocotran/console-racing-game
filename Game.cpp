#define OLC_PGE_APPLICATION
#include <iostream>
#include <string>


using namespace std;

#include "olcPixelGameEngine.h"

class RacingWasm : public olc::PixelGameEngine
{
public:
	RacingWasm()
	{
		sAppName = "Console Racing Game";
	}

private:
	float fCarPos = 0.0f;
	float fDistance = 0.0f;
	float fSpeed = 0.0f;

	float fCurvature = 0.0f;
	float fTrackCurvature = 0.0f;
	float fPlayerCurvature = 0.0f;
	float fTrackDistance = 0.0f;

	float fCurrentLapTime = 0.0f;

	vector<pair<float, float>> vecTrack;	// curvature, distance
	list<float> listLapTimes;

	//std::unique_ptr<olc::Sprite> sprTile;

protected:

	virtual bool OnUserCreate() override
	{
		vecTrack.push_back(make_pair(0.0f, 10.0f));	// Short section for start/finish line
		vecTrack.push_back(make_pair(0.0f, 200.0f));
		vecTrack.push_back(make_pair(1.0f, 200.0f));
		vecTrack.push_back(make_pair(0.0f, 400.0f));
		vecTrack.push_back(make_pair(-1.0f, 100.0f));
		vecTrack.push_back(make_pair(0.0f, 200.0f));
		vecTrack.push_back(make_pair(-1.0f, 200.0f));
		vecTrack.push_back(make_pair(1.0f, 200.0f));


		for (auto t : vecTrack)
			fTrackDistance += t.second;


		listLapTimes = { 0,0,0,0,0 };
		fCurrentLapTime = 0.0f;

		//sprTile = std::make_unique<olc::Sprite>("./images/car.png");

		return true;
	}

	virtual bool OnUserUpdate(float fElapsedTime) override
	{

		if (GetKey(olc::Key::UP).bHeld)
			fSpeed += 2.0f * fElapsedTime;
		else
			fSpeed -= 1.0f * fElapsedTime;

		int nCarDirection = 0;

		// Steering 
		if (GetKey(olc::Key::LEFT).bHeld)
		{
			fPlayerCurvature -= 0.7f * fElapsedTime;
			nCarDirection = -1;
		}

		if (GetKey(olc::Key::RIGHT).bHeld)
		{
			fPlayerCurvature += 0.7f * fElapsedTime;
			nCarDirection = +1;
		}


		// Making the grass more "sticky"
		if (fabs(fPlayerCurvature - fTrackCurvature) >= 0.8f)
			fSpeed -= 5.0f * fElapsedTime;

		// Clamp Speed
		if (fSpeed < 0.0f) fSpeed = 0.0f;
		if (fSpeed > 1.0f) fSpeed = 1.0f;

		// Move car along track according to car speed
		fDistance += (70.0f * fSpeed) * fElapsedTime;

		// Get Point on track
		float fOffset = 0;
		int nTrackSection = 0;

		fCurrentLapTime += fElapsedTime;
		if (fDistance >= fTrackDistance)
		{
			fDistance -= fTrackDistance;
			listLapTimes.push_front(fCurrentLapTime);
			listLapTimes.pop_back();
			fCurrentLapTime = 0.0f;
		}


		// Find position on track (could optimise)
		while (nTrackSection < vecTrack.size() && fOffset <= fDistance)
		{
			fOffset += vecTrack[nTrackSection].second;
			nTrackSection++;
		}

		float fTargetCurvature = vecTrack[nTrackSection - 1].first;

		float fTrackCurveDiff = (fTargetCurvature - fCurvature) * fElapsedTime * fSpeed;
		fCurvature += fTrackCurveDiff;

		fTrackCurvature += (fCurvature)*fElapsedTime * fSpeed;


		// Draw Sky - light blue and dark blue
		for (int y = 0; y < ScreenHeight() / 2; y++)
			for (int x = 0; x < ScreenWidth(); x++)
				Draw(x, y, olc::DARK_BLUE);


		// Draw Scenery - our hills are a rectified sine wave
		for (int x = 0; x < ScreenWidth(); x++)
		{
			int nHillHeight = (int)(fabs(sinf(x * 0.01f + fTrackCurvature) * 16.0f));
			for (int y = (ScreenHeight() / 2) - nHillHeight; y < ScreenHeight() / 2; y++)
				Draw(x, y, olc::DARK_YELLOW);
		}


		//Fill(0, 0, ScreenWidth(), ScreenHeight(), L' ', 0);	// Initialize the screen as an empty space.....

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

				olc::Pixel nGrassColour = sinf(20.0f * powf(1.0f - fPerspective, 3) + fDistance * 0.1f) > 0.0f ? olc::GREEN : olc::DARK_GREEN;
				olc::Pixel nClipColour = sinf(80.0f * powf(1.0f - fPerspective, 3) + fDistance * 0.1f) > 0.0f ? olc::RED : olc::WHITE;


				olc::Pixel nRoadColor = (nTrackSection - 1) == 0 ? olc::WHITE : olc::GREY;


				if (x >= 0 && x < nLeftGrass)
					Draw(x, nRow, nGrassColour);
				if (x >= nLeftGrass && x < nLeftClip)
					Draw(x, nRow, nClipColour);
				if (x >= nLeftClip && x < nRightClip)
					Draw(x, nRow, nRoadColor);
				if (x >= nRightClip && x < nRightGrass)
					Draw(x, nRow, nClipColour);
				if (x >= nRightGrass && x < ScreenWidth())
					Draw(x, nRow, nGrassColour);
			}
		}


		// Draw car
		fCarPos = fPlayerCurvature - fTrackCurvature;
		int nCarPos = ScreenWidth() / 2 + ((int)(ScreenWidth() * fCarPos) / 2.0f);

		switch (nCarDirection)
		{
		case 0:
			FillTriangle(nCarPos, 80, nCarPos - 5, 85, nCarPos + 5, 85, olc::BLACK);
			break;

		case +1:
			FillTriangle(nCarPos + 4, 80, nCarPos - 5, 85, nCarPos + 5, 85, olc::BLACK);
			break;

		case -1:
			FillTriangle(nCarPos - 4, 80, nCarPos - 5, 85, nCarPos + 5, 85, olc::BLACK);
			break;
		}


		// Draw Stats
		/*DrawString(0, 0, "Distance: " + to_string(fDistance));
		DrawString(0, 1, "Target Curvature: " + to_string(fCurvature));
		DrawString(0, 2, "Player Curvature: " + to_string(fPlayerCurvature));
		DrawString(0, 3, "Player Speed: " + to_string(fSpeed));
		DrawString(0, 4, "Track Curvature: " + to_string(fTrackCurvature));*/



		auto disp_time = [](float t) -> string	// Lambda expression
		{
			int nMinutes = t / 60.0f;
			int nSeconds = t - (nMinutes * 60.0f);
			int nMilliSeconds = (t - (float)nSeconds) * 1000.0f;

			return to_string(nMinutes) + "." + to_string(nSeconds) + ":" + to_string(nMilliSeconds);
		};

		// DrawString(10, 8, disp_time(fCurrentLapTime), olc::BLACK, 0.3);

		// Display last 5 lap times
		/*int j = 10;
		for (auto l : listLapTimes)
		{
			DrawString(10, j, disp_time(l));
			j++;
		}*/



		if (GetKey(olc::Key::ESCAPE).bPressed) return false;

		return true;
	}

};

int main()
{
	RacingWasm game;
	if (game.Construct(160, 100, 5, 5))
		game.Start();

	return 0;
}
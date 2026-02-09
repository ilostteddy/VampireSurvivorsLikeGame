#pragma once
#include <graphics.h>
#include <windows.h>

class Bullet {
public:
	POINT position = { 0, 0 };

	Bullet() = default;
	~Bullet() = default;

	void Draw() const {
		setlinecolor(RGB(255, 155, 50));
		setfillcolor(RGB(200, 75, 10));
		fillcircle(position.x, position.y, Radius);
	}

private:
	const int Radius = 10;

};
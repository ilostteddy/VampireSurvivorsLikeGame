#pragma once
#include <graphics.h>
#include <windows.h>

class Button {
public:
	Button(RECT rect, LPCTSTR path_img_idle, LPCTSTR path_img_hover, LPCTSTR path_img_pressed)  {
		button_rect = rect;

		loadimage(&img_idle, path_img_idle);
		loadimage(&img_hover, path_img_hover);
		loadimage(&img_pressed, path_img_pressed);
	}

	~Button() = default;

	void ProcessMouseEvent(const ExMessage& msg) {
		switch（msg.message）{
		case WM_MOUSEMOVE:
			if(current_state == ButtonState::Idle && CheckCursorHit(msg.x, msg.y))
		}

	}

	void button_Draw() {
		switch (current_state) {
		case ButtonState::Idle:
			putimage(button_rect.left, button_rect.top, &img_idle);
			break;
		case ButtonState::Hover:
			putimage(button_rect.left, button_rect.top, &img_hover);
			break;
		case ButtonState::Pressed:
			putimage(button_rect.left, button_rect.top, &img_pressed);
			break;
		}
	}

private:
	ExMessage msg;

	RECT button_rect;  // 定义按钮的矩形区域
	IMAGE img_idle;
	IMAGE img_hover;
	IMAGE img_pressed;

	// 定义按钮状态的枚举类型
	enum class ButtonState {
		Idle,
		Hover,
		Pressed
	};
	ButtonState current_state = ButtonState::Idle; // 当前按钮状态，默认为Idle

	// 检查光标是否在按钮区域内
	bool CheckCursorHit(int x, int y) const {
		return (x >= button_rect.left && x <= button_rect.right &&
			y >= button_rect.top && y <= button_rect.bottom);
	}
};
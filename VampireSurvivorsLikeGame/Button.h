#pragma once
#include <graphics.h>
#include <windows.h>

class Button {
public:
	bool is_game_started = false; // 游戏是否开始的标志变量

	Button(RECT rect, LPCTSTR path_img_idle, LPCTSTR path_img_hover, LPCTSTR path_img_pressed)  {
		button_rect = rect;

		loadimage(&img_idle, path_img_idle);
		loadimage(&img_hover, path_img_hover);
		loadimage(&img_pressed, path_img_pressed);
	}
	 
	~Button() = default;


	void ProcessMouseEvent(const ExMessage& msg) {
		switch (msg.message){
		// 鼠标移动到按钮上时，切换到Hover状态；鼠标移出按钮时，切换回Idle状态
		case WM_MOUSEMOVE:
			if (current_state == ButtonState::Idle && CheckCursorHit(msg.x, msg.y))
				current_state = ButtonState::Hover;
			else if (current_state == ButtonState::Hover && !CheckCursorHit(msg.x, msg.y))
				current_state = ButtonState::Idle;
			break;
		// 鼠标按下时，如果光标在按钮区域内，切换到Pressed状态
		case WM_LBUTTONDOWN:
			if (CheckCursorHit(msg.x, msg.y))
				current_state = ButtonState::Pressed;
			break;
		// 鼠标松开时，如果当前状态是Pressed，触发点击事件
		case WM_LBUTTONUP:
			if (current_state == ButtonState::Pressed)
				OnClick();  // 这里可以添加点击事件的处理逻辑，例如调用一个回调函数
			break;
		// 其他消息不处理
		default:
			break;
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

protected:
	virtual void OnClick() = 0; // 纯虚函数，子类必须实现点击事件处理逻辑

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
#pragma once

#include "Button.h"

// --- 关键修改：告诉编译器，这两个变量在别的文件里定义了 ---
extern bool is_game_started;
extern bool running;

// 游戏开始按钮类，继承自Button基类
class StartButton : public Button {
public:
	// 构造函数，调用父类构造函数来初始化按钮
	StartButton(RECT rect, LPCTSTR path_img_idle, LPCTSTR path_img_hover, LPCTSTR path_img_pressed)
		: Button(rect, path_img_idle, path_img_hover, path_img_pressed) {
	}

protected:
	// 必须实现父类的纯虚函数来处理点击事件
	void OnClick() override {
		is_game_started = true; // 设置游戏开始标志为true
		mciSendString(_T("play bgm repeat"), NULL, 0, NULL); // 循环播放音乐
	}

};



// 游戏结束按钮类，继承自Button基类
class EndButton : public Button {
public:
	EndButton(RECT rect, LPCTSTR path_img_idle, LPCTSTR path_img_hover, LPCTSTR path_img_pressed)
		: Button(rect, path_img_idle, path_img_hover, path_img_pressed) {
	}

protected:
	void OnClick() override {
		running = false;
	}

};
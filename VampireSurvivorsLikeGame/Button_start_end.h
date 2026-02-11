#pragma once

#include "Button.h"


class StartButton : public Button {
public:
	// 构造函数，调用父类构造函数来初始化按钮
	StartButton(RECT rect, LPCTSTR path_img_idle, LPCTSTR path_img_hover, LPCTSTR path_img_pressed)
		: Button(rect, path_img_idle, path_img_hover, path_img_pressed) {
	}

private:
	// 必须实现父类的纯虚函数来处理点击事件
	void OnClick() override {
		
	}

};


class EndButton : public Button {
public:


private:


};
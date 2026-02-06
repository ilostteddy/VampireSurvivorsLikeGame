#include <graphics.h>
#include <iostream>
#include <string>

const int Player_Anim_Num = 6; // 定义一个动画帧总数常量
int idx_current_anim = 0;  // 定义一个全局变量来储存当前动画的索引帧

int main() {
	HWND test = initgraph(1280, 720);
	SetWindowTextW(test, L"类吸血鬼");

	bool running = true;
	ExMessage msg;

	IMAGE img_background;
	loadimage(&img_background, L"assets/img/background.png");

	BeginBatchDraw(); // 批量绘制，减少闪烁

	// 0.主循环
	while (running) {
		DWORD start_time = GetTickCount();

		// 1.处理消息
		while (peekmessage(&msg)) {

		}


		// 更新动画帧
		static int counter = 0; // 定义一个循环内 static 局部变量来记录当前一共播放了几个游戏帧
		if ((++counter % 5) == 0) { // 实现每5个游戏帧切换一个动画帧
			idx_current_anim++;    // 考虑动画帧序列结束后的行为
		}
		idx_current_anim = idx_current_anim % Player_Anim_Num;  // 循环播放动画帧


		// 2.游戏运行时绘制
		if (running) {
			cleardevice(); // 清屏
			putimage(0, 0, &img_background); // 绘制背景	
			FlushBatchDraw(); // 刷新绘制
		}

		// 3.控制帧率
		DWORD end_time = GetTickCount();
		DWORD delta_time = end_time - start_time;
		if (delta_time < (1000 / 60)) {
			Sleep((1000 / 60) - delta_time);
		}
	}

	// 结束
	EndBatchDraw(); // 游戏结束，清理资源
	closegraph(); // 关闭图形窗口

	return 0;
}
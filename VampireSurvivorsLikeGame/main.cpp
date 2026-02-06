#include <graphics.h>
#include <iostream>
#include <string>

int main () {
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
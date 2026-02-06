#include <graphics.h>
#include <iostream>
#include <string>

int main () {
	HWND test = initgraph(600, 650);
	SetWindowTextW(test, L"类吸血鬼");

	bool running = true;
	ExMessage msg;

	BeginBatchDraw(); // 批量绘制，减少闪烁

	while (running) {
		DWORD start_time = GetTickCount();




		DWORD end_time = GetTickCount();
		DWORD delta_time = end_time - start_time;
		if (delta_time < (1000 / 60)) {
			Sleep((1000 / 60) - delta_time);
		}
		cleardevice(); // 清屏
		FlushBatchDraw(); // 刷新绘制
	}


	EndBatchDraw(); // 游戏结束，清理资源
	closegraph(); // 关闭图形窗口

	return 0;
}
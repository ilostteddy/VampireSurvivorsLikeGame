#include <graphics.h>
#include <iostream>
#include <string>
#include "Animation.h"
#include "Player.h"

int main() {
	Player player; // 创建玩家对象

	int screen_width = 1280;
	int screen_height = 720;

	HWND test = initgraph(screen_width, screen_height);

	SetWindowTextW(test, L"类吸血鬼");

	bool running = true; //主循环控制变量

	ExMessage msg; // 消息结构体，用于存储输入消息

	IMAGE img_background; // 背景图片
	loadimage(&img_background, L"assets/img/background.png");

	BeginBatchDraw(); // 批量绘制，减少闪烁

	DWORD last_tick = GetTickCount(); // 初始化上一帧时间

	// 0.主循环
	while (running) {
		DWORD current_tick = GetTickCount(); //  delta (当前时间 - 上一帧的时间)
		int delta = (int)(current_tick - last_tick);
		last_tick = current_tick; // 更新上一帧时间给下一次循环用
		DWORD start_time = GetTickCount(); //sleep用，控制帧率

		// 1.处理消息
		while (peekmessage(&msg)) {
			if (player.ProcessInput(msg)) {   // 处理玩家输入
				running = false;
			}; 
		}

		// 玩家移动逻辑
		player.Move(screen_width, screen_height);

		// 2.游戏运行时绘制
		if (running) {
			cleardevice();
			putimage(0, 0, &img_background);

			player.drawPlayer(delta);

			FlushBatchDraw();
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


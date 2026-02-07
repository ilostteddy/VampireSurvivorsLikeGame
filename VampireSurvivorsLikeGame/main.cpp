#include <graphics.h>
#include <iostream>
#include <string>
#include "Animation.h"

const int Player_Speed = 10; // 定义一个玩家移动速度常量
POINT player_pos = {500, 500}; // 定义一个全局变量来储存玩家的位置

// 通过Animation类的构造函数来把动画写入对象
Animation anim_left_player{ L"assets/img/player_left_%d.png", 6, 45 };
Animation anim_right_player{ L"assets/img/player_right_%d.png", 6, 45 };

// 绘制玩家动画
void drawPlayer(int delta, int dir_x) {
	static bool facing_left = false; // 定义一个静态变量来记录玩家当前的朝向，默认向右
	if (dir_x < 0)
		facing_left = true;
	else if (dir_x >0)
		facing_left = false;

	if (facing_left)
		anim_left_player.Play(player_pos.x, player_pos.y, delta);
	else
		anim_right_player.Play(player_pos.x, player_pos.y, delta);
}


int main() {
	HWND test = initgraph(1280, 720);
	SetWindowTextW(test, L"类吸血鬼");

	bool running = true;
	ExMessage msg;

	IMAGE img_background; // 背景图片
	IMAGE img_shadow;     // 背景图片
	loadimage(&img_background, L"assets/img/background.png");
	loadimage(&img_shadow, L"assets/img/shadow_player.png");

	BeginBatchDraw(); // 批量绘制，减少闪烁

	bool is_move_up = false;
	bool is_move_down = false;
	bool is_move_left = false;
	bool is_move_right = false;

	bool is_facing_right = true; // 记录角色朝向，默认向右

	DWORD last_tick = GetTickCount(); // 初始化上一帧时间

	// 0.主循环
	while (running) {
		//  delta (当前时间 - 上一帧的时间)
		DWORD current_tick = GetTickCount();
		int delta = (int)(current_tick - last_tick);
		last_tick = current_tick; // 更新上一帧时间给下一次循环用

		DWORD start_time = GetTickCount();

		// 1.处理消息
		while (peekmessage(&msg)) {
			if (msg.message == WM_KEYDOWN) {
				switch (msg.vkcode) {
				case VK_ESCAPE:
					running = false; // 按下 ESC 键退出游戏
					break;
				case 'A':
					is_move_left = true;
					break;
				case 'D':
					is_move_right = true;
					break;
				case 'W':
					is_move_up = true;
					break;
				case 'S':
					is_move_down = true;
					break;
				}
			}
			else if (msg.message == WM_KEYUP) {
				switch (msg.vkcode) {
					case 'A':
						is_move_left = false;
						break;
					case 'D':
						is_move_right = false;
						break;
					case 'W':
						is_move_up = false;
						break;
					case 'S':
						is_move_down = false;
						break;
				}
			}
			
		}

		if (is_move_left)  player_pos.x -= Player_Speed;
		if (is_move_right) player_pos.x += Player_Speed;
		if (is_move_up)    player_pos.y -= Player_Speed;
		if (is_move_down)  player_pos.y += Player_Speed;


		// 2.游戏运行时绘制
		if (running) {
			cleardevice();
			putimage(0, 0, &img_background);

			// 确定移动方向
			int dir_x = 0;
			if (is_move_left) dir_x = -1;
			else if (is_move_right) dir_x = 1;

			// 关键调用：传入时间增量和方向
			drawPlayer(delta, dir_x);

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
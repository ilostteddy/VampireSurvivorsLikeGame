#include <graphics.h>
#include <iostream>
#include <string>
#include "Animation.h"

const int Player_Speed = 10; // 定义一个玩家移动速度常量
POINT player_pos = {500, 500}; // 定义一个全局变量来储存玩家的位置


// 通过Animation类的构造函数来把动画写入对象
Animation anim_left_player{ L"assets/img/player_left_%d.png", 6, 80 };
Animation anim_right_player{ L"assets/img/player_right_%d.png", 6, 80 };


IMAGE img_background; // 背景图片
IMAGE img_shadow;     // 人物阴影图片

int screen_width = 1280;
int screen_height = 720;
const int PLAYER_WIDTH = 80;  // 玩家宽度
const int PLAYER_HEIGHT = 80;  // 玩家高度
const int SHADOW_WIDTH = 32; // 阴影宽度 


// 绘制玩家动画
void drawPlayer(int delta, int dir_x) {
	int shadow_x = player_pos.x + (PLAYER_WIDTH - SHADOW_WIDTH) / 2;
	int shadow_y = player_pos.y + PLAYER_HEIGHT - 8;
	putimage_alpha(shadow_x, shadow_y, &img_shadow);

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
	HWND test = initgraph(screen_width, screen_height);
	SetWindowTextW(test, L"类吸血鬼");

	bool running = true;
	ExMessage msg;

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


		// 玩家移动逻辑
		// 01. 获取移动意图（简单用dir_x和dir_y的正负表示）
		int dir_x = is_move_right - is_move_left; // 右移 +1，左移 -1
		int dir_y = is_move_down - is_move_up;    // 下移 +1，上移 -1

		// 02. 计算方向向量的长度
		double length = sqrt(dir_x * dir_x + dir_y * dir_y);

		// 03. 只要长度不为 0（即玩家按了键），就进行标准化处理
		if (length > 0) {
			// 归一化：将分量除以总长度
			// 例如斜向移动时，dx=1, dy=1, length=1.414
			// 则 nx = 1/1.414 = 0.707
			double normalized_x = dir_x / length;
			double normalized_y = dir_y / length;

			// 4. 应用速度：位移 = 方向分量 * 速度标量
			player_pos.x += (int)(normalized_x * Player_Speed);
			player_pos.y += (int)(normalized_y * Player_Speed);
		}

		// 04. 边界检测，确保玩家不会移出屏幕
		if (player_pos.x < 0) player_pos.x = 0;
		if (player_pos.y < 0) player_pos.y = 0;
		if (player_pos.x + PLAYER_WIDTH > screen_width) player_pos.x = screen_width - PLAYER_WIDTH;
		if (player_pos.y + PLAYER_HEIGHT > screen_height) player_pos.y = screen_height - PLAYER_HEIGHT;



		// 2.游戏运行时绘制
		if (running) {
			cleardevice();
			putimage(0, 0, &img_background);


			// 关键调用：传入时间增量和方向
			drawPlayer(delta, is_move_right - is_move_left);

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
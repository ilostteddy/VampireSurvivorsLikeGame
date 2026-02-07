#include <graphics.h>
#include <iostream>
#include <string>

const int Player_Speed = 10; // 定义一个玩家移动速度常量
const int Player_Anim_Num = 6; // 定义一个动画帧总数常量
int idx_current_anim = 0;  // 定义一个全局变量来储存当前动画的索引帧
POINT player_pos = { 500, 500 }; // 定义一个全局变量来储存玩家的位置

IMAGE img_player_left[Player_Anim_Num]; // 定义一个数组来储存玩家的所有动画帧
IMAGE img_player_right[Player_Anim_Num];

// 把图片数组通过循环写入文件
void LoadAnimation() {
	for (int i = 0; i < Player_Anim_Num; i++) {
		std::wstring path = L"assets/img/player_left_" + std::to_wstring(i) + L".png";
		loadimage(&img_player_left[i], path.c_str());
	}
	for (int i = 0; i < Player_Anim_Num; i++) {
		std::wstring path = L"assets/img/player_right_" + std::to_wstring(i) + L".png";
		loadimage(&img_player_right[i], path.c_str());
	}
}

// 封装一个putimage_alpha函数，用于绘制带透明通道的图片
#pragma comment(lib, "Msimg32.lib")
inline void putimage_alpha(int x, int y, IMAGE* img) {
	int w = img->getwidth();
	int h = img->getheight();
	AlphaBlend(GetImageHDC(NULL), x, y, w, h,
		GetImageHDC(img), 0, 0, w, h, { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA });
}

int main() {
	HWND test = initgraph(1280, 720);
	SetWindowTextW(test, L"类吸血鬼");

	bool running = true;
	ExMessage msg;

	IMAGE img_background; // 背景图片
	loadimage(&img_background, L"assets/img/background.png");

	BeginBatchDraw(); // 批量绘制，减少闪烁

	LoadAnimation(); // 加载玩家动画帧

	bool is_move_up = false;
	bool is_move_down = false;
	bool is_move_left = false;
	bool is_move_right = false;

	// 0.主循环
	while (running) {

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

		if (is_move_left) player_pos.x -= Player_Speed;
		if (is_move_right) player_pos.x += Player_Speed;
		if (is_move_up) player_pos.y -= Player_Speed;
		if (is_move_down) player_pos.y += Player_Speed;


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
			putimage_alpha(player_pos.x, player_pos.y, &img_player_right[idx_current_anim]);

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
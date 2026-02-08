#pragma once

#include <graphics.h>
#include "Animation.h"
#include <cmath>

class Player {
public:
	Player() {
		loadimage(&img_shadow, L"assets/img/shadow_player.png");
		// new函数返回对应类型的指针，同时避免拷贝
		anim_left = new Animation{ L"assets/img/player_left_%d.png", 6, 80 };
		anim_right = new Animation{ L"assets/img/player_right_%d.png", 6, 80 };
	}

	~Player() {
		delete anim_left;
		delete anim_right;
	}

	// 处理玩家键盘消息 
	bool ProcessInput(const ExMessage& msg) {
		if (msg.message == WM_KEYDOWN) {
			switch (msg.vkcode) {
			case VK_ESCAPE:     // 检测到 ESC
				return true;    // 返回 true，告诉外面：玩家想退出
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
		return false; // 默认返回 false，表示玩家没有想退出
	}

	// 玩家移动逻辑
	void Move(const int &screen_width, const int &screen_height) {
		// 01. 获取移动意图（简单用dir_x和dir_y的正负表示）
		int dir_x = is_move_right - is_move_left; // 右移 +1，左移 -1
		int dir_y = is_move_down - is_move_up;    // 下移 +1，上移 -1

		// 更新玩家朝向
		if (dir_x < 0)
			facing_left = true;
		else if (dir_x > 0)
			facing_left = false;

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
	}


	// 绘制玩家动画
	void drawPlayer(int delta) {
		// 绘制阴影
		int shadow_x = player_pos.x + (PLAYER_WIDTH - SHADOW_WIDTH) / 2;
		int shadow_y = player_pos.y + PLAYER_HEIGHT - 8;
		putimage_alpha(shadow_x, shadow_y, &img_shadow);

		// 根据朝向播放不同的动画
		if (facing_left)
			// anim_left->Play 等价于 （*anim_left).Play
			anim_left->Play(player_pos.x, player_pos.y, delta);
		else
			anim_right->Play(player_pos.x, player_pos.y, delta);
	}


private:
	const int Player_Speed = 10; // 定义一个玩家移动速度常量
	const int PLAYER_WIDTH = 80;  // 玩家宽度
	const int PLAYER_HEIGHT = 80;  // 玩家高度
	const int SHADOW_WIDTH = 32; // 阴影宽度 
	bool facing_left = false; // 玩家朝向，默认向右

	POINT player_pos = { 500, 500 }; // 定义一个全局变量来储存玩家的位置

	IMAGE img_shadow;     // 人物阴影图片
	ExMessage msg; // 消息结构体，用于存储输入消息
	Animation* anim_left;  // 向左走动画
	Animation* anim_right; // 向右走动画

	// 运动方向判断用
	bool is_move_up = false;
	bool is_move_down = false;
	bool is_move_left = false;
	bool is_move_right = false;
};
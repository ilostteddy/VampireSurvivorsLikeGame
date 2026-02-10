#pragma once

#include <graphics.h>
#include <windows.h>
#include "Animation.h"
#include "Player.h"
#include "Bullet.h"

class Enemy {
public:
	Enemy () {
		loadimage(&img_shadow, L"assets/img/shadow_enemy.png");
		// new函数返回对应类型的指针，同时避免拷贝
		anim_left = new Animation{ L"assets/img/enemy_left_%d.png", 6, 80 };
		anim_right = new Animation{ L"assets/img/enemy_right_%d.png", 6, 80 };

		// 定义边界
		enum class SpawnEdge {  //enum class强枚举
			Up = 0,
			Down,
			Left,
			Right
		};

		// 随机选择一个边界确定敌人出生位置
		SpawnEdge edge = static_cast<SpawnEdge>(rand() % 4);
		switch (edge) {
		case SpawnEdge::Up:
			Enemy_pos.x = rand() % (screen_width - ENEMY_WIDTH);
			Enemy_pos.y = -ENEMY_HEIGHT;
			break;
		case SpawnEdge::Down:
			Enemy_pos.x = rand() % (screen_width - ENEMY_WIDTH);
			Enemy_pos.y = screen_height;
			break;
		case SpawnEdge::Left:
			Enemy_pos.x = -ENEMY_WIDTH;
			Enemy_pos.y = rand() % (screen_height - ENEMY_HEIGHT);
			break;
		case SpawnEdge::Right:
			Enemy_pos.x = screen_width;
			Enemy_pos.y = rand() % (screen_height - ENEMY_HEIGHT);
			break;
		}
	}


	~Enemy () {
		delete anim_left;
		delete anim_right;
	}


	// 检测与子弹碰撞的逻辑
	bool CheckCollisionWithBullet(const Bullet& bullet) {
		bool is_overlap_x = (bullet.position.x >= Enemy_pos.x) && (bullet.position.x <= Enemy_pos.x + ENEMY_WIDTH);
		bool is_overlap_y = (bullet.position.y >= Enemy_pos.y) && (bullet.position.y <= Enemy_pos.y + ENEMY_HEIGHT);
		return is_overlap_x && is_overlap_y;
	}


	// 检测与玩家碰撞的逻辑
	bool CheckCollisionWithPlayer(const Player& player) {
		// 宽松化检测，敌人抽象为一个点
		POINT enemy_position = { Enemy_pos.x + ENEMY_WIDTH/2, Enemy_pos.y + ENEMY_HEIGHT/2 };
		bool is_overlap_x = (enemy_position.x >= player.getPosition().x) && (enemy_position.x <= player.getPosition().x + player.PLAYER_WIDTH);
		bool is_overlap_y = (enemy_position.y >= player.getPosition().y) && (enemy_position.y <= player.getPosition().y + player.PLAYER_HEIGHT);
		return is_overlap_x && is_overlap_y;
	}
	

	// 敌人移动逻辑
	void Enemy_Move(const Player& player) {
		// 建立了一个临时的“标签（别名）”指向了 player 对象内部存坐标的那块物理内存。
		const POINT& player_position = player.getPosition();  
		
		// 这步主要是用来求两个向量之间的距离和方向
		int dir_x = player_position.x - Enemy_pos.x;
		int dir_y = player_position.y - Enemy_pos.y;
		double length = sqrt(dir_x * dir_x + dir_y * dir_y);

		// 更新敌人朝向
		if (dir_x > 0) 
			facing_left = false;
		else if (dir_x < 0)
			facing_left = true;

		// 归一化运算再加上速度应用
		if (length > 0) {
			double normalized_x = dir_x / length;
			double normalized_y = dir_y / length;
			Enemy_pos.x += (int)(normalized_x * Enemy_Speed);
			Enemy_pos.y += (int)(normalized_y * Enemy_Speed);
		}

	}


	// 绘制敌人动画
	void enemy_Draw(int delta) {
		// 绘制阴影
		int shadow_x = Enemy_pos.x + (ENEMY_WIDTH - SHADOW_WIDTH) / 2 - 5 ;
		int shadow_y = Enemy_pos.y + ENEMY_HEIGHT - 30;
		putimage_alpha(shadow_x, shadow_y, &img_shadow);
		// 根据朝向播放不同的动画
		if (facing_left)
			// anim_left->Play 等价于 （*anim_left).Play
			anim_left->Play(Enemy_pos.x, Enemy_pos.y, delta);
		else
			anim_right->Play(Enemy_pos.x, Enemy_pos.y, delta);
	}


	// 与子弹碰撞后CheckCollisionWithBullet为true时候调用
	void getHurt() {
		is_alive = false;
	}


	// 存活检测
	bool CheckAlive() const {
		return is_alive;
	}


private:
	bool is_alive = true; // 敌人存活状态

	int screen_width = 1280;
	int screen_height = 720;

	const int Enemy_Speed = 3; // 定义一个敌人移动速度常量，不能为1否则转为int后数值为0
	const int ENEMY_WIDTH = 80;  // 敌人宽度
	const int ENEMY_HEIGHT = 80;  // 敌人高度
	const int SHADOW_WIDTH = 32; // 阴影宽度 
	bool facing_left = false; // 敌人朝向，默认向右

	POINT Enemy_pos = { 500, 500 }; // 定义一个全局变量来储存敌人的位置

	IMAGE img_shadow;     // 阴影图片
	Animation* anim_left;  // 向左走动画
	Animation* anim_right; // 向右走

};
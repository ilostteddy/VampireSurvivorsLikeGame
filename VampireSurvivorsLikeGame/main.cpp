#include <graphics.h>
#include <iostream>
#include <string>
#include "Animation.h"
#include "Player.h"
#include "Enemy.h"

// 生成新敌人
void TryGenerateEnemy(std::vector<Enemy*>& enemies) {
	const int interval = 50; // 生成敌人的间隔
	static int counter = 0; // 静态变量用于计数
	if ((++counter) % interval == 0)
		enemies.push_back(new Enemy());
}


// 子弹跟随玩家
void UpdateBulletsPosition(const Player& player, std::vector<Bullet>& bullets) {
	const double RADIAL_SPEED = 0.005;// 子弹围绕玩家旋转的速度
	const double TANGENT_SPEED = 0.005;// 子弹切线方向的速度
	double radian_interval = (2 * 3.1415926) / bullets.size(); // 子弹之间的角度间隔
	double radius = 100 + 25 * sin(GetTickCount() * RADIAL_SPEED); // 子弹距离玩家时远时近的波动效果
	POINT player_pos = { player.getPosition().x + player.PLAYER_WIDTH / 2, player.getPosition().y + player.PLAYER_HEIGHT / 2 };

	for (size_t i = 0; i < bullets.size(); i++) {
		double angle = GetTickCount() * TANGENT_SPEED + radian_interval * i; // 计算当前子弹的角度
		bullets[i].position.x = player_pos.x + (int)(radius * cos(angle));
		bullets[i].position.y = player_pos.y + (int)(radius * sin(angle));
	}
}

int main() {

	Player player; // 创建玩家对象

	std::vector<Enemy*> enemies; // 创建敌人对象的容器 
	std::vector<Bullet> bullets(3); // 创建子弹对象的容器，预留给不同类型的子弹，等效于Bullet bullets[3]

	// 窗口部分
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



		// 2.处理数据
		player.Move(screen_width, screen_height); // 玩家移动
		UpdateBulletsPosition(player, bullets); // 子弹跟随玩家
		TryGenerateEnemy(enemies); //敌人生成

		for (Enemy* enemy : enemies)
			enemy->Enemy_Move(player); //敌人移动

		for (Enemy* enemy : enemies) { //检测敌人和玩家碰撞
			if (enemy->CheckCollisionWithPlayer(player)) {
				MessageBox(GetHWnd(), L"Game Over", L"You have been hit!", MB_OK);
				running = false; // 碰撞后结束游戏
				break;
			}
		}

		for (Enemy* enemy : enemies) { // 检测敌人和子弹碰撞
			for (const Bullet& bullet : bullets) {
				if (enemy->CheckCollisionWithBullet(bullet)) {
					// enemy->is_alive = false; // 碰撞后标记敌人为死亡状态
				}
			}

		}

		// 3.渲染
		if (running) {
			cleardevice();
			putimage(0, 0, &img_background);

			player.drawPlayer(delta);

			for (Enemy* enemy : enemies)
				enemy->enemy_Draw(delta);

			for (const Bullet& bullet : bullets) {
				bullet.Draw();
			}

			FlushBatchDraw();
		}



		// 4.控制帧率
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


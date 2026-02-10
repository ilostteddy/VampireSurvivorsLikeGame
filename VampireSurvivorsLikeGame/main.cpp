#include <graphics.h>
#include <iostream>
#include <string>
#include "Animation.h"
#include "Player.h"
#include "Enemy.h"
#include <mmsystem.h>

#pragma comment(lib, "WinMM.lib") // 链接多媒体库

// 加载动画序列帧资源
Atlas* atlas_player_left;
Atlas* atlas_player_right;
Atlas* atlas_enemy_left;
Atlas* atlas_enemy_right;


// 生成新敌人
void TryGenerateEnemy(std::vector<Enemy*>& enemies) { 
	const int interval = 50; // 生成敌人的间隔
	static int counter = 0; // 静态变量用于计数
	if ((++counter) % interval == 0)
		enemies.push_back(new Enemy(atlas_enemy_left, atlas_enemy_right));
}


// 子弹跟随玩家
void UpdateBulletsPosition(const Player& player, std::vector<Bullet>& bullets) {
	const double RADIAL_SPEED = 0.005;// 子弹围绕玩家旋转的速度
	const double TANGENT_SPEED = 0.005;// 子弹切线方向的速度
	double radian_interval = (2 * 3.1415926) / bullets.size(); // 子弹之间的角度间隔
	double radius = 100 + 25 * sin(GetTickCount() * RADIAL_SPEED); // 子弹距离玩家时远时近的波动效果
	POINT player_pos = { player.getPosition().x + player.PLAYER_WIDTH / 2, player.getPosition().y + player.PLAYER_HEIGHT / 2 };

	for (size_t i = 0; i < bullets.size(); i++) {
		double angle = GetTickCount() * TANGENT_SPEED + radian_interval * i; // 计算当前子弹的角度，radian_interval * i做子弹间隔
		bullets[i].position.x = player_pos.x + (int)(radius * cos(angle));
		bullets[i].position.y = player_pos.y + (int)(radius * sin(angle));
	}
}

// 绘制玩家得分
void DrawPlayerScore(int score, int x, int y, int height = 20, LPCTSTR font = _T("Consolas")) {
	static TCHAR text[32];
	_stprintf_s(text, _T("得分: %d"), score);

	// 1. 设置背景透明
	setbkmode(TRANSPARENT);

	// 2. 设置字体样式
	// 出处：EasyX <graphics.h>
	// 参数说明：(高度, 宽度(0表示自适应), 字体名)
	settextstyle(height, 0, font);

	// 3. 设置文字颜色
	settextcolor(WHITE);

	// 4. 指定位置输出
	outtextxy(x, y, text);
}



int main() {
	// 按照逻辑，atlas资源应该在游戏初始化时加载，否则player和enemy对象无法正确创建
	atlas_player_left = new Atlas(_T("assets/img/player_left_%d.png"), 6);
	atlas_player_right = new Atlas(_T("assets/img/player_right_%d.png"), 6);
	atlas_enemy_left = new Atlas(_T("assets/img/enemy_left_%d.png"), 6);
	atlas_enemy_right = new Atlas(_T("assets/img/enemy_right_%d.png"), 6);

	Player player(atlas_player_left, atlas_player_right); // 创建玩家对象

	mciSendString(_T("open assets/mus/bgm.mp3 alias bgm"), NULL, 0, NULL); // 加载目录中的音乐并取名bgm
	mciSendString(_T("open assets/mus/hit.wav alias hit"), NULL, 0, NULL); // 加载目录中的音乐并取名hit

	mciSendString(_T("play bgm repeat"), NULL, 0, NULL); // 循环播放音乐

	static int enemy_killed_count = 0;

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
				static TCHAR text[128];
				_stprintf_s(text, _T("最终得分： %d !"), enemy_killed_count);
				MessageBox(GetHWnd(), text, _T("Game Over!"), MB_OK);
				running = false; // 碰撞后结束游戏
				break;
			}
		}

		for (Enemy* enemy : enemies) { // 检测敌人和子弹碰撞
			for (const Bullet& bullet : bullets) {
				if (enemy->CheckCollisionWithBullet(bullet)) {
					enemy->getHurt(); // 敌人受伤
					enemy_killed_count++;
					mciSendString(_T("play hit from 0"), NULL, 0, NULL); // 从头播放击中音效
				}
			}
		}


		// 移除已死亡的敌人 
		for (size_t i = 0; i < enemies.size(); i++) {
			Enemy* enemy = enemies[i];
			if (!enemy->CheckAlive()) {
				std::swap(enemies[i], enemies.back());
				enemies.pop_back();
				delete enemy; // 重新检查当前索引
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
			
			DrawPlayerScore(enemy_killed_count, 30, 30, 40);

			FlushBatchDraw();
		}



		// 4.控制帧率
		DWORD end_time = GetTickCount();
		DWORD delta_time = end_time - start_time;
		if (delta_time < (1000 / 60)) {
			Sleep((1000 / 60) - delta_time);
		}
	}

	// 释放atlas指针
	delete atlas_player_left;
	delete atlas_player_right;
	delete atlas_enemy_left;
	delete atlas_enemy_right;

	// 结束
	EndBatchDraw(); // 游戏结束，清理资源
	closegraph(); // 关闭图形窗口

	return 0;
}


#include <graphics.h>
#include <iostream>
#include <string>
#include <mmsystem.h>
#pragma region

bool is_game_started = false; // 游戏是否开始的标志变量
bool running = true; //主循环控制变量

#pragma comment(lib, "WinMM.lib") // 链接多媒体库
#pragma comment(lib, "Msimg32.lib") // 封装一个putimage_alpha函数，用于绘制带透明通道的图片

// inline关键字防止重定义
inline void putimage_alpha(int x, int y, IMAGE* img) {
	int w = img->getwidth();
	int h = img->getheight();
	AlphaBlend(GetImageHDC(NULL), x, y, w, h,
		GetImageHDC(img), 0, 0, w, h, { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA });
}

#include "Animation.h"
#include "Player.h"
#include "Enemy.h"
#include "Button.h"
#include "Button_start_end.h"


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
	// 音乐加载部分
	mciSendString(_T("open assets/mus/bgm.mp3 alias bgm"), NULL, 0, NULL); // 加载目录中的音乐并取名bgm
	mciSendString(_T("open assets/mus/hit.wav alias hit"), NULL, 0, NULL); // 加载目录中的音乐并取名hit

	// 动画序列帧加载部分
	// 按照逻辑，atlas资源应该在游戏初始化时加载，否则player和enemy对象无法正确创建
	atlas_player_left = new Atlas(_T("assets/img/player_left_%d.png"), 6);
	atlas_player_right = new Atlas(_T("assets/img/player_right_%d.png"), 6);
	atlas_enemy_left = new Atlas(_T("assets/img/enemy_left_%d.png"), 6);
	atlas_enemy_right = new Atlas(_T("assets/img/enemy_right_%d.png"), 6);

	// 主要游戏对象创建部分
	Player player(atlas_player_left, atlas_player_right); // 创建玩家对象
	std::vector<Enemy*> enemies; // 创建敌人对象的容器 
	std::vector<Bullet> bullets(3); // 创建子弹对象的容器，预留给不同类型的子弹，等效于Bullet bullets[3]

	// 游戏状态变量
	static int enemy_killed_count = 0;
	ExMessage msg; // 消息结构体，用于存储输入消息
	DWORD last_tick = GetTickCount(); // 初始化上一帧时间

	// 窗口部分
	int screen_width = 1280;
	int screen_height = 720;
	HWND test = initgraph(screen_width, screen_height);
	SetWindowTextW(test, L"类吸血鬼");


	// ui按钮部分
	int Button_Width = 200;
	int Button_Height = 100;

	RECT region_start_button; // 定义开始按钮的区域，后续可以用来检测鼠标点击事件是否在这个区域内，从而触发游戏开始的逻辑
	RECT region_end_button; // 定义结束按钮的区域，后续可以用来检测鼠标点击事件是否在这个区域内，从而触发游戏结束的逻辑

	region_start_button.left = (screen_width - Button_Width) / 2;
	region_start_button.right = region_start_button.left + Button_Width;
	region_start_button.top = 430;
	region_start_button.bottom = region_start_button.top + Button_Height;

	region_end_button.left = (screen_width - Button_Width) / 2;
	region_end_button.right = region_end_button.left + Button_Width;
	region_end_button.top = 580;
	region_end_button.bottom = region_end_button.top + Button_Height;

	StartButton start_button{ region_start_button, _T("assets/img/ui_start_idle.png"), _T("assets/img/ui_start_hovered.png"), _T("assets/img/ui_start_pushed.png") };
	EndButton end_button{ region_end_button, _T("assets/img/ui_quit_idle.png"), _T("assets/img/ui_quit_hovered.png"), _T("ui_quit_pushed.png") };


	// 画面部分
	IMAGE img_background; // 背景图片
	loadimage(&img_background, L"assets/img/background.png");
	IMAGE img_menu; // 主界面图片
	loadimage(&img_menu, L"assets/img/menu.png");

	BeginBatchDraw(); // 批量绘制，减少闪烁


	// 0.主循环
	while (running) {
		DWORD current_tick = GetTickCount(); //  delta (当前时间 - 上一帧的时间)
		int delta = (int)(current_tick - last_tick);
		last_tick = current_tick; // 更新上一帧时间给下一次循环用
		DWORD start_time = GetTickCount(); //sleep用，控制帧率

		// 1.处理消息
		while (peekmessage(&msg)) {
			if (is_game_started) {
				if (player.ProcessInput(msg)) {
				running = false;
				}
			}
			else {
				start_button.ProcessMouseEvent(msg);
				end_button.ProcessMouseEvent(msg);
			}
		}


		// 2.处理数据
		if (is_game_started) {
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
		}

		// 3.渲染
		if (running) {

			cleardevice();

			if (is_game_started) {

				putimage(0, 0, &img_background);

				player.drawPlayer(delta);

				for (Enemy* enemy : enemies)
					enemy->enemy_Draw(delta);

				for (const Bullet& bullet : bullets)
					bullet.Draw();

				DrawPlayerScore(enemy_killed_count, 30, 30, 40);
			}

			else {
				putimage(0, 0, &img_menu);
				start_button.button_Draw();
				end_button.button_Draw();
			}
		}
		FlushBatchDraw();
	

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
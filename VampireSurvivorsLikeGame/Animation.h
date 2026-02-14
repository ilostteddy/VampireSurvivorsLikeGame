#pragma once

#include <vector>
#include <graphics.h>
#include "Atlas.h"

class Animation {

public:
	Animation(Atlas* atlas, int intervel) {
		intervel_ms = intervel; // 将传入的帧间隔时间赋值给成员变量interval_ms
		anim_atlas = atlas; // 将传入的Atlas对象指针赋值给成员变量anim_atlas
	}

	// 用Atlas是Animation之间共享的公共资产，所以绝对不能在Animation的析构函数中用delete来释放Atlas指针
	~Animation() = default;

	// delta参数来表示距离上一次调用Play函数时间过去了多久
	void Play(int x, int y, int delta) {
		timer += delta;
		if (timer >= intervel_ms) { // 如果计时器达到或超过帧间隔时间
			idx_frame = (idx_frame + 1) % anim_atlas->frame_list.size(); // 切换到下一帧，循环播放
			timer = 0; // 重置计时器
		}
		putimage_alpha(x, y, anim_atlas->frame_list[idx_frame]);
	}



private:
	int timer = 0; // 定义一个计时器成员变量，用于跟踪动画时间	
	int idx_frame = 0; // 定义一个索引成员变量，用于跟踪当前动画帧
	int intervel_ms = 45;

	Atlas* anim_atlas; // 定义一个指向Atlas对象的指针成员变量，用于存储动画帧资源
};


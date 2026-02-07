#pragma once

#include <vector>
#include <graphics.h>

class Animation {

public:
	Animation(LPCTSTR image_path, int num, int intervel) {
		intervel_ms = intervel;  // 初始化帧间隔成员变量
		TCHAR path_file[256];	// 定义字符数组，用于拼接每帧的完整路径（256是缓冲区大小，避免溢出）

		// 遍历循环加载每一帧的图片到vector<IMAGE*> frame_list
		for (size_t i = 0; i < num; i++) {	// 循环加载 num 张动画帧图片
			_stprintf_s(path_file, image_path, i);
			IMAGE* img = new IMAGE(); // 动态创建 IMAGE 对象（EasyX 的图片对象）
			loadimage(img, path_file); //加载图片到新建的 IMAGE 对象中
			frame_list.push_back(img); // 将加载的图片指针添加到vector<IMAGE*> frame_list;
		}
	}

	// 用指针注意不要忘记释放内存，每个new对应一个delete
	~Animation() {
		for (size_t i = 0; i < frame_list.size(); i++)
			delete frame_list[i]; // 释放每个 IMAGE 对象的内存
	};


	// delta参数来表示距离上一次调用Play函数时间过去了多久
	void Play(int x, int y, int delta) {
		timer += delta;
		if (timer >= intervel_ms) { // 如果计时器达到或超过帧间隔时间
			idx_frame = (idx_frame + 1) % frame_list.size(); // 切换到下一帧，循环播放
			timer = 0; // 重置计时器
		}
		putimage_alpha(x, y, frame_list[idx_frame]);
	}

private:
	int timer = 0; // 定义一个计时器成员变量，用于跟踪动画时间	
	int idx_frame = 0; // 定义一个索引成员变量，用于跟踪当前动画帧
	std::vector<IMAGE*> frame_list;
	int intervel_ms = 0;
};


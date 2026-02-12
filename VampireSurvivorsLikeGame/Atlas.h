#pragma once

#include <vector>
#include <graphics.h>


class Atlas {
public:
	std::vector<IMAGE*> frame_list;

	Atlas(LPCTSTR image_path, int num) {
		// 定义字符数组，用于拼接每帧的完整路径（256是缓冲区大小，避免溢出）
		TCHAR path_file[256];
		
		// 遍历循环加载每一帧的图片到vector<IMAGE*> frame_list
		for (size_t i = 0; i < num; i++) {	// 循环加载 num 张动画帧图片
			_stprintf_s(path_file, image_path, i);
			IMAGE* img = new IMAGE(); // 动态创建 IMAGE 对象（EasyX 的图片对象）
			loadimage(img, path_file); //加载图片到新建的 IMAGE 对象中
			frame_list.push_back(img); // 将加载的图片指针添加到vector<IMAGE*> frame_list;
		}
	}

	~Atlas() {
		for (size_t i = 0; i < frame_list.size(); i++)
			delete frame_list[i]; // 释放每个 IMAGE 对象的内存
	};


private:
	

};

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

	// 深拷贝构造函数（关键：基于现有Atlas创建新的Atlas，复制所有图像帧）
	Atlas(const Atlas& other) {
		for (size_t i = 0; i < other.frame_list.size(); i++) {
			IMAGE* src_img = other.frame_list[i];

			// 如果当前要拷贝的源图像指针 `src_img` 是一个 “空指针”（没有指向任何有效内存），就跳过当前这一轮循环，不处理这个无效的图像
			if (src_img == nullptr) continue;

			// 创建新的IMAGE，尺寸和原图像一致
			IMAGE* new_img = new IMAGE();
			int w = src_img->getwidth();
			int h = src_img->getheight();
			Resize(new_img, w, h);

			// 逐像素拷贝原图像到新图像（深拷贝）
			BitBlt(GetImageHDC(new_img), 0, 0, w, h,
				GetImageHDC(src_img), 0, 0, SRCCOPY);

			frame_list.push_back(new_img);
		}
	}


	// 没法直接调用，缺一个深拷贝构造函数
	void invert() {
		// 遍历每一张需要翻转的图像帧
		for (size_t i = 0; i < frame_list.size(); i++) {
			IMAGE* img = frame_list[i];
			if (img == nullptr) continue;  // 空指针保护

			// 1. 获取图像宽高
			int width = img->getwidth();
			int height = img->getheight();
			if (width <= 0 || height <= 0) continue;  // 无效图像跳过

			// 2. 创建临时IMAGE存储翻转后的像素（避免覆盖原缓冲区导致错误）
			IMAGE* temp_img = new IMAGE;
			Resize(temp_img, width, height);

			// 3. 获取原图像和临时图像的像素缓冲区指针
			DWORD* src_buffer = GetImageBuffer(img);         // 原图像缓冲区
			DWORD* dst_buffer = GetImageBuffer(temp_img);    // 翻转后临时缓冲区

			// 4. 逐行逐像素水平翻转
			for (int y = 0; y < height; y++) {  // 遍历每一行
				for (int x = 0; x < width; x++) {  // 遍历当前行的每一列
					// 原像素索引：y行x列
					int src_idx = y * width + x;
					// 翻转后像素索引：y行(width-x-1)列（核心翻转逻辑）
					int dst_idx = y * width + (width - x - 1);
					// 拷贝像素（实现水平镜像）
					dst_buffer[dst_idx] = src_buffer[src_idx];
				}
			}
			// 5. 用翻转后的图像替换原图像
			delete img; // 释放原图像内存
			frame_list[i] = temp_img; // 替换为翻转后的图像
		}
	}


	~Atlas() {
		for (size_t i = 0; i < frame_list.size(); i++)
			delete frame_list[i]; // 释放每个 IMAGE 对象的内存
	};


	 


private:
	

};

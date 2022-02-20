#pragma once
#include <string>
#include <vector>
#include <opencv2/opencv.hpp>
#include <opencv2/core/utils/logger.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/types_c.h>
#include "json/json.h"

using namespace cv;
using namespace std;

struct color {
	int r, g, b, a;
};

class Frame {
private:
	vector<color> color_table;

public:
	void SetFrame(cv::Mat frame, int accuracy);
	void SaveFrame(Json::Value *adofai, double angle_offset, int position);
	void CompareFrame(Frame *frame);
};

class Player {
private:
	double fps;
	int width, height;
	string adofai_dat;

	vector<Frame> frames;

public:
	bool SetVideoFile(string video_path, double fps, double size, int accuracy, int loop);
	bool SetData(string adofai_path, int position);
	void Save(string adofai_path);
};


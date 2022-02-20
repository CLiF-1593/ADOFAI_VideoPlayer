#include "Player.h"
#include <fstream>
#include "gotoxy.h"
#include "ColorCalc.h"

using namespace std;

int effect_cnt;

bool Player::SetVideoFile(string video_path, double fps, double size, int accuracy, int loop) {

	utils::logging::setLogLevel(cv::utils::logging::LogLevel::LOG_LEVEL_SILENT);

	effect_cnt = 0;

	VideoCapture cap(video_path.c_str());
	if (!cap.isOpened()) {
		printf("Error : Can't open the video");
		return true;
	}

	cap.set(cv::CAP_PROP_POS_FRAMES, 0);
	cv::Mat video;
	if (!cap.read(video)) {
		std::cout << "Error : Failed to extract a frame.\n";
		return true;
	}
	this->width = video.cols * size;
	this->height = video.rows * size;

	this->fps = fps;

	cout << "Video Size : " << this->width << "x" << this->height << endl;
	cout << endl << "Video Processing . . . " << endl;
	
	int video_fps = cap.get(CAP_PROP_FPS);
	double video_spf = 1000.0 / (double)video_fps;
	double target_spf = 1000.0 / (double)fps;

	double cur_running_time = 0;

	this->frames.clear();

	Frame prev_frame;

	for (int i = 0; i < loop; i++) {
		for (int frame_count = 0; frame_count < cap.get(cv::CAP_PROP_FRAME_COUNT); frame_count++) {
			if (cur_running_time > target_spf) {
				cap.set(cv::CAP_PROP_POS_FRAMES, frame_count);
				cv::Mat frame;
				if (!cap.read(frame)) {
					std::cout << "Error : Failed to extract a frame.\n";
					return true;
				}
				resize(frame, frame, Size(frame.cols * size, frame.rows * size), 0, 0, CV_INTER_AREA);
				Frame f;
				f.SetFrame(frame, accuracy);
				f.CompareFrame(&prev_frame);
				this->frames.push_back(f);
				cur_running_time -= target_spf;

				resize(frame, frame, Size(frame.cols / size / 2, frame.rows / size / 2), 0, 0, CV_INTER_AREA);
				imshow("Screen Preview", frame);
				cv::waitKey(video_spf);
			}
			cur_running_time += video_spf;
			gotoxy();
			cout << ceil((double)(i *cap.get(cv::CAP_PROP_FRAME_COUNT) + frame_count + 1) / (double)(cap.get(cv::CAP_PROP_FRAME_COUNT) * loop) * 100.0) << "%";
		}
	}
	destroyWindow("Screen Preview");
	return false;
}

Json::Value Position(int floor, int x, int y) {
	Json::Value pos;
	pos["floor"] = floor;
	pos["eventType"] = "PositionTrack";
	pos["positionOffset"][0] = x;
	pos["positionOffset"][1] = y;
	pos["editorOnly"] = "Disabled";
	return pos;
}

string debug_string(string str) {
	while (str.size() < 2) {
		str = "0" + str;
	}
	return str;
}

Json::Value SetColor(int floor, color col, int start_tile_num, int end_tile_num, double angle_offset) {
	Json::Value c;
	c["floor"] = floor;
	c["eventType"] = "RecolorTrack";
	c["startTile"][0] = start_tile_num;
	c["startTile"][1] = "ThisTile";
	c["endTile"][0] = end_tile_num;
	c["endTile"][1] = "ThisTile";
	c["trackColorType"] = "Single";
	stringstream rs, gs, bs, as;
	rs << std::hex << col.r;
	gs << std::hex << col.g;
	bs << std::hex << col.b;
	as << std::hex << col.a;
	string result(debug_string(rs.str()) + debug_string(gs.str()) + debug_string(bs.str()) + debug_string(as.str()));
	c["trackColor"] = result;
	c["secondaryTrackColor"] = "ffffff";
	c["trackColorAnimDuration"] = 2;
	c["trackColorPulse"] = "None";
	c["trackPulseLength"] = 10;
	c["trackStyle"] = "Gems";
	c["trackTexture"] = "";
	c["trackTextureScale"] = 1;
	c["angleOffset"] = angle_offset;
	c["eventTag"] = "";
	return c;
}

bool Player::SetData(string adofai_path, int position) {
	Json::CharReaderBuilder reader;
	Json::Value root;
	ifstream is(adofai_path, ifstream::binary);
	string errorMessage;
	auto bret = Json::parseFromStream(reader, is, &root, &errorMessage);

	if (bret == false) {
		cout << "Error to parse JSON file !!!" << " " << errorMessage << endl;
		return true;
	}

	auto *actions = &root["actions"];
	auto *angle_data = &root["angleData"];

	int x = 0, y = 0;

	Json::Value camera;
	camera["floor"] = position;
	camera["eventType"] = "MoveCamera";
	camera["duration"] = 0;
	camera["relativeTo"] = "Tile";
	camera["position"][0] = this->width / 2;
	camera["position"][1] = -this->height / 2;
	camera["rotation"] = 0;
	camera["zoom"] = 15 * (this->width > this->height ? this->width : this->height);
	camera["angleOffset"] = 0;
	camera["ease"] = "Linear";
	camera["eventTag"] = "";
	actions->append(camera);

	Json::Value speed;
	speed["floor"] = position;
	speed["eventType"] = "SetSpeed";
	speed["speedType"] = "Bpm";
	speed["beatsPerMinute"] = 60.0 / (double)this->frames.size() * this->fps;
	speed["bpmMultiplier"] = 1;
	actions->append(speed);
	
	angle_data->insert(position, 0);

	cout << endl << "Making Tiles . . . " << endl;

	for (int i = 0; i < this->height; i++) {
		for (int j = 0; j < this->width; j++) {
			angle_data->insert(position + j + i * this->width, 0); //angle insert
		}
		if (i) {
			actions->append(Position(i * this->width + position + 1, -this->width, -1));
		}
		gotoxy();
		cout << ceil((double)(i + 1) / (double)this->height * 100.0) << "%";
	}

	cout << endl << "Setting Frames Pixel Colors . . . " << endl;
	for (int i = 0; i < this->frames.size(); i++) {
		double angle_offset = 180.0 * (double)i / (double)this->frames.size();
		this->frames[i].SaveFrame(actions, angle_offset, position);
		gotoxy();
		cout << ceil((double)(i + 1) / (double)this->frames.size() * 100.0) << "% (" << i + 1 << " / " << this->frames.size() << ")";
	}

	cout << endl << "Total Change Color Effects : " << effect_cnt << endl;

	cout << endl << "Saving . . . " << endl;
	this->adofai_dat = root.toStyledString();
	return false;
}

void Player::Save(string adofai_path) {
	FILE *f;
	fopen_s(&f, adofai_path.c_str(), "wb");
	fprintf_s(f, "%s", this->adofai_dat.c_str());
	fclose(f);
}

#define ACCURACY(x) (floor((double)x / (double)accuracy)) * accuracy

void Frame::SetFrame(cv::Mat frame, int accuracy) {
	for (int i = 0; i < frame.rows; i++) {
		for (int j = 0; j < frame.cols; j++) {
			Vec3b bgrPixel = frame.at<Vec3b>(i, j);
			color c;
			Color::RGB_Color rgb;
			rgb.r = (int)bgrPixel[2];
			rgb.g = (int)bgrPixel[1];
			rgb.b = (int)bgrPixel[0];
			Color::HSV_Color hsv = Color::RGBtoHSV(rgb);
			hsv.h = ACCURACY(hsv.h);
			hsv.s = ACCURACY(hsv.s);
			hsv.v = ACCURACY(hsv.v);
			rgb = Color::HSVtoRGB(hsv);
			c.r = rgb.r;
			c.g = rgb.g;
			c.b = rgb.b;
			c.a = 255;
			this->color_table.push_back(c);
		}
	}
}

bool CompareColor(color c1, color c2) {
	return (c1.r == c2.r && c1.g == c2.g && c1.b == c2.b);
}

void Frame::SaveFrame(Json::Value *adofai, double angle_offset, int position) {
	color prev_color = { -1,-1,-1,-1 };
	for (int i = 0; i < this->color_table.size(); i++) {
		if (this->color_table[i].a != -1) {
			int j = i + 1;
			for (; j < this->color_table.size(); j++) {
				if (!CompareColor(this->color_table[i], this->color_table[j])) {
					break;
				}
			}
			auto act = SetColor(position, this->color_table[i], i + 1, j, angle_offset);
			adofai->append(act);
			i = j - 1;
			effect_cnt++;
		}
	}
}


void Frame::CompareFrame(Frame * frame) {
	if (this->color_table.size() != frame->color_table.size()) {
		frame->color_table.clear();
		for (int i = 0; i < this->color_table.size(); i++) {
			frame->color_table.push_back(this->color_table[i]);
		}
		return;
	}
	for (int i = 0; i < this->color_table.size(); i++) {
		if (CompareColor(this->color_table[i], frame->color_table[i])) {
			this->color_table[i].a = -1;
		}
		else {
			frame->color_table[i].r = this->color_table[i].r;
			frame->color_table[i].g = this->color_table[i].g;
			frame->color_table[i].b = this->color_table[i].b;
		}
	}
}

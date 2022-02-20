#include "Player.h"

int main(int ac, char** av) {
	cout << "[ ADOFAI Video Player ]" << endl << endl;
	cout << "영상을 맵으로 변환해줍니다. (효과가 너무 과해지지 않도록 주의하세요)" << endl;
	cout << "Developer : CLiF" << endl;
	cout << "Version : v 1.0" << endl << endl;
	
	cout << "Video Path : ";
	string video;
	cin >> video;

	cout << "Video Size : ";
	double size;
	cin >> size;

	cout << "Video Frame Per Second : ";
	double fps;
	cin >> fps;

	cout << "Adofai Map Path : ";
	string adofai;
	cin >> adofai;

	cout << "Start Tile Number : ";
	unsigned int tile;
	cin >> tile;

	cout << "Color Accuracy : ";
	unsigned int accuracy = 0;
	cin >> accuracy;

	cout << "Loop : ";
	unsigned int loop = 1;
	cin >> loop;

	Player player;
	if (player.SetVideoFile(video, fps, size, accuracy, loop)) { //1920*1080 -> 16 * 9
		return 1;
	} 
	if (player.SetData(adofai, tile)) {
		return 1;
	}
	player.Save(adofai);
	cout << "Completed!!";
	system("pause");
	return 0;
}
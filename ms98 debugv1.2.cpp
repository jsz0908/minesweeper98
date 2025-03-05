// minesweeper98 debugv1.2
// by:jsz0908
// github主页：https://github.com/jsz0908/
// 2025.3.5

//

#include <iostream>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include <conio.h>
#include <string>
//#include <Windows.h>  //字体颜色 鼠标事件....

using namespace std;

///////////////////////////////////全局常变量/////////////////////////////////////

const int MAX_ROW = 30;
const int MAX_COL = 30;
const int MAX_MINE = 891;  //以上数据参考了Minesweeper Arbiter游戏设置中的最大值

//#define MINE 10

enum STATE { BOMB = -1, UNKNOWN, SAFE }; //每个单位的三种不同状态

struct Cell {
	char type = ' ';	//'X': bomb,'#':unknown;'(0-8)':surrounding bomb(s);' ':safe
	STATE state = UNKNOWN;
	int mine_around = 0;
};

Cell player_map[MAX_ROW][MAX_COL];				        //玩家可见地图
Cell internal_map[MAX_ROW][MAX_COL];			        //实际地图

bool gamewin = false;  // 为true时获得胜利
bool gameover = false; // 为true时结束游戏
bool First_Click = false; // 首次点击不能为雷,只有true才能继续

int row = 8;
int col = 8;
int mine = 10;

int safe_cell = 0; //安全格子数

//////////////////////////游戏本体/////////////////////////////////////////


void Init_Map();										//地图初始化		
void Display_Map(Cell(*map)[MAX_COL], int row, int col);  //刷新地图显示
void Generate_Bomb(int mine);							//生成雷

void Click_Map(int& x, int& y);							//手动开雷
void Open_Cell(int input_x, int input_y);				//递归开无雷3*3区域

void Cycle_Game_Test();									//地雷生成随机测试
void FirstClickNoBomb();								//第一次点击不会开到雷
void Cycle_Game();										//扫雷循环


//////////////////////////菜单界面/////////////////////////////////////////
void Main_Menu();										//菜单
void Difficulty_Menu();									//难度选择
void Console_App_Help();								//帮助
void Exit();											//退出
void Test_Menu();										//测试菜单
void About();											//关于

//////////////////////////其他////////////////////////////////////////////

string gnrt_space(int count);							//空格生成器
int Digit_Num(int num);									//计算数字的位数

//////////////////////////////////////////////////////////////////////////


int main() {
	Main_Menu();


	system("pause");
	return 0;
}


void Init_Map() {
	for (int i = 0; i < row; ++i) {
		for (int j = 0; j < col; ++j) {
			player_map[i][j] = { '#', UNKNOWN, 0 };
			internal_map[i][j] = player_map[i][j];
		}
	}
}




void Display_Map(Cell(*map)[MAX_COL], int row, int col) {
	system("cls");

	cout << "地图大小为：" << row << " * " << col << endl;

	cout << gnrt_space(Digit_Num(row) + 1);
	if (Digit_Num(col) == 2) {
		for (int i = 1; i < col + 1; ++i) {
			if (i / 10 <= 0)
				cout << " " << " ";
			else
				cout << i / 10 << " ";
		}
		cout << endl;					//列数第一行（有十位的情况)

		cout << gnrt_space(Digit_Num(row) + 1);
		for (int i = 1; i < col + 1; ++i) {
			cout << i % 10 << " ";		//列数第二行
		}
	}
	else {
		for (int j = 1; j < col + 1; ++j) {
			cout << j << " ";			//列数只在10以内
		}

	}

	cout << endl;
	for (int i = 0; i < row; ++i) {
		if ((i + 1) / 10 == 0 && Digit_Num(row) == 2)
			cout << gnrt_space(1);
		cout << i + 1 << " ";
		for (int j = 0; j < col; ++j) {
			cout << map[i][j].type << ' ';
		}
		cout << endl;
	}

}


void Generate_Bomb(int MINE) {
	srand((unsigned int)time(NULL));
	int mine_count = 0;
	int x, y;
	while (mine_count < MINE) {
		x = rand() % row;
		y = rand() % col;
		if (internal_map[x][y].state != BOMB) {
			internal_map[x][y] = { 'X' , BOMB };
			++mine_count;
		}
	}

}

void Click_Map(int& x, int& y) {
	while (true) {
		cout << "请输入坐标（x行y列）..." << endl;
		cout << "*x,y之间空格隔开（行范围：1到" << row << "；列范围：1到" << col << ")" << endl;
		cin >> x >> y;
		if (x >= 1 && y >= 1 && x <= row && y <= col) {
			break;
		}
		else {
			cout << "坐标错误，请重新输入。" << endl;
		}
	}
}


void Open_Cell(int input_x, int input_y) {
	if (internal_map[input_x][input_y].state == BOMB) {
		gameover = true;
		return;
	}

	if (player_map[input_x][input_y].state == SAFE) return;  //开过的格子不必继续打开

	player_map[input_x][input_y].state = SAFE;
	++safe_cell;

	int mine_around = 0;									//准备统计雷数
	for (int delta_x = -1; delta_x <= 1; ++delta_x) {
		for (int delta_y = -1; delta_y <= 1; ++delta_y) {
			if (delta_x == 0 && delta_y == 0) continue;
			int temp_x = input_x + delta_x;
			int temp_y = input_y + delta_y;

			if (temp_x >= 0 && temp_x < row && temp_y >= 0 && temp_y < col) {
				if (internal_map[temp_x][temp_y].state == BOMB)
					++mine_around;							// 计算周围雷数
			}
		}
	}


	player_map[input_x][input_y].mine_around = mine_around;
	if (mine_around == 0) {
		player_map[input_x][input_y].type = ' '; // 无雷不显示雷数
	}
	else {
		player_map[input_x][input_y].type = mine_around + '0';
		return;  // 如果有雷，则不进行下列递归
	}


	for (int delta_x = -1; delta_x <= 1; ++delta_x) {
		for (int delta_y = -1; delta_y <= 1; ++delta_y) {
			int temp_x = input_x + delta_x;
			int temp_y = input_y + delta_y;

			if (temp_x >= 0 && temp_x < row && temp_y >= 0 && temp_y < col) {
				if (player_map[temp_x][temp_y].state == UNKNOWN) {
					Open_Cell(temp_x, temp_y);			// 递归展开周围无雷的格子
				}
			}
		}
	}
}

void FirstClickNoBomb() {
	int x, y;
	Init_Map();
	Generate_Bomb(mine);
	Display_Map(player_map, row, col);
	Click_Map(x, y);
	--x; --y;
	while (!First_Click) {
		if (internal_map[x][y].state == BOMB) {
			Init_Map();
			Generate_Bomb(mine);
		}
		else {
			First_Click = true;
		}
	}
	Open_Cell(x, y);

}


void Cycle_Game() {
	gameover = false;
	/*Init_Map();
	Generate_Bomb(mine);*/

	while (1) {
		Display_Map(player_map, row, col);
		int input_x, input_y;
		Click_Map(input_x, input_y);
		--input_x; --input_y;
		Open_Cell(input_x, input_y);

		if (safe_cell + mine == row * col) {
			cout << "胜利Win,恭喜!)" << endl;
			safe_cell = 0;
			break;
		}

		if (gameover) {
			Display_Map(internal_map, row, col);
			cout << "输掉了，哈哈！" << endl;
			break;
		}
	}

	cout << "是否返回主菜单？(按Y(是）/N（直接退出)" << endl;
	while (1) {
		if (_kbhit()) {
			char ch = _getch();
			if (ch == 'y' || ch == 'Y') {
				Main_Menu();
				break;
			}
			if (ch == 'n' || ch == 'N') {
				Exit();
				break;
			}
			else continue;

		}
	}


}

void Cycle_Game_Test() {
	gameover = false;
	Init_Map();
	Display_Map(internal_map, row, col);
	cout << "按任意键以继续" << endl;
	(void)_getch();
	Generate_Bomb(mine);
	Display_Map(internal_map, row, col);
	cout << "按任意键回到主菜单" << endl;
	(void)_getch();

}


void Main_Menu() {
	system("cls");
	cout << "###############minesweeper98 debug v1.2###############" << endl;
	cout << "####################菜单 Main Menu####################" << endl;
	cout << "1.开始游戏" << endl;
	cout << "2.帮助" << endl;
	cout << "3.地雷随机生成测试" << endl;
	cout << "4.关于作者" << endl;
	cout << "0.退出" << endl;
	cout << "######################################################" << endl;
	cout << "在键盘上输入数字以继续...";

	while (1) {
		if (_kbhit()) {
			char ch = _getch();

			if (ch == '1') {
				Difficulty_Menu(); break;
			}
			if (ch == '2') {
				Console_App_Help(); break;
			}
			if (ch == '3') {
				Test_Menu(); break;
			}
			if (ch == '4') {
				About(); break;
			}
			if (ch == '0') {
				Exit(); break;
			}

		}
	}
}


void Difficulty_Menu() {
	system("cls");
	cout << "###############难度 Difficulty##############" << endl;
	cout << "1.简单 Easy (8行8列10雷)" << endl;
	cout << "2.普通 Normal (16行16列40雷)" << endl;
	cout << "3.困难 Hard (16行32列99雷)" << endl;
	cout << "0.返回主菜单" << endl;
	cout << "在键盘上输入数字以继续...";

	while (1) {
		if (_kbhit()) {
			char ch = _getch();

			if (ch == '1') {
				system("cls");
				row = 8; col = 8; mine = 10;
				FirstClickNoBomb();
				Cycle_Game();

				break;
			}

			else if (ch == '2') {
				system("cls");
				row = 16; col = 16; mine = 40;
				FirstClickNoBomb();
				Cycle_Game();
				break;
			}

			else if (ch == '3') {
				system("cls");
				row = 16; col = 32; mine = 99;
				FirstClickNoBomb();
				Cycle_Game();
				break;
			}
			else if (ch == '0') {
				Main_Menu(); break;
			}
		}
	}
}

void Console_App_Help() {
	system("cls");
	cout << "此版本仅为测试" << endl;
	cout << "按任意键返回菜单...";

	(void)_getch();

	Main_Menu();
}

void Exit() {
	system("cls");
	cout << "欢迎下次回来" << endl;
	cout << "感谢您的游玩！" << endl;
	cout << endl << endl << endl;

	cout << "---------------------------------------------------" << endl;
	cout << "ms98 debugv1.2目前处于开发阶段" << endl;

	cout << "胜利条件还没加入" << endl;
	cout << "后续版本会陆续增加彩色字体、插旗、鼠标操作以及插旗快速开雷功能" << endl;
	cout << "---------------------------------------------------" << endl;
	cout << "作者:jsz0908" << endl;
	cout << "Github:https://github.com/jsz0908/" << endl;
	cout << "QQ:3412817216" << endl;
	cout << "Email:shengzhe0908@gmail.com" << endl;
	cout << "---------------------------------------------------" << endl;

}


void Test_Menu() {

	system("cls");
	cout << "###############测试难度 Difficulty of Test##############" << endl;
	cout << "1.简单 Easy (8行8列10雷)" << endl;
	cout << "2.普通 Normal (16行16列40雷)" << endl;
	cout << "3.困难 Hard (16行32列99雷)" << endl;
	cout << "0.返回主菜单" << endl;
	cout << "在键盘上输入数字以继续...";
	while (1) {
		if (_kbhit()) {
			char ch = _getch();
			if (ch == '1') {
				row = 8; col = 8; mine = 10;
				Cycle_Game_Test();

				break;
			}
			if (ch == '2') {
				row = 16; col = 16; mine = 40;
				Cycle_Game_Test();
				break;
			}
			if (ch == '3') {
				row = 16; col = 30; mine = 99;
				Cycle_Game_Test();
				break;
			}
			if (ch == '0') break;

		}

	}
	Main_Menu();
}


void About() {
	system("cls");
	cout << "---------------------------------------------------" << endl;
	cout << "ms98 debugv1.2目前处于开发阶段" << endl;
	cout << "后续版本会陆续增加彩色字体、插旗、鼠标操作以及插旗快速开雷功能" << endl;
	cout << "---------------------------------------------------" << endl;
	cout << "作者:jsz0908" << endl;
	cout << "Github:https://github.com/jsz0908/" << endl;
	cout << "QQ:3412817216" << endl;
	cout << "Email:shengzhe0908@gmail.com" << endl;
	cout << "---------------------------------------------------" << endl;
	cout << "按任意键返回主菜单" << endl;
	(void)_getch();
	Main_Menu();
}



string gnrt_space(int count) {
	return string(count, ' ');
}

int Digit_Num(int num) {
	if (num == 0) return 1;
	int digit = (int)(log10(abs(num)) + 1);
	return digit;
}



//更新日志:
// 
//debug v1.1:
//***********ms98 debugv1.1目前处于开发阶段
//***********bug:首次点击可能开到雷的问题还没解决(debug v1.2已解决）
//***********bug:胜利条件还没加入
//***********预告：后续版本会陆续增加彩色字体、插旗、鼠标操作以及插旗快速开雷功能
//***********2025.3.5 00:31
//debug v1.2
//***********修复：首次点击可能开到雷的问题
//***********修复：输入地雷坐标非法时死循环的问题
//***********更新：扫雷界面显示行列数
//***********2025.3.5 15:42









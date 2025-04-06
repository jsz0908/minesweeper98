// minesweeper98 release v2.1
// by:jsz0908
// github主页：https://github.com/jsz0908/
// 2025.4.6


#include <iostream>
#include <cmath>						
#include <ctime>					   //随机数
#include <conio.h>					   //非阻塞
#include <string>
#include <Windows.h>                 //字体颜色 键盘鼠标事件....

using namespace std;

///////////////////////////////////全局常变量/////////////////////////////////////

const int MAX_ROW = 30;
const int MAX_COL = 30;
const int MAX_MINE = 891;  //以上数据参考了Minesweeper Arbiter游戏设置中的最大值

//#define MINE 10

enum STATE { BOMB = -1, UNKNOWN, SAFE, FLAG }; //每个单位的四种不同状态

struct Cell {
	char type = ' ';	//'X': bomb,'#':unknown;'(0-8)':surrounding bomb(s);' ':safe;'F':flag
	STATE state = UNKNOWN;
	int mine_around = 0;
	int flag_around = 0;
};

Cell player_map[MAX_ROW][MAX_COL];				        //玩家可见地图
Cell internal_map[MAX_ROW][MAX_COL];			        //实际地图
Cell final_map[MAX_ROW][MAX_COL];						//游戏结束时的地图

int row = 8;
int col = 8;
int mine = 10;

bool gamewin = false;				// 为true时获得胜利
bool gameover = false;				// 为true时结束游戏
bool First_Click = false;			// 首次点击不能为雷,只有true才能继续
//bool FlagState = false;			    // 初始化为开雷模式


//局面统计:
bool StatsMode = true;				// 局面统计模式开关（默认为关）
bool ConsoleFontSize = true;			// 分辨率切换
int safe_cell = 0;					// 安全格子数
int flag_count = 0;					// 插旗数
int Click = 0;						// 操作次数

int cursor_x = 0, cursor_y = 0;		// 控制光标位置


// 颜色常量
const int COLOR_DEFAULT = 0x07;    // 默认白色
const int COLOR_BOMB = 0x0C;       // 亮红色
const int COLOR_FLAG = 0x0B;       // 亮青色
const int COLOR_SAFE = 0x0F;       // 亮白色
const int COLOR_NUM = 0x0A;        // 亮绿色
const int COLOR_Menu = 0x06;       // 黄色

const int Color_ControlCursor = 0xF0;//白色背景光标
const int Color_CubeNum[9] = 
{ Color_ControlCursor,0x09,0x02,0x0C,0x01,0x04,0x0B,0x07,0x08 }; //0-8地雷数字颜色 


//////////////////////////游戏本体/////////////////////////////////////////

void Init_Map();										//地图初始化		
void Display_Map(Cell(*map)[MAX_COL], int row, int col);  //刷新地图显示
void Generate_Bomb(int mine);							//生成雷

//void Click_Map(int& x, int& y);							//手动开雷                1.4版本移除，操作过于麻烦被淘汰
void Control_Game();									//键盘控制光标
void Open_Cell(int input_x, int input_y);				//递归开无雷3*3区域
void Flag_Cell(int input_x, int input_y);				//插旗
//void DisplayFlagState();
//void Safe_Cell_Count();
void Final_Map();										//最终地图

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

void SetConsoleFontSize(int fontWidth, int fontHeight); //设置字体高度（大小）
void SetConsoleSize(int width, int height);				//设置控制台大小
void LockConsoleWindowSize();							//锁定控制台大小


void SetColor(int color);								//设置颜色
void ResetColor();										//刷新颜色
void MoveCursor(int x, int y);							//光标位置重置
void HideCursor();										//隐藏光标



//////////////////////////测试用///////////////////////////////////////////


void Init_Stats() {
	cursor_x = row / 2 - 1;
	cursor_y = col / 2 - 1;
	First_Click = false;


	Click = 0;
	flag_count = 0;
	safe_cell = 0;
}



void Init_Console() {
	LockConsoleWindowSize();	//锁定控制台窗口尺寸（无法用鼠标扩展）
	SetConsoleSize(75, 30);	   //控制台大小
	SetConsoleFontSize(0, 24); //字体大小

	HideCursor();	//隐藏光标
}




//////////////////////////Main函数////////////////////////////////////////

int main() {
	srand((unsigned int)time(NULL));
	Init_Console();	//窗口初始化
	
	Main_Menu();
	
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
	MoveCursor(0, 0); // 代替了system("cls"); 优化了显示

	SetColor(COLOR_Menu);
	cout << "地图大小：" << row << " * " << col << endl;
	cout << "地雷数：" << mine << endl;
	ResetColor();

	cout << gnrt_space(Digit_Num(row) + 1);
	SetColor(COLOR_Menu);
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
	ResetColor();

	cout << endl;

	for (int i = 0; i < row; ++i) {
		SetColor(COLOR_Menu);
		if ((i + 1) / 10 == 0 && Digit_Num(row) == 2)
			cout << gnrt_space(1);
		cout << (i + 1) << " "; // 行号
		ResetColor();

		for (int j = 0; j < col; ++j) {
			// 设置颜色
			if (i == cursor_x && j == cursor_y) {
				
				switch (map[cursor_x][cursor_y].type) {
				case '1':SetColor(Color_ControlCursor | Color_CubeNum[1]); break; 
				case '2':SetColor(Color_ControlCursor | Color_CubeNum[2]); break; 
				case '3':SetColor(Color_ControlCursor | Color_CubeNum[3]); break; 
				case '4':SetColor(Color_ControlCursor | Color_CubeNum[4]); break; 
				case '5':SetColor(Color_ControlCursor | Color_CubeNum[5]); break; 
				case '6':SetColor(Color_ControlCursor | Color_CubeNum[6]); break; 
				case '7':SetColor(Color_ControlCursor | Color_CubeNum[7]); break; 
				case '8':SetColor(Color_ControlCursor | Color_CubeNum[8]); break; 
				default:SetColor(Color_ControlCursor); break;// 亮黄色背景
				}
			}
			else {
				switch (map[i][j].state) {
				case BOMB:  SetColor(COLOR_BOMB); break;
				case FLAG:  SetColor(COLOR_FLAG); break;
				case SAFE:  SetColor(COLOR_SAFE); break;
				default:    SetColor(COLOR_DEFAULT);
				}
				switch (map[i][j].type) {
				case '1':SetColor(0x09); break; //蓝色
				case '2':SetColor(0x02); break;
				case '3':SetColor(0x0C); break;
				case '4':SetColor(0x01); break;
				case '5':SetColor(0x04); break;
				case '6':SetColor(0x0B); break;
				case '7':SetColor(0x07); break;
				case '8':SetColor(0x08); break;
				default:break;
				}
			}

			cout << map[i][j].type << ' ';
		}
		ResetColor();
		cout << endl;
	}
	cout << "方向键移动光标，Enter(或者空格Space)开雷，F 插旗，ESC 退出" << endl;
	SetColor(COLOR_Menu);
	cout << "按T可打开/关闭当前局面统计" << endl;

	if (StatsMode == true) {
		cout << endl;
		cout << "局面统计:" << endl;
		cout << "Flag标记数：" << flag_count << " 操作次数：" << Click << " 已打开格子数：" << safe_cell << endl;
	}

	ResetColor();
	
}



void Generate_Bomb(int MINE) {

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

void Open_Cell(int input_x, int input_y) {
	if (player_map[input_x][input_y].state == FLAG) return;  // 插旗格子不检测

	if (internal_map[input_x][input_y].state == BOMB) {      //踩雷，游戏结束
		gameover = true;
		return;
	}

	if (player_map[input_x][input_y].state == SAFE &&
		(player_map[input_x][input_y].mine_around
		== player_map[input_x][input_y].flag_around)) {
		// 如果已经打开过 且周围地雷数与插旗数相等时 直接跳过（后面有相同操作,不可重复）
		return;
	}

	

	int temp = player_map[input_x][input_y].state;  //保存格子打开前状态

	player_map[input_x][input_y].state = SAFE;	//打开格子操作
	//++safe_cell;

	if (temp == UNKNOWN) {
		player_map[input_x][input_y].state = SAFE;
		safe_cell++; // 统计安全格数
	}


	int mine_around = 0, flag_around = 0; // 准备统计雷数
	for (int delta_x = -1; delta_x <= 1; ++delta_x) {
		for (int delta_y = -1; delta_y <= 1; ++delta_y) {
			if (delta_x == 0 && delta_y == 0) continue;
			int temp_x = input_x + delta_x;
			int temp_y = input_y + delta_y;

			if (temp_x >= 0 && temp_x < row && temp_y >= 0 && temp_y < col) {
				if (internal_map[temp_x][temp_y].state == BOMB)
					++mine_around; // 计算周围雷数
				if (player_map[temp_x][temp_y].state == FLAG)
					++flag_around;
			}
		}
	}

	player_map[input_x][input_y].mine_around = mine_around;
	player_map[input_x][input_y].flag_around = flag_around;

	if (mine_around == 0) {
		player_map[input_x][input_y].type = ' '; // 无雷不显示雷数
	}
	else {
		player_map[input_x][input_y].type = mine_around + '0';
	}

	// 快速开雷：如果周围标记的雷数等于周围的雷数，就递归打开未标记的格子
	if (mine_around == flag_around) {
		for (int delta_x = -1; delta_x <= 1; ++delta_x) {
			for (int delta_y = -1; delta_y <= 1; ++delta_y) {
				int temp_x = input_x + delta_x;
				int temp_y = input_y + delta_y;

				if (temp_x >= 0 && temp_x < row && temp_y >= 0 && temp_y < col) {
					if (player_map[temp_x][temp_y].state == UNKNOWN) {
						Open_Cell(temp_x, temp_y); // 递归展开周围无雷的格子
					}
				}
			}
		}
	}
}



void Flag_Cell(int input_x, int input_y) {
	if (player_map[input_x][input_y].state == SAFE) {			   //若格子被翻开 则flag无效
		//cout << "无效坐标，请重新输入" << endl;
		return;
	}
	if (player_map[input_x][input_y].state == UNKNOWN) {
		player_map[input_x][input_y] = { 'F', FLAG };
		++flag_count;
	}
	else {
		player_map[input_x][input_y] = { '#', UNKNOWN };
		--flag_count;
	}
}

void Final_Map() {
	for (int i = 0; i < row; ++i) {
		for (int j = 0; j < col; ++j) {
			if (player_map[i][j].state == UNKNOWN) {
				if (safe_cell + mine == row * col)
					internal_map[i][j] = { 'F', FLAG };

				final_map[i][j] = internal_map[i][j];
			}
			else {
				final_map[i][j] = player_map[i][j];
			}
		}
	}
}

void FirstClickNoBomb() {
	while (!First_Click) {
		if (internal_map[cursor_x][cursor_y].state == BOMB) {
			internal_map[cursor_x][cursor_y] = { '#', UNKNOWN, 0 };
			int new_x, new_y;
			do {
				new_x = rand() % row;
				new_y = rand() % col;
			} while (internal_map[new_x][new_y].state == BOMB || (new_x == cursor_x && new_y == cursor_y));

			internal_map[new_x][new_y] = { 'X', BOMB };
		}
		else {
			First_Click = true;
		}
	}

	for (int i = 0; i < row; ++i) {
		for (int j = 0; j < col; ++j) {
			if (internal_map[i][j].state == UNKNOWN) {
				internal_map[i][j] = { ' ', SAFE };
			}
		}
	}

	Open_Cell(cursor_x, cursor_y);
}


//void DisplayFlagState() {
//	if (FlagState == false)
//		cout << "开雷模式" << endl;
//	else
//		cout << "插旗模式" << endl;
//}


void Control_Game() {
	
	while (1) {
	
		char key = _getch();
		switch (key) {
		case 72: // 方向键 ↑
			if (cursor_x > 0) cursor_x--;
			break;
		case 80: // 方向键 ↓
			if (cursor_x < row - 1) cursor_x++;
			break;
		case 75: // 方向键 ←
			if (cursor_y > 0) cursor_y--;
			break;
		case 77: // 方向键 →
			if (cursor_y < col - 1) cursor_y++;
			break;
		case 13: case 32:// Enter 键 或 space 键
			if (Click == 0)
				FirstClickNoBomb();
			else
				Open_Cell(cursor_x, cursor_y);
			/*Safe_Cell_Count();*/
			++Click;
			break;

		case 'F': case 'f': // 插旗模式
			Flag_Cell(cursor_x, cursor_y);
			++Click;
			break;
		case 'T': case 't':
			StatsMode = !StatsMode;
			system("cls");
			Display_Map(player_map, row, col);
			break;

		case 27: // ESC 退出
			break;
		}

		if (key == 27)
			break;
		Display_Map(player_map, row, col);
	

		// 检查游戏状态
		if (safe_cell + mine == row * col) {
			Final_Map();
			Display_Map(final_map, row, col);
			cout << "恭喜！你赢了！" << endl;
			(void)_getch();
			//Display_Map(internal_map, row, col);
			break;
		}
		if (gameover) {
			Final_Map();
			Display_Map(final_map, row, col);
			cout << "游戏结束，你踩到雷了！" << endl;
			(void)_getch();
			break;
		}
	}

}


void Cycle_Game() {

	gameover = false;
	Init_Stats();
	Init_Map();
	Generate_Bomb(mine);
	Display_Map(player_map, row, col);

	Control_Game(); // 用方向键玩扫雷
	Sleep(50);
	system("cls");
	SetColor(COLOR_FLAG);
	cout << "按任意键重新开始" << endl << "按M回到主菜单" << endl << "按数字0退出" << endl;
	ResetColor();
	while (1) {
		if (_kbhit()) {
			char ch = _getch();
			if (ch == 'M' || ch == 'm') {
				Main_Menu();
				break;
			}
			else if (ch == '0') {
				Exit();
				break;
			}
			else {
				system("cls");
				Cycle_Game();
			}

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
	SetColor(COLOR_Menu);
	cout << "##############minesweeper98 release v2.1.1############" << endl;
	cout << "####################菜单 Main Menu####################" << endl;
	cout << "1.开始游戏" << endl;
	cout << "2.帮助" << endl;
	cout << "3.地雷随机生成测试" << endl;
	cout << "4.关于作者" << endl;
	cout << "5.分辨率设置" << endl;
	cout << "0.退出" << endl;
	cout << "######################################################" << endl;
	cout << "在键盘上输入数字以继续...";
	ResetColor();
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
			if (ch == '5'){
				ConsoleFontSize = !ConsoleFontSize;
				if (ConsoleFontSize == true)
					SetConsoleFontSize(0, 24);
				else
					SetConsoleFontSize(0, 16);

			}
			if (ch == '0') {
				Exit(); break;
			}

		}
	}
}


void Difficulty_Menu() {
	system("cls");
	SetColor(COLOR_Menu);
	cout << "###############难度 Difficulty##############" << endl;
	cout << "1.简单 Easy (8行8列10雷)" << endl;
	cout << "2.普通 Normal (16行16列40雷)" << endl;
	cout << "3.困难 Hard (16行30列99雷)" << endl;
	cout << "0.返回主菜单" << endl;
	cout << "在键盘上输入数字以继续...";
	ResetColor();

	while (1) {
		if (_kbhit()) {
			char ch = _getch();

			if (ch == '1') {
				system("cls");
				row = 8; col = 8; mine = 10;
				break;
			}

			else if (ch == '2') {
				system("cls");
				row = 16; col = 16; mine = 40;
				break;
			}

			else if (ch == '3') {
				system("cls");
				row = 16; col = 30; mine = 99;
				break;
			}
			else if (ch == '0') {
				Main_Menu(); break;
			}
		}
	}
	Cycle_Game();
}

void Console_App_Help() {
	system("cls");
	SetColor(COLOR_Menu);
	cout << "游戏操作" << endl;
	cout << "方向键移动光标，Enter(或者空格Space)开雷，F 插旗，ESC 退出" << endl;
	cout << "按任意键返回菜单...";
	ResetColor();
	(void)_getch();

	Main_Menu();
}

void Exit() {
	system("cls");
	cout << "欢迎下次回来" << endl;
	cout << "感谢您的游玩！" << endl;
	cout << endl << endl << endl;
	SetColor(COLOR_Menu);
	cout << "---------------------------------------------------" << endl;
	cout << "ms98还会继续开发" << endl;
	cout << "后续版本会陆续增加新功能并优化" << endl;
	cout << "---------------------------------------------------" << endl;
	cout << "作者:jsz0908" << endl;
	cout << "Github:https://github.com/jsz0908/" << endl;
	cout << "QQ:3412817216" << endl;
	cout << "Email:shengzhe0908@gmail.com" << endl;
	cout << "---------------------------------------------------" << endl;
	ResetColor();

}

void Test_Menu() {

	system("cls");
	SetColor(COLOR_Menu);
	cout << "###############测试难度 Difficulty of Test##############" << endl;
	cout << "1.简单 Easy (8行8列10雷)" << endl;
	cout << "2.普通 Normal (16行16列40雷)" << endl;
	cout << "3.困难 Hard (16行30列99雷)" << endl;
	cout << "0.返回主菜单" << endl;
	cout << "在键盘上输入数字以继续...";
	ResetColor();
	while (1) {
		if (_kbhit()) {
			char ch = _getch();
			if (ch == '1') {
				row = 8; col = 8; mine = 10;
				break;
			}
			if (ch == '2') {
				row = 16; col = 16; mine = 40;
				break;
			}
			if (ch == '3') {
				row = 16; col = 30; mine = 99;
				break;
			}
			if (ch == '0') break;

		}

	}
	system("cls");
	Cycle_Game_Test();
	Main_Menu();
}


void About() {
	system("cls");
	SetColor(COLOR_Menu);
	cout << "---------------------------------------------------" << endl;
	cout << "ms98还会继续开发" << endl;
	cout << "后续版本会陆续增加新功能并优化" << endl;
	cout << "---------------------------------------------------" << endl;
	cout << "作者:jsz0908" << endl;
	cout << "Github:https://github.com/jsz0908/" << endl;
	cout << "QQ:3412817216" << endl;
	cout << "Email:shengzhe0908@gmail.com" << endl;
	cout << "---------------------------------------------------" << endl;
	cout << "按任意键返回主菜单" << endl;
	ResetColor();
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

void SetConsoleFontSize(int fontWidth, int fontHeight) {
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_FONT_INFOEX cfi;

	cfi.cbSize = sizeof(CONSOLE_FONT_INFOEX);
	GetCurrentConsoleFontEx(hOut, FALSE, &cfi);

	cfi.dwFontSize.X = fontWidth;   // 字体宽度
	cfi.dwFontSize.Y = fontHeight;  // 字体高度（调整这个就是“缩放”）

	wcscpy_s(cfi.FaceName, L"Consolas"); // 设置字体类型
	SetCurrentConsoleFontEx(hOut, FALSE, &cfi);
}

void SetConsoleSize(int width, int height) {
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);

	// 步骤 1：设置缓冲区尺寸（必须 >= 窗口尺寸）
	COORD bufferSize;
	bufferSize.X = (SHORT)width;
	bufferSize.Y = (SHORT)height;
	SetConsoleScreenBufferSize(hOut, bufferSize);

	// 步骤 2：设置窗口尺寸（不能超过缓冲区尺寸）
	SMALL_RECT windowSize;
	windowSize.Left = 0;
	windowSize.Top = 0;
	windowSize.Right = width - 1;
	windowSize.Bottom = height - 1;
	SetConsoleWindowInfo(hOut, TRUE, &windowSize);
}

void LockConsoleWindowSize() {
	HWND hwnd = GetConsoleWindow();

	// 禁用最大化按钮 + 禁止调整窗口大小
	LONG style = GetWindowLong(hwnd, GWL_STYLE);
	style &= ~WS_MAXIMIZEBOX;     // 禁用最大化
	style &= ~WS_SIZEBOX;         // 禁用调整边框
	SetWindowLong(hwnd, GWL_STYLE, style);

	// 强制窗口刷新样式生效
	SetWindowPos(hwnd, nullptr, 0, 0, 0, 0,
		SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
}

// 在工具函数区域添加
void SetColor(int color) {
	static HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, color);
//形参color说明（四位16进制数)
//背景色（高四位）
//0x00 → 黑色背景   0x80 → 灰色背景
//0x10 → 蓝色背景   0x90 → 亮蓝背景
//0x20 → 绿色背景   0xA0 → 亮绿背景
//0x30 → 青色背景   0xB0 → 亮青背景
//0x40 → 红色背景   0xC0 → 亮红背景
//0x50 → 紫色背景   0xD0 → 亮紫背景
//0x60 → 黄色背景   0xE0 → 亮黄背景
//0x70 → 白色背景   0xF0 → 亮白背景

//前景色（低四位）
//0x00 → 黑色       0x08 → 灰色
//0x01 → 蓝色       0x09 → 亮蓝
//0x02 → 绿色       0x0A → 亮绿
//0x03 → 青色       0x0B → 亮青
//0x04 → 红色       0x0C → 亮红
//0x05 → 紫色       0x0D → 亮紫
//0x06 → 黄色       0x0E → 亮黄
//0x07 → 白色       0x0F → 亮白

//例如color = 0x00意思是0x00 | 0x00 (黑色背景 | 黑色前景)

}

void ResetColor() {
	SetColor(COLOR_DEFAULT);
}


void MoveCursor(int x, int y) {
	COORD coord = { (SHORT)x, (SHORT)y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void HideCursor() {
	CONSOLE_CURSOR_INFO cursorInfo;
	cursorInfo.dwSize = 1;
	cursorInfo.bVisible = FALSE;
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
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
//***********修复：胜利条件还没加入的问题
//***********更新：扫雷界面显示行列数
//***********2025.3.5 15:42
//release v1.3
//***********优化：首次点击若开到雷不会重置全部地雷，只会更改这颗雷的位置，提升效率
//***********重大更新: flag机制:输入f(F)并回车可进入标flag/开雷模式 游戏基本玩法完成
//******************目前为止，扫雷有以下三种输入格式:
//******************输入格式1:行数 列数   （如：1 2 意思是打开(或flag标记）状态下 操作1行2列的方格
//******************输入格式2：f(或F）    （如：F   意思是切换 打开/flag标记 模式）
//******************输入格式3：f 行数 列数 （如 F 1 2 意思是切换模式 并操作1行2列的方格）
//***********更新：帮助模块加入了游戏说明
//***********2025.3.7 23:07

//release v2.1
//***********!!!重大更新!!!:用键盘控制方向键来进行游戏操作，摒弃了输入坐标的玩法
//***********更新：彩色字体
//***********更新：游戏内按T可打开实时局面统计(点击数、Flag标记数以及已打开格子数)
//***********2025.4.3 17:22

//release v2.1.1
//***********更新：从此版本开始采用静态编译（绿色安全，无需依赖dll）
//***********更新：控制台窗口可以调整分辨率（在主菜单）
//***********更新：默认开启实时局面统计（游戏中）
//***********更新：不同数字对应不同颜色（游戏中）
//***********更新：游戏胜利或结束可以快速新开游戏
//***********更新：游戏结束时可以显示已打开的区域，以及未发掘的雷
//				   且如果满足胜利条件，未flag的雷块会变成Flag
//***********2025.4.7 00:01






// minesweeper98 debugv1.2
// by:jsz0908
// github��ҳ��https://github.com/jsz0908/
// 2025.3.5

//

#include <iostream>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include <conio.h>
#include <string>
//#include <Windows.h>  //������ɫ ����¼�....

using namespace std;

///////////////////////////////////ȫ�ֳ�����/////////////////////////////////////

const int MAX_ROW = 30;
const int MAX_COL = 30;
const int MAX_MINE = 891;  //�������ݲο���Minesweeper Arbiter��Ϸ�����е����ֵ

//#define MINE 10

enum STATE { BOMB = -1, UNKNOWN, SAFE }; //ÿ����λ�����ֲ�ͬ״̬

struct Cell {
	char type = ' ';	//'X': bomb,'#':unknown;'(0-8)':surrounding bomb(s);' ':safe
	STATE state = UNKNOWN;
	int mine_around = 0;
};

Cell player_map[MAX_ROW][MAX_COL];				        //��ҿɼ���ͼ
Cell internal_map[MAX_ROW][MAX_COL];			        //ʵ�ʵ�ͼ

bool gamewin = false;  // Ϊtrueʱ���ʤ��
bool gameover = false; // Ϊtrueʱ������Ϸ
bool First_Click = false; // �״ε������Ϊ��,ֻ��true���ܼ���

int row = 8;
int col = 8;
int mine = 10;

int safe_cell = 0; //��ȫ������

//////////////////////////��Ϸ����/////////////////////////////////////////


void Init_Map();										//��ͼ��ʼ��		
void Display_Map(Cell(*map)[MAX_COL], int row, int col);  //ˢ�µ�ͼ��ʾ
void Generate_Bomb(int mine);							//������

void Click_Map(int& x, int& y);							//�ֶ�����
void Open_Cell(int input_x, int input_y);				//�ݹ鿪����3*3����

void Cycle_Game_Test();									//���������������
void FirstClickNoBomb();								//��һ�ε�����Ὺ����
void Cycle_Game();										//ɨ��ѭ��


//////////////////////////�˵�����/////////////////////////////////////////
void Main_Menu();										//�˵�
void Difficulty_Menu();									//�Ѷ�ѡ��
void Console_App_Help();								//����
void Exit();											//�˳�
void Test_Menu();										//���Բ˵�
void About();											//����

//////////////////////////����////////////////////////////////////////////

string gnrt_space(int count);							//�ո�������
int Digit_Num(int num);									//�������ֵ�λ��

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

	cout << "��ͼ��СΪ��" << row << " * " << col << endl;

	cout << gnrt_space(Digit_Num(row) + 1);
	if (Digit_Num(col) == 2) {
		for (int i = 1; i < col + 1; ++i) {
			if (i / 10 <= 0)
				cout << " " << " ";
			else
				cout << i / 10 << " ";
		}
		cout << endl;					//������һ�У���ʮλ�����)

		cout << gnrt_space(Digit_Num(row) + 1);
		for (int i = 1; i < col + 1; ++i) {
			cout << i % 10 << " ";		//�����ڶ���
		}
	}
	else {
		for (int j = 1; j < col + 1; ++j) {
			cout << j << " ";			//����ֻ��10����
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
		cout << "���������꣨x��y�У�..." << endl;
		cout << "*x,y֮��ո�������з�Χ��1��" << row << "���з�Χ��1��" << col << ")" << endl;
		cin >> x >> y;
		if (x >= 1 && y >= 1 && x <= row && y <= col) {
			break;
		}
		else {
			cout << "����������������롣" << endl;
		}
	}
}


void Open_Cell(int input_x, int input_y) {
	if (internal_map[input_x][input_y].state == BOMB) {
		gameover = true;
		return;
	}

	if (player_map[input_x][input_y].state == SAFE) return;  //�����ĸ��Ӳ��ؼ�����

	player_map[input_x][input_y].state = SAFE;
	++safe_cell;

	int mine_around = 0;									//׼��ͳ������
	for (int delta_x = -1; delta_x <= 1; ++delta_x) {
		for (int delta_y = -1; delta_y <= 1; ++delta_y) {
			if (delta_x == 0 && delta_y == 0) continue;
			int temp_x = input_x + delta_x;
			int temp_y = input_y + delta_y;

			if (temp_x >= 0 && temp_x < row && temp_y >= 0 && temp_y < col) {
				if (internal_map[temp_x][temp_y].state == BOMB)
					++mine_around;							// ������Χ����
			}
		}
	}


	player_map[input_x][input_y].mine_around = mine_around;
	if (mine_around == 0) {
		player_map[input_x][input_y].type = ' '; // ���ײ���ʾ����
	}
	else {
		player_map[input_x][input_y].type = mine_around + '0';
		return;  // ������ף��򲻽������еݹ�
	}


	for (int delta_x = -1; delta_x <= 1; ++delta_x) {
		for (int delta_y = -1; delta_y <= 1; ++delta_y) {
			int temp_x = input_x + delta_x;
			int temp_y = input_y + delta_y;

			if (temp_x >= 0 && temp_x < row && temp_y >= 0 && temp_y < col) {
				if (player_map[temp_x][temp_y].state == UNKNOWN) {
					Open_Cell(temp_x, temp_y);			// �ݹ�չ����Χ���׵ĸ���
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
			cout << "ʤ��Win,��ϲ!)" << endl;
			safe_cell = 0;
			break;
		}

		if (gameover) {
			Display_Map(internal_map, row, col);
			cout << "����ˣ�������" << endl;
			break;
		}
	}

	cout << "�Ƿ񷵻����˵���(��Y(�ǣ�/N��ֱ���˳�)" << endl;
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
	cout << "��������Լ���" << endl;
	(void)_getch();
	Generate_Bomb(mine);
	Display_Map(internal_map, row, col);
	cout << "��������ص����˵�" << endl;
	(void)_getch();

}


void Main_Menu() {
	system("cls");
	cout << "###############minesweeper98 debug v1.2###############" << endl;
	cout << "####################�˵� Main Menu####################" << endl;
	cout << "1.��ʼ��Ϸ" << endl;
	cout << "2.����" << endl;
	cout << "3.����������ɲ���" << endl;
	cout << "4.��������" << endl;
	cout << "0.�˳�" << endl;
	cout << "######################################################" << endl;
	cout << "�ڼ��������������Լ���...";

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
	cout << "###############�Ѷ� Difficulty##############" << endl;
	cout << "1.�� Easy (8��8��10��)" << endl;
	cout << "2.��ͨ Normal (16��16��40��)" << endl;
	cout << "3.���� Hard (16��32��99��)" << endl;
	cout << "0.�������˵�" << endl;
	cout << "�ڼ��������������Լ���...";

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
	cout << "�˰汾��Ϊ����" << endl;
	cout << "����������ز˵�...";

	(void)_getch();

	Main_Menu();
}

void Exit() {
	system("cls");
	cout << "��ӭ�´λ���" << endl;
	cout << "��л�������棡" << endl;
	cout << endl << endl << endl;

	cout << "---------------------------------------------------" << endl;
	cout << "ms98 debugv1.2Ŀǰ���ڿ����׶�" << endl;

	cout << "ʤ��������û����" << endl;
	cout << "�����汾��½�����Ӳ�ɫ���塢���졢�������Լ�������ٿ��׹���" << endl;
	cout << "---------------------------------------------------" << endl;
	cout << "����:jsz0908" << endl;
	cout << "Github:https://github.com/jsz0908/" << endl;
	cout << "QQ:3412817216" << endl;
	cout << "Email:shengzhe0908@gmail.com" << endl;
	cout << "---------------------------------------------------" << endl;

}


void Test_Menu() {

	system("cls");
	cout << "###############�����Ѷ� Difficulty of Test##############" << endl;
	cout << "1.�� Easy (8��8��10��)" << endl;
	cout << "2.��ͨ Normal (16��16��40��)" << endl;
	cout << "3.���� Hard (16��32��99��)" << endl;
	cout << "0.�������˵�" << endl;
	cout << "�ڼ��������������Լ���...";
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
	cout << "ms98 debugv1.2Ŀǰ���ڿ����׶�" << endl;
	cout << "�����汾��½�����Ӳ�ɫ���塢���졢�������Լ�������ٿ��׹���" << endl;
	cout << "---------------------------------------------------" << endl;
	cout << "����:jsz0908" << endl;
	cout << "Github:https://github.com/jsz0908/" << endl;
	cout << "QQ:3412817216" << endl;
	cout << "Email:shengzhe0908@gmail.com" << endl;
	cout << "---------------------------------------------------" << endl;
	cout << "��������������˵�" << endl;
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



//������־:
// 
//debug v1.1:
//***********ms98 debugv1.1Ŀǰ���ڿ����׶�
//***********bug:�״ε�����ܿ����׵����⻹û���(debug v1.2�ѽ����
//***********bug:ʤ��������û����
//***********Ԥ�棺�����汾��½�����Ӳ�ɫ���塢���졢�������Լ�������ٿ��׹���
//***********2025.3.5 00:31
//debug v1.2
//***********�޸����״ε�����ܿ����׵�����
//***********�޸��������������Ƿ�ʱ��ѭ��������
//***********���£�ɨ�׽�����ʾ������
//***********2025.3.5 15:42









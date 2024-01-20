#include <graphics.h>
#include <conio.h>
#include <iostream>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 650

#define RATIO 50

#define ROW 9
#define COL 12

#define START_X 100
#define START_Y 100

#define KEY_UP 'w'
#define KEY_DOWN 's'
#define KEY_LEFT 'a'
#define KEY_RIGHT 'd'
#define KEY_QUIT '\x1B'
#define isValid(pos) (pos.x >= 0 && pos.x < ROW && pos.y >= 0 && pos.y < COL)

// 加载初始地图
int map[ROW][COL] = {
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0},
	{0, 1, 4, 1, 0, 2, 1, 0, 2, 1, 0, 0},
	{0, 1, 0, 1, 0, 1, 0, 0, 1, 1, 1, 0},
	{0, 1, 0, 2, 0, 1, 1, 4, 1, 1, 1, 0},
	{0, 1, 1, 1, 0, 3, 1, 1, 1, 4, 1, 0},
	{0, 1, 2, 1, 1, 4, 1, 1, 1, 1, 1, 0},
	{0, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
};

enum _PROPS
{
	WALL,	 // 墙
	FLOOR,	 // 地板
	BOX_DES, // 箱子目标
	MAN,	 // 人
	BOX,	 // 箱子
	HIT,	 // 箱子命中目标
	ALL
};

enum _DIRECTION
{
	UP,
	DOWN,
	LEFT,
	RIGHT
};

struct _POS
{
	int x;
	int y;
};

typedef _DIRECTION DIRECTION;
typedef struct _POS POS;
typedef enum _PROPS PROPS;
IMAGE images[ALL];
POS man; // 小人的位置

void changeMap(POS *pos, PROPS prop)
{
	// 改变某一格道具,并刷新图片
	map[pos->x][pos->y] = prop;
	putimage(START_X + pos->y * RATIO, START_Y + pos->x * RATIO, &images[prop]);
}

// 实现上下左右控制
void gameControl(DIRECTION direct)
{
	POS next_pos = man, next_next_pos = man;
	switch (direct)
	{
	case UP:
		next_pos.x = man.x - 1;
		next_next_pos.x = man.x - 2;
		break;
	case DOWN:
		next_pos.x = man.x + 1;
		next_next_pos.x = man.x + 2;
		break;
	case LEFT:
		next_pos.y = man.y - 1;
		next_next_pos.y = man.y - 2;
		break;
	case RIGHT:
		next_pos.y = man.y + 1;
		next_next_pos.y = man.y + 2;
		break;
	}
	// 1. 下一步是地板,前进一步(人站到next_pos的位置, 人之前站的位置设置为地板)
	if (map[next_pos.x][next_pos.y] == FLOOR && isValid(next_pos))
	{
		changeMap(&next_pos, MAN);
		changeMap(&man, FLOOR);
		man = next_pos;
	}
	// 2. 下一步是箱子,判断下下步, 如果是地板或者目的地,能推箱子往前走
	if (map[next_pos.x][next_pos.y] == BOX && isValid(next_pos))
	{
		// 2.1 箱子前是地板
		// 2.2 箱子后是墙
		if (map[next_next_pos.x][next_next_pos.y] == FLOOR && isValid(next_next_pos))
		{
			changeMap(&next_next_pos, BOX);
			changeMap(&next_pos, MAN);
			changeMap(&man, FLOOR);
			man = next_pos;
		}
		else if (map[next_next_pos.x][next_next_pos.y] == BOX_DES && isValid(next_next_pos))
		{
			changeMap(&next_next_pos, HIT);
			changeMap(&next_pos, MAN);
			changeMap(&man, FLOOR);
			man = next_pos;
		}
	}
}

bool isGameOver()
{
	for (int i = 0; i < ROW; i++)
	{
		for (int j = 0; j < COL; j++)
		{
			if (map[i][j] == BOX_DES)
			{
				return false;
			}
		}
	}
	return true;
}

void gameOverScene()
{
	IMAGE bg;
	loadimage(&bg, _T("./resource/victory.jpg"), SCREEN_WIDTH, SCREEN_HEIGHT, true);
	putimage(0, 0, &bg);
}

int main()
{
	IMAGE bg_img;

	// 创建背景图
	initgraph(800, 650);
	loadimage(&bg_img, _T("./resource/blackground.bmp"), SCREEN_WIDTH, SCREEN_HEIGHT, true);
	putimage(0, 0, &bg_img);

	// 墙 0, 地板 1, 箱子目的地 2, 人 3, 箱子 4, 命中目标 5,
	loadimage(&images[WALL], _T("./resource/wall_right.bmp"), RATIO, RATIO, true);
	loadimage(&images[FLOOR], _T("./resource/floor.bmp"), RATIO, RATIO, true);
	loadimage(&images[BOX_DES], _T("./resource/des.bmp"), RATIO, RATIO, true);
	loadimage(&images[MAN], _T("./resource/man.bmp"), RATIO, RATIO, true);
	loadimage(&images[BOX], _T("./resource/box.bmp"), RATIO, RATIO, true);
	loadimage(&images[HIT], _T("./resource/box.bmp"), RATIO, RATIO, true);

	// 给每一格贴图
	for (int i = 0; i < ROW; i++)
	{
		for (int j = 0; j < COL; j++)
		{
			if (map[i][j] == MAN)
			{
				man.x = i;
				man.y = j;
			}
			putimage(START_X + j * RATIO, START_Y + i * RATIO, &images[map[i][j]]);
		}
	}

	bool quit = false;

	do
	{
		if (kbhit())
		{
			char ch = _getch();
			std::cout << ch << std::endl;
			if (ch == KEY_UP)
			{
				gameControl(UP);
			}
			else if (ch == KEY_DOWN)
			{
				gameControl(DOWN);
			}

			else if (ch == KEY_LEFT)
			{
				gameControl(LEFT);
			}

			else if (ch == KEY_RIGHT)
			{
				gameControl(RIGHT);
			}
			else if (ch == KEY_QUIT)
			{
				quit = true;
			}
			if (isGameOver())
			{
				gameOverScene();
			}
		}
		Sleep(100);
	} while (quit == false);

	system("pause");
	return 0;
}
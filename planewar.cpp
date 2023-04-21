#include<stdio.h>
#include<iostream>
#include<easyx.h>
#include<graphics.h>
#include<stdlib.h>
#include<assert.h>
#include <conio.h>
#include<time.h>
using namespace std;

//设置图片可用的两个函数
//要加附加库winmm.lib
#pragma comment( lib, "MSIMG32.LIB")
//支持任意底色的png图片，但透明效果取决于图片本身，若图片底色与图片本身颜色相近，透明化效果会大打折扣，如出现很大的黑色边缘块。
//使用 Windows GDI 函数实现透明位图
//参数1：NULL即可，参数2、参数3为图片输出的坐标，参数4为源图片的指针，参数5为要透明的底色(若图片为透明图片，默认为BLACK)
void transparentimage1(IMAGE* dstimg, int x, int y, IMAGE* srcimg, UINT transparentcolor)
{
	HDC dstDC = GetImageHDC(dstimg);
	HDC srcDC = GetImageHDC(srcimg);
	int w = srcimg->getwidth();
	int h = srcimg->getheight();
	// 使用 Windows GDI 函数实现透明位图
	TransparentBlt(dstDC, x, y, w, h, srcDC, 0, 0, w, h, transparentcolor);
}
//不需要指定透明的颜色，此种方法透明效果非常好，但只支持底色本身为透明色的png图片，即在windows下打开时就没有底色的png图片。
void transparentimage2(IMAGE* dstimg, int x, int y, IMAGE* srcimg) //新版png
{
	HDC dstDC = GetImageHDC(dstimg);
	HDC srcDC = GetImageHDC(srcimg);
	int w = srcimg->getwidth();
	int h = srcimg->getheight();
	BLENDFUNCTION bf = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
	AlphaBlend(dstDC, x, y, w, h, srcDC, 0, 0, w, h, bf);
}
//初始信息
enum MyImage
{
	bkImage = 0,
	myPlaneImage = 1,
	bulletImage = 2,
	smallFoeImage = 3,
	hugeFoeImage = 4,
	failImage = 5
};
IMAGE pImage[failImage +1];
enum MyEnum
{
	//子弹个数
	BulletNum = 15,
	//窗口
	WinWidth = 500,
	WinHeight = 700,
	//我方飞机
	MyPlaneWidth = 120,
	MyPlaneHeight = 120,
	//子弹
	BulletWidth = 25,
	BulletHeight = 25,
	EnemyNum = 100,
	//敌方飞机
	FoeWidth = 80,
	FoeHeight = 80,
	SMALL = 1,
	BIG=3
};
//设置玩家
struct MyPlane
{
	int x, y;
	bool isLive;//是否存活
	//敌机特有属性
	int width,height;
	int hp;
	int type;
	int win;
}player,bullet[15],enemy[EnemyNum];
//封装定时器
bool Timer(int ms,int id)
{
	static DWORD t[10];
	if (clock() - t[id] > ms)
	{
		t[id] = clock();
		return true;
	}
	return false;
}
//判断游戏是否失败
int askMyHp()
{
	for (int i = 0; i < EnemyNum; i++)
	{
		if (!enemy[i].isLive)
		{
			continue;
		}
		if (enemy[i].win)
		{
			continue;
		}
		if (enemy[i].height + enemy[i].y > player.y)
		{
			enemy[i].win = 1;
			player.hp--;
			if (player.hp <= 0)
			{
				player.isLive = false;
				break;
			}
		}
	}
	return player.hp;
}
//加载图片
void LoadMyImage()
{
	//设置背景图
	loadimage(&pImage[bkImage], "./images/background.png", WinWidth, WinHeight);
	//加载我方飞机
	loadimage(&pImage[myPlaneImage], "./images/plane.png", MyPlaneWidth, MyPlaneHeight);
	//加载子弹
	loadimage(&pImage[bulletImage], "./images/bullet1.png", BulletWidth, BulletHeight);
	//加载敌方飞机
	loadimage(&pImage[smallFoeImage], "./images/enemy1.png", FoeWidth, FoeHeight);
	loadimage(&pImage[hugeFoeImage], "./images/enemy2.png", 150, 150);
	//加载失败图片
	loadimage(&pImage[failImage], "./images/again.png", 300, 41);
}
//初始化画面
void GameDraw()
{
	LoadMyImage();
	//背景
	putimage(0, 0, &pImage[bkImage]);
	if (!askMyHp())
	{
		putimage(WinWidth / 2-150, WinHeight / 2-20, &pImage[failImage]);
		return;
	}
	//我方
	putimage(player.x, player.y, &pImage[myPlaneImage], SRCINVERT);//最后一个参数为去背景
	transparentimage1(NULL, player.x, player.y, & pImage[myPlaneImage], BLACK);
	//子弹
	for (int i = 0; i < BulletNum; i++)
	{
		if (bullet[i].isLive)
		{
			putimage(bullet[i].x, bullet[i].y, &pImage[bulletImage], SRCINVERT);
			transparentimage2(NULL, bullet[i].x, bullet[i].y, &pImage[bulletImage]);//透明png
		}
	}
	//敌机
	for (int i = 0; i < EnemyNum; i++)
	{
		if (enemy[i].isLive)
		{
			if (enemy[i].type == BIG)
			{
				putimage(enemy[i].x, enemy[i].y, &pImage[hugeFoeImage], SRCINVERT);//最后一个参数为去背景
				transparentimage2(NULL, enemy[i].x, enemy[i].y, &pImage[hugeFoeImage]);
			}
			else
			{
				putimage(enemy[i].x, enemy[i].y, &pImage[smallFoeImage], SRCINVERT);//最后一个参数为去背景
				transparentimage2(NULL, enemy[i].x, enemy[i].y, &pImage[smallFoeImage]);
			}
		}
	}
}
//添加子弹
void CreateBullet()
{
	if (!player.isLive)
	{
		return;
	}
	for (int i = 0; i < BulletNum; i++)
	{
		if (!bullet[i].isLive)
		{
			bullet[i].x = player.x + (MyPlaneWidth - BulletWidth) / 2.0;
			bullet[i].y = player.y - BulletHeight/* * 2*/;
			bullet[i].isLive = true;
			bullet[i].width = BulletWidth;
			bullet[i].height = BulletHeight;
			break;
		}
	}
}
//子弹移动
void BulletMove(int speed)
{
	if (!player.isLive)
	{
		return;
	}
	for (int i = 0; i < BulletNum; i++)
	{
		if (bullet[i].isLive)
		{
			bullet[i].y -= speed;
			if (bullet[i].y < 0)
			{
				bullet[i].isLive = false;
			}
		}
	}
}
//设置敌机类型
void enemyHp(int enemyNo)
{
	if (rand() % 10 != 0)
	{
		enemy[enemyNo].type = SMALL;
		enemy[enemyNo].hp = 3;
		enemy[enemyNo].width = FoeWidth;
		enemy[enemyNo].height = FoeHeight;
	}
	else
	{
		enemy[enemyNo].type = BIG;
		enemy[enemyNo].hp = 5;
		enemy[enemyNo].width = FoeWidth + 100;
		enemy[enemyNo].height = FoeHeight + 100;
	}
}
//添加敌机
void CreateEnemy()
{
	for (int i = 0; i < EnemyNum; i++)
	{
		if (!enemy[i].isLive)
		{
			enemy[i].x = rand()%(WinWidth- 150);
			enemy[i].y = -100;
			enemy[i].isLive = true;
			enemyHp(i);
			enemy[i].win = 0;
			break;
		}
	}
}
//敌机移动
void EnemyMove(int speed)
{
	if (!player.isLive)
	{
		return;
	}
	for (int i = 0; i < EnemyNum; i++)
	{
		if (enemy[i].isLive)
		{
			enemy[i].y += speed;
			if (enemy[i].y > WinHeight)
			{
				enemy[i].isLive = false;
			}
		}
	}
}
void GameInit()
{
	//设置我方飞机位置
	player.x = (WinWidth - MyPlaneWidth) / 2.0;
	player.y = WinHeight - MyPlaneHeight;
	player.width = MyPlaneWidth;
	player.height = MyPlaneHeight;
	player.hp = 10;
	player.isLive = true;
	//初始化子弹
	for (int i = 0; i < BulletNum; i++)
	{
		bullet[i].x = player.x + (MyPlaneWidth - BulletWidth) / 2.0 ;
		bullet[i].y = player.y - BulletHeight*2;
		bullet[i].isLive = false;
	}
	//初始化敌机
	for (int i = 0; i < EnemyNum; i++)
	{
		enemy[i].isLive = false;
		enemyHp(i);
	}
}
//角色移动，获得键盘消息
void playerMove(int speed)
{
	if (!player.isLive)
	{
		return;
	}
#if 0
	//判断是否有键盘输入
	if (_kbhit())
	{
		char key = _getch();
		switch (key)
		{
		case 'w':
		case 'W':
			player.y -= speed;
			break;
		case 's':
		case 'S':
			player.y += speed;
			break;
		case 'a':
		case 'A':
			player.x -= speed;
			break;
		case 'd':
		case 'D':
			player.x += speed;
			break;
		default:
			break;
		}
	}
#elif 1
	//使用windowapi获取键盘输入
	//如果检测字母按键，则要用大写，这样才能检测到大写和小写
	if (GetAsyncKeyState(VK_UP)|| GetAsyncKeyState('W'))
	{
		if (player.y > 0  )
		{
			player.y -= speed;
		}
	}
	if (GetAsyncKeyState(VK_DOWN) || GetAsyncKeyState('S'))
	{
		if (player.y + MyPlaneHeight < WinHeight)
		{
			player.y += speed;
		}
	}
	if (GetAsyncKeyState(VK_LEFT) || GetAsyncKeyState('A'))
	{
		if (player.x+MyPlaneWidth/2 > 0)
		{
			player.x -= speed;
		}
	}
	if (GetAsyncKeyState(VK_RIGHT) || GetAsyncKeyState('D'))
	{
		if (player.x + MyPlaneWidth / 2 < WinWidth)
		{
			player.x += speed;
		}
	}
#endif
	//点击空格键则发射子弹
	if (GetAsyncKeyState(VK_SPACE)&& Timer(300, 2))
	{
		CreateBullet();
	}
}
//打敌机
void batEnemy()
{
	if (!player.isLive)
	{
		return;
	}
	for (int i = 0; i < EnemyNum; i++)
	{
		if (!enemy[i].isLive)
		{
			continue;
		}
		for (int j = 0; j < BulletNum; j++)
		{
			if (!bullet[j].isLive)
			{
				continue;
			}
			if (bullet[j].x >= enemy[i].x && bullet[j].x <= enemy[i].x+enemy[i].width
				&& bullet[j].y >= enemy[i].y && bullet[j].y <= enemy[i].height+enemy[i].y)
			{
				enemy[i].hp--;
				bullet[j].isLive = false;
			}
		}
		if (enemy[i].hp <= 0)
		{
			enemy[i].isLive = false;
		}
	}
}
int main()
{
	//打开窗口
	initgraph(WinWidth, WinHeight/*,SHOWCONSOLE*/);
	GameInit();
	//双缓冲绘图
	BeginBatchDraw();
	//游戏运行
	while (1)
	{
		GameDraw();
		FlushBatchDraw();
		playerMove(5);
		BulletMove(5);
		if (Timer(2000,1))
		{
			CreateEnemy();
		}
		EnemyMove(1);
		batEnemy();
	}
	EndBatchDraw();
	//关闭窗口
	closegraph();
	return 0;
}


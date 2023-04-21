#include<stdio.h>
#include<iostream>
#include<easyx.h>
#include<graphics.h>
#include<stdlib.h>
#include<assert.h>
#include <conio.h>
#include<time.h>
using namespace std;

//����ͼƬ���õ���������
//Ҫ�Ӹ��ӿ�winmm.lib
#pragma comment( lib, "MSIMG32.LIB")
//֧�������ɫ��pngͼƬ����͸��Ч��ȡ����ͼƬ������ͼƬ��ɫ��ͼƬ������ɫ�����͸����Ч�������ۿۣ�����ֺܴ�ĺ�ɫ��Ե�顣
//ʹ�� Windows GDI ����ʵ��͸��λͼ
//����1��NULL���ɣ�����2������3ΪͼƬ��������꣬����4ΪԴͼƬ��ָ�룬����5ΪҪ͸���ĵ�ɫ(��ͼƬΪ͸��ͼƬ��Ĭ��ΪBLACK)
void transparentimage1(IMAGE* dstimg, int x, int y, IMAGE* srcimg, UINT transparentcolor)
{
	HDC dstDC = GetImageHDC(dstimg);
	HDC srcDC = GetImageHDC(srcimg);
	int w = srcimg->getwidth();
	int h = srcimg->getheight();
	// ʹ�� Windows GDI ����ʵ��͸��λͼ
	TransparentBlt(dstDC, x, y, w, h, srcDC, 0, 0, w, h, transparentcolor);
}
//����Ҫָ��͸������ɫ�����ַ���͸��Ч���ǳ��ã���ֻ֧�ֵ�ɫ����Ϊ͸��ɫ��pngͼƬ������windows�´�ʱ��û�е�ɫ��pngͼƬ��
void transparentimage2(IMAGE* dstimg, int x, int y, IMAGE* srcimg) //�°�png
{
	HDC dstDC = GetImageHDC(dstimg);
	HDC srcDC = GetImageHDC(srcimg);
	int w = srcimg->getwidth();
	int h = srcimg->getheight();
	BLENDFUNCTION bf = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
	AlphaBlend(dstDC, x, y, w, h, srcDC, 0, 0, w, h, bf);
}
//��ʼ��Ϣ
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
	//�ӵ�����
	BulletNum = 15,
	//����
	WinWidth = 500,
	WinHeight = 700,
	//�ҷ��ɻ�
	MyPlaneWidth = 120,
	MyPlaneHeight = 120,
	//�ӵ�
	BulletWidth = 25,
	BulletHeight = 25,
	EnemyNum = 100,
	//�з��ɻ�
	FoeWidth = 80,
	FoeHeight = 80,
	SMALL = 1,
	BIG=3
};
//�������
struct MyPlane
{
	int x, y;
	bool isLive;//�Ƿ���
	//�л���������
	int width,height;
	int hp;
	int type;
	int win;
}player,bullet[15],enemy[EnemyNum];
//��װ��ʱ��
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
//�ж���Ϸ�Ƿ�ʧ��
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
//����ͼƬ
void LoadMyImage()
{
	//���ñ���ͼ
	loadimage(&pImage[bkImage], "./images/background.png", WinWidth, WinHeight);
	//�����ҷ��ɻ�
	loadimage(&pImage[myPlaneImage], "./images/plane.png", MyPlaneWidth, MyPlaneHeight);
	//�����ӵ�
	loadimage(&pImage[bulletImage], "./images/bullet1.png", BulletWidth, BulletHeight);
	//���صз��ɻ�
	loadimage(&pImage[smallFoeImage], "./images/enemy1.png", FoeWidth, FoeHeight);
	loadimage(&pImage[hugeFoeImage], "./images/enemy2.png", 150, 150);
	//����ʧ��ͼƬ
	loadimage(&pImage[failImage], "./images/again.png", 300, 41);
}
//��ʼ������
void GameDraw()
{
	LoadMyImage();
	//����
	putimage(0, 0, &pImage[bkImage]);
	if (!askMyHp())
	{
		putimage(WinWidth / 2-150, WinHeight / 2-20, &pImage[failImage]);
		return;
	}
	//�ҷ�
	putimage(player.x, player.y, &pImage[myPlaneImage], SRCINVERT);//���һ������Ϊȥ����
	transparentimage1(NULL, player.x, player.y, & pImage[myPlaneImage], BLACK);
	//�ӵ�
	for (int i = 0; i < BulletNum; i++)
	{
		if (bullet[i].isLive)
		{
			putimage(bullet[i].x, bullet[i].y, &pImage[bulletImage], SRCINVERT);
			transparentimage2(NULL, bullet[i].x, bullet[i].y, &pImage[bulletImage]);//͸��png
		}
	}
	//�л�
	for (int i = 0; i < EnemyNum; i++)
	{
		if (enemy[i].isLive)
		{
			if (enemy[i].type == BIG)
			{
				putimage(enemy[i].x, enemy[i].y, &pImage[hugeFoeImage], SRCINVERT);//���һ������Ϊȥ����
				transparentimage2(NULL, enemy[i].x, enemy[i].y, &pImage[hugeFoeImage]);
			}
			else
			{
				putimage(enemy[i].x, enemy[i].y, &pImage[smallFoeImage], SRCINVERT);//���һ������Ϊȥ����
				transparentimage2(NULL, enemy[i].x, enemy[i].y, &pImage[smallFoeImage]);
			}
		}
	}
}
//����ӵ�
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
//�ӵ��ƶ�
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
//���õл�����
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
//��ӵл�
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
//�л��ƶ�
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
	//�����ҷ��ɻ�λ��
	player.x = (WinWidth - MyPlaneWidth) / 2.0;
	player.y = WinHeight - MyPlaneHeight;
	player.width = MyPlaneWidth;
	player.height = MyPlaneHeight;
	player.hp = 10;
	player.isLive = true;
	//��ʼ���ӵ�
	for (int i = 0; i < BulletNum; i++)
	{
		bullet[i].x = player.x + (MyPlaneWidth - BulletWidth) / 2.0 ;
		bullet[i].y = player.y - BulletHeight*2;
		bullet[i].isLive = false;
	}
	//��ʼ���л�
	for (int i = 0; i < EnemyNum; i++)
	{
		enemy[i].isLive = false;
		enemyHp(i);
	}
}
//��ɫ�ƶ�����ü�����Ϣ
void playerMove(int speed)
{
	if (!player.isLive)
	{
		return;
	}
#if 0
	//�ж��Ƿ��м�������
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
	//ʹ��windowapi��ȡ��������
	//��������ĸ��������Ҫ�ô�д���������ܼ�⵽��д��Сд
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
	//����ո�������ӵ�
	if (GetAsyncKeyState(VK_SPACE)&& Timer(300, 2))
	{
		CreateBullet();
	}
}
//��л�
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
	//�򿪴���
	initgraph(WinWidth, WinHeight/*,SHOWCONSOLE*/);
	GameInit();
	//˫�����ͼ
	BeginBatchDraw();
	//��Ϸ����
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
	//�رմ���
	closegraph();
	return 0;
}


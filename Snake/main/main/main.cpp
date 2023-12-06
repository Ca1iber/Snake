// snakeCPP.cpp : This file contains the 'main' function. Program execution begins and ends there.
/*
Update Log
Demo1:将控制左右改变贪吃蛇方向拓展为按下上下左右控制对应上下左右，同时禁止当前运动方向的反方向按键
Demo2:计划增加奖励苹果，每四个变成奖励苹果



*/
// Calling libraries and namespaces
#include <iostream>
#include <list>
#include <thread>
using namespace std;
#include <Windows.h>
#include<vector>
// Defining the console screen size
int nScreenWidth = 120;
int nScreenHeight = 30;

// Defining the snake position using an structure
struct sSnakeSegment {
	int x;
	int y;
};

//Bonus Food
struct sBonusFood
{
	int x;
	int y;
};
std::vector<sBonusFood> BonusFood(4);

// The game!
int main()
{
	// Create Screen Buffer
	wchar_t* screen = new wchar_t[nScreenWidth * nScreenHeight];
	/*
	通过这行代码，你创建了一个可以存储游戏屏幕信息的缓冲区。你可以通过修改 screen 指针来更新游戏屏幕上的字符，然后将这些字符渲染到屏幕上，以实现游戏的显示效果。
	通常，在游戏编程中，你会在内存中创建一个虚拟的游戏屏幕，然后根据游戏逻辑不断更新它，最后将其显示在实际的屏幕上。
	*/

	for (int i = 0; i < nScreenWidth * nScreenHeight; i++) screen[i] = L' ';

	HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	/*
		代码功能为创建一个新的控制台屏幕缓冲区，返回一个HANDLE句柄以供以后操作使用
		HANDLE-->句柄：
		 系统为每个进程在内存中分配一定的区域，用来存放各个句柄，即一个个32位无符号整型值（32位操作系统中）。
		 每个32位无符号整型值相当于一个指针，指向内存中的另一个区域（我们不妨称之为区域A）。而区域A中存放的正是对象在内存中的地址。
		 当对象在内存中的位置发生变化时，区域A的值被更新，变为当前时刻对象在内存中的地址，而在这个过程中，区域A的位置以及对应句柄的值是不发生变化的。
		 1.句柄是对象生成时系统指定的，属性是只读的，程序员不能修改句柄。
		 2.不同的系统中，句柄的大小（字节数）是不同的，可以使用sizeof()来计算句柄的大小。
		 3.通过句柄，程序员只能调用系统提供的服务（即API调用），不能像使用指针那样，做其它的事。
		
		CreateConsoleScreenBuffer-->Window API中的一个函数，用与在控制台上写入字符。这个函数用于操作控制台屏幕缓冲区，并将字符写入指定位置。
		他接受五个参数：
		1.dwDesiredAccess-->对控制台屏幕缓冲区的访问。 有关访问权限的列表，参阅 "控制台缓冲区安全性和访问权限"。
			在此为   "GENERIC_READ | GENERIC_WRITE"		这是一个标志，指定了对控制台屏幕缓冲区的访问权限。
			"GENERIC_READ"	 表示可以读取缓冲区内容，"GENERIC_WRITE"	 表示可以写入缓冲区内容，因此这个参数表示你可以读取和写入缓冲区。
			使用	"|"		则是因为"GENERIC_READ"=0x80000000,"GENERIC_WRITE"=0x40000000可以在此按照按位或的操作同时置1，如果为"&"则会被同时置0

		2.dwShareMode-->此参数可以为零，表示无法共享缓冲区，也可以是以下一个或多个值:
				a.FILE_SHARE_READ 0x00000001	可以在控制台屏幕缓冲区上执行其他打开操作，以便进行读取访问。
				b.FILE_SHARE_WRITE 0x00000002	可以在主机屏幕缓冲区上执行其他打开操作，以便进行写入访问。

		3.*lpSecurityAttributes-->指向 SECURITY_ATTRIBUTES 结构的指针，该结构确定是否可由子进程继承返回的句柄。
			如果 lpSecurityAttributes 为 NULL，则无法继承句柄。 结构的 lpSecurityDescriptor 成员指定新控制台屏幕缓冲区的安全描述符。 
			如果 lpSecurityAttributes 为 NULL，则控制台屏幕缓冲区将获取默认的安全描述符。 主机屏幕缓冲区的默认安全描述符中的 ACL 来自创建者的主要令牌或模拟令牌
			此处设置为NULL表示不指定安全属性

		4.dwFlags-->要创建的控制台屏幕缓冲区的类型。 唯一支持的屏幕缓冲区类型是 CONSOLE_TEXTMODE_BUFFER。

		5.lpScreenBufferData-->保留参数，理应为NULL

		最终，CreateConsoleScreenBuffer 函数将返回一个 HANDLE 类型的句柄 hConsole，
		你可以使用这个句柄来执行与控制台屏幕缓冲区相关的操作，比如写入内容或将缓冲区设置为活动屏幕缓冲区，从而更新显示。

		通常，你可以在这个缓冲区中绘制你想要在控制台上显示的文本、图形或其他信息，然后使用 WriteConsoleOutputCharacter 或其他相关函数来将内容写入缓冲区，
		并使用 SetConsoleActiveScreenBuffer 来更新控制台屏幕上显示的内容。这是在控制台应用程序中进行文本或图形绘制的常见做法。
	
	*/
	SetConsoleActiveScreenBuffer(hConsole);
	/*
		SetConsoleActiveScreenBuffer是一个 Windows API 函数，用于设置指定的控制台屏幕缓冲区为活动屏幕缓冲区。
		这意味着设置为活动的屏幕缓冲区将用于在控制台上显示内容，而不是默认的标准输出（通常是 CMD 窗口）。

	*/
	DWORD dwBytesWritten = 0;
	/*
	DWORD 是 Windows API 中的一种数据类型，表示双字（Double Word），通常是 32 位无符号整数。
	在这里，DWORD dwBytesWritten 是一个用于输出的参数，用于存储一个操作写入的字节数。
	在 Windows API 中，DWORD 常用于表示字节数、句柄、错误码等。
	*/

	while (1) {
		// Reset to known state
		list<sSnakeSegment> snake = { {60,15},{61,15},{62,15},{63,15},{64,15} };//设置小蛇初始长度和位置
		
		//***********食物初始位置固定，可以将其优化***********//
		
		//int nFoodX = 45;
		int nFoodX = rand() % nScreenWidth;//(伪随机，没办法啦)
		//int nFoodY = 18;
		int nFoodY = (rand() % (nScreenHeight - 3)) + 3;//(伪随机)
		int BonusCount = 0;
		BonusFood[0].x = 0, BonusFood[0].y = 2;										//上左
		BonusFood[1].x = BonusFood[0].x + 1, BonusFood[1].y = BonusFood[0].y;		//上右
		BonusFood[2].x = BonusFood[0].x, BonusFood[2].y = BonusFood[0].y + 1;		//下左
		BonusFood[3].x = BonusFood[0].x + 1, BonusFood[3].y = BonusFood[0].y + 1;	//下右
		int nScore = 0;
		int nSnakeDirection = rand()%4;
		int nSnakeDirectionOld = nSnakeDirection;
		bool bDead = false;
		bool bKeyLeft = false, bKeyRight = false, bKeyLeftOld = false, bKeyRightOld = false;
		bool bKeyUp = false, bKeyDown = false, bKeyUpOld = false, bKeyDownOld = false;
		while (!bDead) {
			// Frame Timing, compensate for aspect ratio of command line
			auto t1 = chrono::system_clock::now();//记录当前时刻的系统时钟
			while ((chrono::system_clock::now() - t1) < ((nSnakeDirection % 2 == 1) ? 120ms : 200ms))
			{
				// Get Input, 
				bKeyRight = (0x8000 & GetAsyncKeyState((unsigned char)('\x27'))) != 0;
				bKeyLeft = (0x8000 & GetAsyncKeyState((unsigned char)('\x25'))) != 0;
				bKeyDown = (0x8000 & GetAsyncKeyState((unsigned char)('\x28'))) != 0;
				bKeyUp = (0x8000 & GetAsyncKeyState((unsigned char)('\x26'))) != 0;

				/*
				GetAsyncKeyState 函数：这是一个 Windows API 函数，用于获取指定键的状态。
				它接受一个键的虚拟键码作为参数，并返回一个 SHORT 类型(2Bytes,16bits)的值，表示键的状态。
				如果指定的键被按下，则返回值的最高位（最左边的位）将被设置为1，否则为0。

				(unsigned char)('\x27') 和 (unsigned char)('\x25')：这些代码将十六进制字符转换为无符号字符，表示右箭头键和左箭头键的虚拟键码。
				\x27 表示右箭头键的虚拟键码，\x25 表示左箭头键的虚拟键码。
				此处使用unsigned char 进行强制类型转换是为了保证代码的健壮性和可移植性，因为在不同的环境中虚拟键码可能有所不同

				(0x8000 & GetAsyncKeyState(...)) != 0：这是一个位运算，用于检查键的状态。
				GetAsyncKeyState 返回的状态值与 0x8000（二进制的最高位为1）进行按位与运算，以检查最高位是否被设置为1。
				如果最高位为1，表示键被按下，那么条件为真，bKeyRight 或 bKeyLeft 将被设置为 true。

				这里将等号右边的式子看成判断，就不难看出返回的是bool值，并将其赋给bKeyRight和bKeyLeft。
				*/


				//********************调整方向代码，可以尝试优化为4个方向的代码*******************//
				if (bKeyRight && !bKeyRightOld)
				{
					//nSnakeDirection++;
					//if (nSnakeDirection == 4) nSnakeDirection = 0;
					if (nSnakeDirectionOld != 3)
					{
						nSnakeDirection = 1;
					}

				}

				if (bKeyLeft && !bKeyLeftOld)
				{
					//nSnakeDirection--;
					//if (nSnakeDirection == -1) nSnakeDirection = 3;
					if (nSnakeDirectionOld != 1)
					{
						nSnakeDirection = 3;
					}

				}

				if (bKeyUp && !bKeyUpOld)
				{
					if (nSnakeDirectionOld != 2)
					{
						nSnakeDirection = 0;
					}
				}

				if (bKeyDown && !bKeyDownOld)
				{
					if (nSnakeDirectionOld != 0)
					{
						nSnakeDirection = 2;
					}
				}
				nSnakeDirectionOld = nSnakeDirection;
				bKeyRightOld = bKeyRight;
				bKeyLeftOld = bKeyLeft;
				bKeyUpOld = bKeyUp;
				bKeyDownOld = bKeyDown;
			}

			// Game logic!

			// Update the snake position and move the head to the correct place
			switch (nSnakeDirection)
			{
				//.front()返回首元素的引用，.begin()返回指向首元素的迭代器				
			case 0:// Up
				snake.push_front({ snake.front().x, snake.front().y - 1 });
				break;				
			case 1:// Right
				snake.push_front({ snake.front().x + 1, snake.front().y });
				break;				
			case 2:// Down
				snake.push_front({ snake.front().x, snake.front().y + 1 });
				break;				
			case 3:// Left
				snake.push_front({ snake.front().x - 1, snake.front().y });
				break;
			}
			/*
				这段Switch case代码控制了蛇头的移动，当蛇头面向不同方位时.向snake链表的头插入一个新元素，
				同时结合后面的pop_back()方法弹出蛇尾就可以在视觉上形成蛇前进的效果
			*/



			//Collision detect snake food(NORMAL)
			if (snake.front().x == nFoodX && snake.front().y == nFoodY) //食物碰撞检测，如果食物和头坐标相同说明蛇吃到了食物
			{
				nScore++;
				BonusCount++;
				//if (BonusCount != 4)//当没有吃满四个时，食物正常生成
				{
					while (screen[nFoodY * nScreenWidth + nFoodX] != L' ') 
					{
						nFoodX = rand() % nScreenWidth;
						nFoodY = (rand() % (nScreenHeight - 3)) + 3;
					}
				}
				/*
				生成新的食物坐标代码，用于避免食物生成在蛇的体内，可以如此理解：当食物的坐标不是空格时，进入循环重新生成随机食物坐标，直至生成的位置是空格不满足循环
				此时确定食物坐标退出循环
				*/

				for (int i = 0; i < 5; i++)
					snake.push_back({ snake.back().x, snake.back().y });
				//吃到食物之后在链表尾部增加5个尾部坐标，当贪吃蛇向前移动时可以视为长度增长
			}

			//generate snake food(BONUS)
			//if (BonusCount == 4)
			//{
			//	while (screen[BonusFood[0].y * nScreenWidth + BonusFood[0].x] != L' ' &&
			//		screen[BonusFood[1].y * nScreenWidth + BonusFood[1].x] != L' ' &&
			//		screen[BonusFood[2].y * nScreenWidth + BonusFood[2].x] != L' ' &&
			//		screen[BonusFood[3].y * nScreenWidth + BonusFood[3].x] != L' ' ||//BonusFood的四个角不全为空时
			//		!(BonusFood[0].x < nScreenWidth - 1) && (BonusFood[0].y < 3) && (BonusFood[0].y == nScreenHeight))//BonusFood左上格不符合位置要求时
			//	{
			//		BonusFood[0].x = rand() % (nScreenWidth - 1);
			//		BonusFood[0].y = (rand() % (nScreenHeight - 2)) + 3;
			//		BonusFood[1].x = BonusFood[0].x + 1, BonusFood[1].y = BonusFood[0].y;		//上右
			//		BonusFood[2].x = BonusFood[0].x, BonusFood[2].y = BonusFood[0].y + 1;		//下左
			//		BonusFood[3].x = BonusFood[0].x + 1, BonusFood[3].y = BonusFood[0].y + 1;	//下右

			//	}
			//	nFoodX = 0;
			//	nFoodY = 0;
			//}

			// Collision detect snake world
			//蛇身与边界的检测，后期也许......可以优化？
			if (snake.front().x < 0 || snake.front().x >= nScreenWidth)
				bDead = true;
			if (snake.front().y < 3 || snake.front().y >= nScreenHeight)
				bDead = true;

			// Collision detect snake vs snake(Game over!)
			for (list<sSnakeSegment>::iterator i = snake.begin(); i != snake.end(); i++)
				if (i != snake.begin() && i->x == snake.front().x && i->y == snake.front().y)
					bDead = true;

			// Snake chops it's own tail
			//通过将尾部弹出实现蛇身向前移动
			snake.pop_back();



			// Screen design(super easy and simple)

			// Clear screen
			for (int i = 0; i < nScreenWidth * nScreenHeight; i++) screen[i] = L' ';

			// Printing stats and borders for the game
			for (int i = 0; i < nScreenWidth; i++) {
				screen[i] = L' ';//第一行
				screen[2 * nScreenWidth + i] = L'=';//第三行
			}
			wsprintf(&screen[nScreenWidth + 5], L"Snake Game demo by Sulyvahn   Score: %d", nScore);//第二行
			/*
			wsprintf 函数的作用是将格式化的文本消息填充到指定位置的字符数组中，以便后续在屏幕上显示。
			在这个例子中，它将游戏标题和分数格式化后写入到屏幕上的特定位置，用于显示游戏的状态信息。
			这是游戏界面更新的一部分，用于向玩家显示相关信息。
			*/

			// Print the snake
			for (auto s : snake)
				screen[s.y * nScreenWidth + s.x] = bDead ? L'+' : L'O';

			// Print the snake's head
			screen[snake.front().y * nScreenWidth + snake.front().x] = bDead ? L'X' : L'@';

			// Give it food, mostly apples :)
			//食物坐标
			screen[nFoodY * nScreenWidth + nFoodX] = L'$';

			if (bDead)
				wsprintf(&screen[3 * nScreenWidth + 40], L"    按下空格键重新游玩    ");

			// Show the game!
			//使用WriteConsoleOutputCharacter来更新写入屏幕缓冲区内容，
			//使用 SetConsoleActiveScreenBuffer 来更新控制台屏幕上显示的内容
			WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);
			/*
			 HANDLE  hConsoleOutput,      // 控制台屏幕缓冲区的句柄。控制台屏幕缓冲区的句柄。 此句柄必须具有 GENERIC_WRITE 访问权限。hConsole具有此权限
			 LPCTSTR lpCharacter,         // 要写入的字符数据。为下一时刻screen上的数据。
			 DWORD   nLength,            // 要写入的字符数。将屏幕缓冲区选中位置重新写入
			 COORD   dwWriteCoord,       // 写入操作的起始坐标，{0,0}从头开始
			 LPDWORD lpNumberOfCharsWritten // 用于接收成功写入的字符数
			*/
		}

		// Wait for the user to hit space to restart the game
		//当没有检测到输入空格时循环等待用户输入
		while ((0x8000 & GetAsyncKeyState((unsigned char)('\x20'))) == 0);
	}

	return 0;
}
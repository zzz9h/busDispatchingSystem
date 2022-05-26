	#include<stdio.h>
	#include<stdlib.h>
	#include<string.h>
	#pragma warning (disable:4996)
	char WaitForSolve[20][25];
	int from[20], to[20], IsSolve[20],time;
	struct need 
	{
		  char require[30];
		  struct need* nextptr;
		  int fromstation;
		  int tostation;
	};
	struct bus 
	{
		  int pos;
		  int Direction;//1为正，-1为负
		  int CurTragetStationfrom;
		  int CurTragetStationto;
		  int CurTragetStationIsReverse;//针对clock到达信息发出地后是否反向
		  char target[30];
		  bool IsTask;
		  bool IsWait;
	};
	struct station 
	{
		  char clockwise[30];
		  char counterclockwise[30];
	};
	bus initBus() 
	{
		  bus Bus;
		  Bus.pos = 0;
		  Bus.Direction = 1;
		  Bus.IsWait = false;
		  Bus.IsTask = false;
		  Bus.CurTragetStationfrom = -1;
		  Bus.CurTragetStationto = -1;
		  Bus.CurTragetStationIsReverse = 0;
		  for (int i = 0; i <= 10; i++) {
			  if (i == 10) {
				  Bus.target[i] = '\0';
			  }
			  else {
				  Bus.target[i] = '0';
			  }
		  }
		  return Bus;
	}
	station initStation() 
	{
		  station Station;
		  for (int i = 0; i <= 10; i++) {
				if (i == 10) {
					Station.clockwise[i] = '\0';
					Station.counterclockwise[i] = '\0';
				}
			  else
				{
					Station.clockwise[i] = '0';
					Station.counterclockwise[i] = '0';
			  }
		  }
		  return Station;
	}
	typedef struct need  demand;
	typedef struct bus  bus;
	typedef struct station  station;
	void printBusState(bus Bus) 
	{
		  printf("BUS:\n");
		  printf("position: %d\n", Bus.pos);
		  printf("target: %s\n", Bus.target);
	}
	void printStationState(station Station) 
	{
		  printf("STATION:\n");
		  printf("clockwise: %s\n", Station.clockwise);
		  printf("counterclockwise: %s\n", Station.counterclockwise);
	}
	demand* creatRequire(demand* headptr)
	{
		  char inputRequire[30];
		  int from, to;
		  headptr = NULL;
		  demand* lastptr = NULL;
		  demand* currentptr = NULL;
		  while (true) 
		  {
				currentptr = (demand*)malloc(sizeof(demand));
				scanf("%s", inputRequire);
				if (strcmp(inputRequire, "end") == 0) 
				{
					break;
				}
				else if (strcmp(inputRequire, "clock") == 0) 
				{
					strcpy(currentptr->require, inputRequire);
				}
				else if (strcmp(inputRequire, "counterclockwise") == 0) 
				{
					strcpy(currentptr->require, inputRequire);
					scanf("%d", &from);
					currentptr->fromstation = from;
					currentptr->tostation = -1;
				}
				else if (strcmp(inputRequire, "target") == 0) 
				{
					strcpy(currentptr->require, inputRequire);
					scanf("%d", &to);
					currentptr->tostation = to;
					currentptr->fromstation = -1;
				}
				else if (strcmp(inputRequire, "clockwise") == 0) 
				{
					strcpy(currentptr->require, inputRequire);
					scanf("%d", &from);
					currentptr->fromstation = from;
					currentptr->tostation = -1;
				}
				if (headptr == NULL)
				{
					headptr = currentptr;
					lastptr = currentptr;
				}
				else 
				{
					lastptr->nextptr = currentptr;
					lastptr = currentptr;
				}
			}
		  lastptr->nextptr = NULL;
		  return headptr;
	}
	int findNearestDirection(bus Bus, int trapos) 
	{
		int nowpos = Bus.pos;
		int d1, d2;
		if (nowpos > trapos)
		{
			d1 = nowpos - trapos;
			d2 = 20 - d1;
			if (d1 >= d2)
			{
				return 1;
			}
			else
			{
				return -1;
			}
		}
		else
		{
			d2 = trapos - nowpos;
			d1 = 20 - d2;
			if (d1 >= d2)
			{
				return 1;
			}
			else 
			{
				return -1;
			}
		}
	}
	void putInList(char WaitForSolve[][25], int from[], int to[], demand* require)
	{
		int i = 1;
		for (i = 1; i < 20; i++) 
		{
			if (WaitForSolve[i][0] == '\0' && IsSolve[i] != 1)
			{
				strcpy(WaitForSolve[i], require->require);
				from[i] = require->fromstation;
				to[i] = require->tostation;
				break;
			}
			else 
			{
				continue;
			}
		}
	}
	void solveWaitedRequire(bus& Bus) 
	{
		int i = 1;
		for (i = 1; i < 20; i++)
		{
			if (WaitForSolve[i][0] == '\0') 
			{
				continue;
			}
			if (strcmp(WaitForSolve[i], "clockwise") == 0) 
			{
				if (Bus.pos == (from[i] - 1) * 2) 
				{
					from[i] = 0, to[i] = 0, WaitForSolve[i][0] = '\0', IsSolve[i] = 1;
					continue;
				}
				else
				{
					Bus.IsTask = true;
					Bus.CurTragetStationfrom = (from[i] - 1) * 2;
					Bus.Direction = findNearestDirection(Bus, (from[i] - 1) * 2);
					Bus.CurTragetStationIsReverse = 1;
					from[i] = 0, to[i] = 0, WaitForSolve[i][0] = '\0', IsSolve[i] = 1;
					return;
				}	
			}
			else if (strcmp(WaitForSolve[i], "counterclockwise") == 0) 
			{
				if (Bus.pos == (from[i] - 1) * 2)
				{
					from[i] = 0, to[i] = 0, WaitForSolve[i][0] = '\0', IsSolve[i] = 1;
					continue;
				}
				else
				{
					Bus.IsTask = true;
					Bus.CurTragetStationfrom = (from[i] - 1) * 2;
					Bus.Direction = findNearestDirection(Bus, (from[i] - 1) * 2);
					Bus.CurTragetStationIsReverse = -1;
					from[i] = 0, to[i] = 0, WaitForSolve[i][0] = '\0', IsSolve[i] = 1;
					return;
				}
			}
			else if (strcmp(WaitForSolve[i], "target") == 0)
			{
				if (Bus.pos == (to[i] - 1) * 2) 
				{
					from[i] = 0, to[i] = 0, WaitForSolve[i][0] = '\0', IsSolve[i] = 1;
					continue;
				}
				else
				{
					Bus.IsTask = true;
					Bus.CurTragetStationto = (to[i] - 1) * 2;
					Bus.Direction = findNearestDirection(Bus, (to[i] - 1) * 2);
					Bus.CurTragetStationIsReverse = 0;
					from[i] = 0, to[i] = 0, WaitForSolve[i][0] = '\0', IsSolve[i] = 1;
					return;
				}
			}
		}
		return;
	}
	void Move(bus& Bus)
	{
		if (Bus.Direction == -1)
		{
			Bus.pos -= 1;
			if (Bus.pos < 0) 
			{
				Bus.pos = 20 + Bus.pos;
			}
		}
		else if (Bus.Direction == 1) 
		{
			Bus.pos += 1;
			if (Bus.pos >= 20) 
			{
				Bus.pos = Bus.pos - 20;
			}
		}
	}
	void STATE(bus Bus, demand* headptr, station Station) 
	{
		demand* currentptr = headptr;
		while (currentptr != NULL)
		{
			if (strcmp(currentptr->require, "clock") == 0)
			{
				time++;
				if (Bus.IsTask)
				{
					if (!Bus.IsWait) {//接客，送客都需要停一秒
						Move(Bus);
					}
				}
				printf("Time:%d\n", time);
				printBusState(Bus);
				printStationState(Station);
				if (Bus.IsWait)
				{//说明等完了一秒后，可以发车了
					Bus.IsWait = false;
				}
			}
			else if (strcmp(currentptr->require, "clockwise") == 0) 
			{
				if (!Bus.IsTask)
				{
					Bus.Direction = findNearestDirection(Bus, (currentptr->fromstation - 1) * 2);
					Bus.IsTask = true;
					Bus.CurTragetStationfrom = (currentptr->fromstation - 1) * 2;
					Bus.CurTragetStationIsReverse = 1;
				}
				else if (Bus.IsTask)
				{
					if (
				!(Bus.CurTragetStationfrom == (currentptr->fromstation - 1) * 2 && Bus.CurTragetStationIsReverse == 1)) 
					{
						putInList(WaitForSolve, from, to, currentptr);
					}
				}
				Station.clockwise[(currentptr->fromstation) - 1] = '1';
			}
			else if (strcmp(currentptr->require, "target") == 0)
			{
				if (!Bus.IsTask)
				{
					Bus.IsTask = true;
					Bus.Direction = findNearestDirection(Bus, (currentptr->tostation - 1) * 2);
					Bus.CurTragetStationto = (currentptr->tostation - 1) * 2;
				}
				else 
				{
					if (
						!(Bus.CurTragetStationto== (currentptr->tostation - 1) * 2))
					{
						putInList(WaitForSolve, from, to, currentptr);
					}
				
				}
				Bus.target[(currentptr->tostation) - 1] = '1';
			}
			else if (strcmp(currentptr->require, "counterclockwise") == 0)
			{
				if (!Bus.IsTask) 
				{
					Bus.Direction = findNearestDirection(Bus, (currentptr->fromstation - 1) * 2);
					Bus.IsTask = true;
					Bus.CurTragetStationfrom = (currentptr->fromstation - 1) * 2;
					Bus.CurTragetStationIsReverse = -1;
				}
				else if (Bus.IsTask)
				{
					if (
				!(Bus.CurTragetStationfrom == (currentptr->fromstation - 1) * 2 &&Bus.CurTragetStationIsReverse==-1)) 
					{
						putInList(WaitForSolve, from, to, currentptr);
					}
				}
				Station.counterclockwise[(currentptr->fromstation) - 1] = '1';
			}

			if (strcmp(currentptr->require, "clock") == 0)
			{//避免出现其他指令误判的情况，位置判断以秒为单位
				if (Bus.CurTragetStationfrom == Bus.pos)
				{
					Bus.CurTragetStationfrom = -1;
					if (Bus.CurTragetStationIsReverse == -1) 
					{
						Station.counterclockwise[(Bus.pos / 2)] = '0';
					}
					else if (Bus.CurTragetStationIsReverse == 1) 
					{
						Station.clockwise[(Bus.pos / 2)] = '0';
					}
					Bus.IsTask = false;
					Bus.IsWait = true;
				}
				if (Bus.CurTragetStationto == Bus.pos) 
				{
					Bus.CurTragetStationto = -1;
					Bus.target[(Bus.pos / 3)] = '0';
					Bus.IsTask = false;
					Bus.IsWait = true;
				}
			}
			if (!Bus.IsTask)
			{//搜寻待处理指令时是需要随时搜寻的，不用按秒判断
				solveWaitedRequire(Bus);
			}
			currentptr = currentptr->nextptr;
		}
	}
	void FreeRequire(demand* headptr)
	{
		demand* currentptr, * temp;
		currentptr = headptr;
		while (currentptr != NULL)
		{
			temp = currentptr->nextptr;
			free(currentptr);
			currentptr = temp;
		}
	}
	int main() 
	{
		for (int i = 1; i < 20; i++)
		{
			WaitForSolve[i][0] = '\0';
			from[i] = 0;
			to[i] = 0;
			IsSolve[i] = 0;
			}
		time = 0;
		demand* headptr = NULL;
		headptr = creatRequire(headptr);
		bus Bus;
		station Station;
		Bus = initBus();
		Station = initStation();
		printf("Time:%d\n", time);
		printBusState(Bus);
		printStationState(Station);
		STATE(Bus, headptr, Station);
		printf("end\n");
		FreeRequire(headptr);
		return 0;
	}

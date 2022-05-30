#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>

#define MAXN 103 //最大数组元素数
char STRATEGY; //配置文件中策略名称的末尾字母
int TIME; //绝对时间
int TOTAL_STATION;
int GAP; //每站之间的距离
int LENGTH; //轨道总长度

char WaitForSolve[MAXN][25];
int from[MAXN], to[MAXN], IsSolve[MAXN];

typedef struct Request {
    char content[MAXN];
    struct Request *nextPtr;
    struct Request *prePtr; //指向上一个需求
    int fromStation;
    int toStation;
    int isServed[MAXN];
} R;

typedef struct Bus {
    int pos;
    int isClockwise;//1为正，-1为负
    int curTragetStationfrom;
    int curTragetStationto;
    int isCurStationReverse;//针对clock到达信息发出地后是否反向
    char target[MAXN];
    bool isTask, isTempWait, isTargetWait; //记录运行与等待状态
} B;

typedef struct Station {
    char clockwise[MAXN];
    char counterclockwise[MAXN];
} S;


B initBus() {
    B Bus;
    Bus.pos = 0;
    Bus.isClockwise = 1;
    Bus.isTargetWait = false;
    Bus.isTask = false;
    Bus.curTragetStationfrom = -1;
    Bus.curTragetStationto = -1;
    Bus.isCurStationReverse = 0;
    int i;
    for (i = 0; i <= TOTAL_STATION; i++) {
        if (i == TOTAL_STATION) {
            Bus.target[i] = '\0';
        } else {
            Bus.target[i] = '0';
        }
    }
    return Bus;
}

S initStation() {
    S Station;
    int i;
    for (i = 0; i <= TOTAL_STATION; i++) {
        if (i == TOTAL_STATION) {
            Station.clockwise[i] = '\0';
            Station.counterclockwise[i] = '\0';
        } else {
            Station.clockwise[i] = '0';
            Station.counterclockwise[i] = '0';
        }
    }
    return Station;
}

R *headPtr; //创建Request类型链表的虚拟头结点，用于存放指令
B Bus; //模拟公交车
S Station; //模拟站台

void output(B *Bus, S *Station) {
    printf("Time: %d\n", TIME);
    printf("BUS:\n");
    printf("position: %d\n", Bus->pos);
    printf("target: %s\n", Bus->target);
    printf("STATION:\n");
    printf("clockwise: %s\n", Station->clockwise);
    printf("counterclockwise: %s\n", Station->counterclockwise);
}

R *creatRequire(R *headPtr) {
    char inputRequire[30];
    int from, to;
    headPtr = NULL;
    R *lastPtr = NULL;
    R *currentPtr = NULL;
    while (true) {
        currentPtr = (R *) malloc(sizeof(R));
        scanf("%s", inputRequire);
        if (strcmp(inputRequire, "end") == 0) {
            break;
        } else if (strcmp(inputRequire, "clock") == 0) {
            strcpy(currentPtr->content, inputRequire);
        } else if (strcmp(inputRequire, "counterclockwise") == 0) {
            strcpy(currentPtr->content, inputRequire);
            scanf("%d", &from);
            currentPtr->fromStation = from;
            currentPtr->toStation = -1;
        } else if (strcmp(inputRequire, "target") == 0) {
            strcpy(currentPtr->content, inputRequire);
            scanf("%d", &to);
            currentPtr->toStation = to;
            currentPtr->fromStation = -1;
        } else if (strcmp(inputRequire, "clockwise") == 0) {
            strcpy(currentPtr->content, inputRequire);
            scanf("%d", &from);
            currentPtr->fromStation = from;
            currentPtr->toStation = -1;
        }
        if (headPtr == NULL) {
            headPtr = currentPtr;
            lastPtr = currentPtr;
        } else {
            lastPtr->nextPtr = currentPtr;
            lastPtr = currentPtr;
        }
    }
    lastPtr->nextPtr = NULL;
    return headPtr;
}

int findNearestDirection(B *Bus, int trapos) {
    int nowpos = Bus->pos;
    int d1, d2;
    if (nowpos > trapos) {
        d1 = nowpos - trapos;
        d2 = LENGTH - d1;
        if (d1 >= d2) {
            return 1;
        } else {
            return -1;
        }
    } else {
        d2 = trapos - nowpos;
        d1 = LENGTH - d2;
        if (d1 >= d2) {
            return 1;
        } else {
            return -1;
        }
    }
}

void putInList(char WaitForSolve[][25], int from[], int to[], R *require) {
    int i = 1;
    for (i = 1; i < MAXN; i++) {
        if (WaitForSolve[i][0] == '\0' && IsSolve[i] != 1) {
            strcpy(WaitForSolve[i], require->content);
            from[i] = require->fromStation;
            to[i] = require->toStation;
            break;
        } else {
            continue;
        }
    }
}

void solveWaitedRequire(B *Bus) {
    int i = 1;
    for (i = 1; i < MAXN; i++) {
        if (WaitForSolve[i][0] == '\0') {
            continue;
        }
        if (strcmp(WaitForSolve[i], "clockwise") == 0) {
            if (Bus->pos == (from[i] - 1) * GAP) {
                from[i] = 0, to[i] = 0, WaitForSolve[i][0] = '\0', IsSolve[i] = 1;
                continue;
            } else {
                Bus->isTask = true;
                Bus->curTragetStationfrom = (from[i] - 1) * GAP;
                Bus->isClockwise = findNearestDirection(Bus, (from[i] - 1) * GAP);
                Bus->isCurStationReverse = 1;
                from[i] = 0, to[i] = 0, WaitForSolve[i][0] = '\0', IsSolve[i] = 1;
                return;
            }
        } else if (strcmp(WaitForSolve[i], "counterclockwise") == 0) {
            if (Bus->pos == (from[i] - 1) * GAP) {
                from[i] = 0, to[i] = 0, WaitForSolve[i][0] = '\0', IsSolve[i] = 1;
                continue;
            } else {
                Bus->isTask = true;
                Bus->curTragetStationfrom = (from[i] - 1) * GAP;
                Bus->isClockwise = findNearestDirection(Bus, (from[i] - 1) * GAP);
                Bus->isCurStationReverse = -1;
                from[i] = 0, to[i] = 0, WaitForSolve[i][0] = '\0', IsSolve[i] = 1;
                return;
            }
        } else if (strcmp(WaitForSolve[i], "target") == 0) {
            if (Bus->pos == (to[i] - 1) * GAP) {
                from[i] = 0, to[i] = 0, WaitForSolve[i][0] = '\0', IsSolve[i] = 1;
                continue;
            } else {
                Bus->isTask = true;
                Bus->curTragetStationto = (to[i] - 1) * GAP;
                Bus->isClockwise = findNearestDirection(Bus, (to[i] - 1) * GAP);
                Bus->isCurStationReverse = 0;
                from[i] = 0, to[i] = 0, WaitForSolve[i][0] = '\0', IsSolve[i] = 1;
                return;
            }
        }
    }
    return;
}

void Move(B *Bus) {
    if (Bus->isClockwise == -1) {
        Bus->pos -= 1;
        if (Bus->pos < 0) {
            Bus->pos = LENGTH + Bus->pos;
        }
    } else if (Bus->isClockwise == 1) {
        Bus->pos += 1;
        if (Bus->pos >= LENGTH) {
            Bus->pos = Bus->pos - LENGTH;
        }
    }
}

void STATE(B *Bus, R *headPtr, S *Station) {
    R *currentPtr = headPtr;
    while (currentPtr != NULL) {
        if (strcmp(currentPtr->content, "clock") == 0) {
            TIME++;
            if (Bus->isTask) {
                if (!Bus->isTempWait) {//接客，送客都需要停一秒
                    Move(Bus);
                }
            }
            output(Bus, Station);
            if (Bus->isTempWait) {//说明等完了一秒后，可以发车了
                Bus->isTempWait = false;
            }
        } else if (strcmp(currentPtr->content, "clockwise") == 0) {
            if (!Bus->isTask) {
                Bus->isClockwise = findNearestDirection(Bus, (currentPtr->fromStation - 1) * GAP);
                Bus->isTask = true;
                Bus->curTragetStationfrom = (currentPtr->fromStation - 1) * GAP;
                Bus->isCurStationReverse = 1;
            } else if (Bus->isTask) {
                if (
                        !(Bus->curTragetStationfrom == (currentPtr->fromStation - 1) * GAP &&
                          Bus->isCurStationReverse == 1)) {
                    putInList(WaitForSolve, from, to, currentPtr);
                }
            }
            Station->clockwise[(currentPtr->fromStation) - 1] = '1';
        } else if (strcmp(currentPtr->content, "target") == 0) {
            if (!Bus->isTask) {
                Bus->isTask = true;
                Bus->isClockwise = findNearestDirection(Bus, (currentPtr->toStation - 1) * GAP);
                Bus->curTragetStationto = (currentPtr->toStation - 1) * GAP;
            } else {
                if (
                        !(Bus->curTragetStationto == (currentPtr->toStation - 1) * GAP)) {
                    putInList(WaitForSolve, from, to, currentPtr);
                }

            }
            Bus->target[(currentPtr->toStation) - 1] = '1';
        } else if (strcmp(currentPtr->content, "counterclockwise") == 0) {
            if (!Bus->isTask) {
                Bus->isClockwise = findNearestDirection(Bus, (currentPtr->fromStation - 1) * GAP);
                Bus->isTask = true;
                Bus->curTragetStationfrom = (currentPtr->fromStation - 1) * GAP;
                Bus->isCurStationReverse = -1;
            } else if (Bus->isTask) {
                if (
                        !(Bus->curTragetStationfrom == (currentPtr->fromStation - 1) * GAP &&
                          Bus->isCurStationReverse == -1)) {
                    putInList(WaitForSolve, from, to, currentPtr);
                }
            }
            Station->counterclockwise[(currentPtr->fromStation) - 1] = '1';
        }

        if (strcmp(currentPtr->content, "clock") == 0) {//避免出现其他指令误判的情况，位置判断以秒为单位
            if (Bus->curTragetStationfrom == Bus->pos) {
                Bus->curTragetStationfrom = -1;
                if (Bus->isCurStationReverse == -1) {
                    Station->counterclockwise[(Bus->pos / GAP)] = '0';
                } else if (Bus->isCurStationReverse == 1) {
                    Station->clockwise[(Bus->pos / GAP)] = '0';
                }
                Bus->isTask = false;
                Bus->isTempWait = true;
            }
            if (Bus->curTragetStationto == Bus->pos) {
                Bus->curTragetStationto = -1;
                Bus->target[(Bus->pos / GAP)] = '0';
                Bus->isTask = false;
                Bus->isTempWait = true;
            }
        }
        if (!Bus->isTask) {//搜寻待处理指令时是需要随时搜寻的，不用按秒判断
            solveWaitedRequire(Bus);
        }
        currentPtr = currentPtr->nextPtr;
    }
}

void FreeRequire(R *headPtr) {
    R *currentPtr, *temp;
    currentPtr = headPtr;
    while (currentPtr != NULL) {
        temp = currentPtr->nextPtr;
        free(currentPtr);
        currentPtr = temp;
    }
}

int main() {
    int i;
    for (i = 1; i < MAXN; i++) {
        WaitForSolve[i][0] = '\0';
        from[i] = 0;
        to[i] = 0;
        IsSolve[i] = 0;
    }
    headPtr = creatRequire(headPtr);
    Bus = initBus();
    Station = initStation();
    B *bus = &Bus;
    S *station = &Station;
    output(bus, station);
    STATE(bus, headPtr, station);
    printf("end\n");
    FreeRequire(headPtr);
    return 0;
}

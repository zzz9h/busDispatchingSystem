#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "stdbool.h"

#define MAXN 33 //最大数组元素数

char STRATEGY; //配置文件中策略名称的末尾字母
int TIME = 0; //绝对时间
int TOTAL_STATION = 5;
int GAP = 3; //每站之间的距离
int LENGTH = 15; //轨道总长度

//模拟用户需求
typedef struct Request {
    char content[MAXN]; //记录需求
    int isServed; //记录是否已经满足
    struct Request *nextPtr; //指向下一个需求
    struct Request *prePtr; //指向上一个需求
    int fromStation; //记录发出请求的车站
    int toStation; //记录target
} R;

//模拟公交车
typedef struct Bus {
    int isTask, isTempWait, isTargetWait; //记录运行与等待状态
    int isClockwise; //记录是否顺时针运行
    int isCurStationReverse; //记录到达站点后是否需要反向
    int pos; //记录位置
    int curTargetStationFrom, curTargetStationTo; //记录当前需要到达的上车车站或target车站
    char target[MAXN]; //记录车内站点请求情况
} B;

//模拟公交站台
typedef struct Station {
    char clockwise[MAXN]; //记录当前车站顺时针请求
    char counterclockwise[MAXN]; //记录当前车站逆时针请求
} S;

//初始化公交车状态
B initBus() {
    B bus = {
            .isTask =  false,
            .isTempWait =  false,
            .isTargetWait = false,
            .isClockwise =  true,
            .isCurStationReverse =  false,
            .pos =  0,
            .curTargetStationFrom =  -1,
            .curTargetStationTo =  -1,
    };
    int i = 0;
    while (i < TOTAL_STATION)
        bus.target[i++] = '0';
    bus.target[i] = '\0';
    return bus;
}

//初始化需求状态
R initRequest() {
    R request = {
            .isServed = false,
            .nextPtr = NULL,
            .prePtr = NULL,
            .fromStation = -1,
            .toStation = -1,
    };
    return request;
}

//初始化站台状态
S initStation() {
    S station;
    int i = 0;
    while (i < TOTAL_STATION) {
        station.clockwise[i] = '0';
        station.counterclockwise[i] = '0';
        i++;
    }
    station.clockwise[i] = station.counterclockwise[i] = '\0';
    return station;
}

R *headPtr; //创建Request类型链表的虚拟头结点，用于存放指令
B bus; //模拟公交车
S station; //模拟站台

R *createRequire() {
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


void sstfMove() {
    if (bus.isClockwise == true) {
        bus.pos = (bus.pos + 1) % LENGTH;
    } else {
        bus.pos = (bus.pos - 1 + LENGTH) % LENGTH;
    }
}

void output() {
    printf("TIME:%d\n", TIME);

    puts("BUS:");
    printf("position:%d\n", bus.pos);
    printf("target:%s\n", bus.target);

    puts("STATION:");
    printf("clockwise:%s\n", station.clockwise);
    printf("counterclockwise:%s\n", station.counterclockwise);
}

void sstfFindNearestRequest() {
    int i = bus.pos / GAP, j = bus.pos / GAP;
    while (true) {
        if (bus.target[j] == '1' || station.clockwise[j] == '1' || station.counterclockwise[j] == '1') {
            bus.isTask = true;
            bus.curTargetStationTo = j * GAP;
            bus.isClockwise = true;
            break;
        } else if (bus.target[i] == '1' || station.clockwise[i] == '1' || station.counterclockwise[i] == '1') {
            bus.isTask = true;
            bus.curTargetStationTo = i * GAP;
            bus.isClockwise = false;
            break;
        }
        int tmp1 = i, tmp2 = j;
        i = (i - 1 + TOTAL_STATION) % TOTAL_STATION;
        j = (j + 1) % TOTAL_STATION;
        if (i == j || (j == tmp1 && i == tmp2)) {
            bus.isTask = false;
            break;
        }
    }
}

void SSTF() {
    R *currentPtr = headPtr;
    while (currentPtr != NULL) {
        if (strcmp(currentPtr->content, "clock") == 0) {
            TIME++;
            if (!bus.isTask)
                sstfFindNearestRequest();
            if (bus.isTask && !bus.isTempWait && !bus.isTargetWait)
                sstfMove();
            if (bus.isTempWait)
                bus.isTempWait = false;
            if (bus.isTargetWait)
                bus.isTargetWait = false;
            output();
            if (bus.pos == bus.curTargetStationTo &&
                (bus.target[bus.pos / GAP] == '1' || station.clockwise[bus.pos / GAP] == '1'
                 || station.counterclockwise[bus.pos / GAP] == '1')) {
                bus.isTargetWait = true;
            } else if (bus.isClockwise == true && bus.pos % GAP == 0
                       && (bus.target[bus.pos / GAP] == '1' || station.clockwise[bus.pos / GAP] == '1')) {
                bus.isTempWait = true;
            } else if (bus.isClockwise == false && bus.pos % GAP == 0
                       && (bus.target[bus.pos / GAP] == '1' || station.counterclockwise[bus.pos / GAP] == '1')) {
                bus.isTempWait = true;
            }
            if (bus.isTargetWait) {
                int tmp = bus.pos / GAP;
                station.clockwise[tmp] = station.counterclockwise[tmp] = bus.target[tmp] = '0';
                bus.curTargetStationTo = -1;
                bus.isTask = false;
//                sstfFindNearestRequest();
            }
            if (bus.isTempWait) {
                if (bus.isClockwise == true) {
                    int tmp = bus.pos / GAP;
                    station.clockwise[tmp] = bus.target[tmp] = '0';
                } else {
                    int tmp = bus.pos / GAP;
                    station.counterclockwise[tmp] = bus.target[tmp] = '0';
                }
            }
        } else if (strcmp(currentPtr->content, "clockwise") == 0) {
            station.clockwise[currentPtr->fromStation - 1] = '1';
        } else if (strcmp(currentPtr->content, "counterclockwise") == 0) {
            station.counterclockwise[currentPtr->fromStation - 1] = '1';
        } else if (strcmp(currentPtr->content, "target") == 0) {
            bus.target[currentPtr->toStation - 1] = '1';
        }
        currentPtr = currentPtr->nextPtr;
    }

}

void freeRequire() {
    R *currentPtr, *temp;
    currentPtr = headPtr;
    while (currentPtr != NULL) {
        temp = currentPtr->nextPtr;
        free(currentPtr);
        currentPtr = temp;
    }
}

int main() {
    bus = initBus();
    station = initStation();
    headPtr = createRequire();
    output();
    SSTF();
    puts("end");
    freeRequire();
    return 0;
}

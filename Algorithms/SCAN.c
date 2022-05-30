#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<stdbool.h>
#define MAXN 103 //最大数组元素数
char STRATEGY; //配置文件中策略名称的末尾字母
int TIME = 0; //绝对时间
int TOTAL_STATION = 6; 
int GAP = 4; //每站之间的距离
int LENGTH = 24; //轨道总长度。

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
    {
        bus.target[i++] = '0';
    }
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
S station; //模拟站台。以上初始化

R *createRequire() {
    char inputRequire[30];
    int from, to;
    headPtr = NULL;
    R *lastPtr = NULL;
    R *currentPtr = NULL;
    while (true) {
        currentPtr = (R *) malloc(sizeof(R));
        *currentPtr = initRequest();
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
//请求更新
int reRequest(int time){
    int i = 0, isEnd = 0;
    R *presentPtr;
    presentPtr = headPtr;
    //循环到上一个clock结束的指令
    while (i != time){
        if (strcmp(presentPtr->content, "clock") == 0)
        {
            i ++;
        }
        presentPtr = presentPtr->nextPtr;
    }
    if (presentPtr == NULL)
    {
        isEnd = 1;
    }
    //改请求列表
    while (isEnd != 1 && strcmp(presentPtr->content, "clock") != 0)
    {
        if (strcmp(presentPtr->content, "clockwise") == 0)
        {
            station.clockwise[presentPtr->fromStation - 1] = '1';
        }
        else if (strcmp(presentPtr->content, "counterclockwise") == 0)
        {
            station.counterclockwise[presentPtr->fromStation - 1] = '1';
        }
        else if (strcmp(presentPtr->content, "target") == 0)
        {
            bus.target[presentPtr->toStation - 1] = '1';
        }
        presentPtr = presentPtr->nextPtr;
        if (presentPtr == NULL)
        {
            isEnd = 1;
        }
    }
    return isEnd;
}
//是否没有请求，改进：应该可以用公交车是否有目标车站简化代替（TIME=0？）
void isEmptyRequest(){
    int i = 0;
    for (i = 0; station.clockwise[i] != 0; i++){
        if (station.clockwise[i] == '1')
        {
            i = -1;
            break;
        }
    }
    if (i != -1)    //排查逆时针
    {
        for (i = 0; station.counterclockwise[i] != 0; i++){
            if (station.counterclockwise[i] == '1')
            {
                i = -1;
                break;
            }
        }
    }
    if (i != -1)    //排查target
    {
        for (i = 0; bus.target[i] != 0; i++){
            if (bus.target[i] == '1')
            {
                i = -1;
                break;
            }
        }
    }
    if (i == -1)    //有请求
    {
        bus.isTask = 1;
    }
    else
    {
        bus.isTask = 0;
    }
}

void stationDeal(int time, int key){//到站处理, key=1是完成当前请求时的处理
    int i = 0;//i,计数clock次数
    int posStation = bus.pos / GAP;//当前站点位置，0，1，2……，MAXN-1.
    R *presentPtr;
    presentPtr = headPtr; 
    char string1[MAXN] = "clock";
    //遍历到此时为止的需求记录列表
    while (i != (time + 1) && presentPtr != NULL)//time+1
    {
        if (strcmp(presentPtr->content, string1) == 0)  //clock指令
        {
            i ++;
        }
        else    //其他指令
        {
            if (presentPtr->toStation == (posStation + 1) && presentPtr->isServed != 1)
            {   //车上请求列表中某未完成的请求
                presentPtr->isServed = 1;
                bus.target[posStation] = '0';
            }
            else if (presentPtr->fromStation == (posStation + 1) && presentPtr->isServed != 1)
            {   //站台
                presentPtr->isServed = 1;
                station.clockwise[posStation] = '0';
                station.counterclockwise[posStation] = '0';
            }
        }
        presentPtr = presentPtr->nextPtr;
    }
    if (key == 1)
    {
        bus.curTargetStationTo = -1;
    }
}
void stationTo(){
    char wholeRequest[MAXN] = {0};
    int i = 0, prePos = bus.pos / GAP;//prePos记录完成请求的终止位置，0，1，…，MAXN-1.
    int sum = 0;//gap记录到下一目的站数,记录该方向寻找路程（是否过半）
    int turnN;//转向指标
    if (TOTAL_STATION % 2 == 0)
    {
        turnN = TOTAL_STATION / 2;
    }
    else
    {
        turnN = (TOTAL_STATION - 1) / 2;
    }
    //总请求列表
    while (i < TOTAL_STATION)
    {
        if (station.clockwise[i] == '1' || station.counterclockwise[i] == '1' || bus.target[i] == '1')
        {
            wholeRequest[i] = '1';
        }
        else
        {
            wholeRequest[i] = '0';
        }
        i ++;
    }
    //按方向选定目标站点
    i = prePos;
    if (bus.isClockwise == 1)
    {
        while (wholeRequest[i] != '1')
        {
            i = (i + 1) % TOTAL_STATION;
            sum ++;
            if (sum > turnN)
            {
                bus.isClockwise = 0;
                i = prePos;
                while (wholeRequest[i] != '1')
                {
                    i = (i + TOTAL_STATION - 1) % TOTAL_STATION;
                }
                break;
            }
        }
    }
    else
    {
        while (wholeRequest[i] != '1')
        {
            i = (i + TOTAL_STATION - 1) % TOTAL_STATION;
            sum ++;
            if (sum > turnN)
            {
                bus.isClockwise = 1;
                i = prePos;
                while (wholeRequest[i] != '1')
                {
                    i = (i + 1) % TOTAL_STATION;
                }
                break;
            }
        }
    }
    bus.curTargetStationTo = i + 1;
}
//该站是否要停
void needStop(){
    int pos = bus.pos / GAP;
    if (bus.target[pos] == '1' || station.clockwise[pos] == '1' || station.counterclockwise[pos] == '1')
    {
        bus.isTempWait = 1;
    }
}

void scanMove(){
    if (bus.isClockwise == 1)
    {
        bus.pos = (bus.pos + 1) % LENGTH;
    }
    else
    {
        bus.pos = (bus.pos + LENGTH - 1) % LENGTH;
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

void freeRequire() {
    R *currentPtr, *temp;
    currentPtr = headPtr;
    while (currentPtr != NULL) {
        temp = currentPtr->nextPtr;
        free(currentPtr);
        currentPtr = temp;
    }
}

void SCAN(){
    int isEnd = 0;  //是否出现end
    TIME = 0;

    output();//TIME=0输出
    reRequest(TIME);
    while (isEnd != 1)
    {
        isEmptyRequest();
        if (bus.isTask == 1)//请求列表不空
        {
            if (bus.curTargetStationTo == -1)
            {
                stationTo();
            }
            if (bus.isTargetWait == 1)
            {
                stationDeal(TIME, 1);  //站台处理
                isEmptyRequest();
                if (bus.isTask == 1)
                {
                    stationTo();
                    bus.isTargetWait = -1;
                }
                else
                {
                    bus.isTargetWait = -1;
                }                
            }
            else if (bus.isTempWait == 1)
            {
                stationDeal(TIME, 0);
                bus.isTempWait = -1;
            }
            else    //不属于预判停靠；
            {
                if (bus.isTempWait == -1 || bus.isTargetWait == -1)
                {
                    stationDeal(TIME, 0);
                    bus.isTempWait = 0;
                    bus.isTargetWait = 0;
                }
                scanMove();
                if (bus.pos % GAP == 0 && (bus.pos / GAP + 1) == bus.curTargetStationTo && bus.isTargetWait == 0)
                {
                    bus.isTargetWait = 1;
                }
                else if (bus.pos % GAP == 0 && (bus.pos / GAP + 1) != bus.curTargetStationTo && bus.isTempWait == 0)
                {
                    needStop();
                }
            }
        }
        else
        {
            bus.isTargetWait = -1;//标志已等待1s，后续原地请求可立刻完成
        }
        TIME ++;
        output();
//        if (TIME == 5){//linshi
//            TIME = 5;
//        }
        isEnd = reRequest(TIME);
    }
    isEmptyRequest();
    //end后有未完成的请求
    while (bus.isTask == 1)//请求列表不空
    {
        if (bus.curTargetStationTo == -1)
        {
            stationTo();
        }
        if (bus.isTargetWait == 1)
        {
            stationDeal(TIME, 1);  //站台处理
            stationTo();
            bus.isTargetWait = -1;
        }
        else if (bus.isTempWait == 1)
        {
            stationDeal(TIME, 0);
            bus.isTempWait = -1;
        }
        else    //不属于预判停靠；
        {
            if (bus.isTempWait == -1 || bus.isTargetWait == -1)
            {
                stationDeal(TIME, 0);
                bus.isTempWait = 0;
                bus.isTargetWait = 0;
            }
            isEmptyRequest();
            if (bus.isTask == 0)
            {
                break;
            }
            scanMove();
            if (bus.pos % GAP == 0 && (bus.pos / GAP + 1) == bus.curTargetStationTo && bus.isTargetWait == 0)
            {
                bus.isTargetWait = 1;
            }
            else if (bus.pos % GAP == 0 && (bus.pos / GAP + 1) != bus.curTargetStationTo && bus.isTempWait == 0)
            {
                needStop();
            }
        }
        TIME ++;
        output();
        isEmptyRequest();
    }
}

int main(void){
    bus = initBus();
    station = initStation();
    headPtr = createRequire();
    R* ptr = headPtr;
    SCAN();
    puts("end");
    freeRequire();
    return 0;
}

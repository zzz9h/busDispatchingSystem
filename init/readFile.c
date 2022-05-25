#include "stdio.h"
#include "stdlib.h"

void readFile() {
    int i = 1;        // i为什么为1？因为13行我们已经处理了head[0]的情况
    char head[3], ch; // head数组分别读取三行的首字母

    FILE *fPtr;
    if ((fPtr = fopen("dict.dic", "r")) == NULL) //打开文件
        printf("File could not be opened\n");

    ch = getc(fPtr); //读取第一行首字母
    head[0] = ch;
    while (!feof(fPtr)) { //往后遍历
        ch = fgetc(fPtr);
        if (ch == '\n') {                            //如果读到换行
            head[i++] = fgetc(fPtr); //下一个字符就是下一行的首字母
        }
    }
    rewind(fPtr); //归位
    for (i = 0; i < 3; i++) { //遍历三行首字母，处理它们
        int x;
        char fir[9], sec[5], thir[5];
        ch = head[i];
        switch (ch) {
            case 'T':
                fscanf(fPtr, "%s %s %d", fir, sec, &x); // x获取站点数量
                printf("Total_Station: %d\n", x);
                //注意，真实程序中不会有这个printf操作的，printf可以替换成存储等其他操作
                break;
            case 'S':
                fscanf(fPtr, "%s %s %s", fir, sec, thir); // thir获取相关策略
                printf("Strategy: %c\n", thir[0]);
                //注意，真实程序中不会有这个printf操作的，printf可以替换成存储等其他操作
                break;
            case 'D':
                fscanf(fPtr, "%s %s %d", fir, sec, &x); // x获取站点间距离
                printf("Distance: %d\n", x);
                //注意，真实程序中不会有这个printf操作的，printf可以替换成存储等其他操作
                break;
        }
    }
}

int main(void) {
    readFile();
    return EXIT_SUCCESS;
}

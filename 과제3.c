#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>

// 과제3_로빛_21기_수습단원_박도연
// 
// 입력받은 연도가 윤년인지 판단한다.
int main(void)
{
    int year;

    printf("연도를 입력하세요: ");

    // 입력값이 정수인지 판단
    if (scanf("%d", &year) != 1)
    {
        printf("입력 오류: 정수를 입력해야 합니다.\n");
        return 1;
    }

    // 입력값이 1이상인지 판단
    if (year <= 0)
    {
        printf("입력 오류: 연도는 1 이상의 정수이어야 합니다.\n");
        return 1;
    }

    // 윤년인지 아닌지 판단
    if ((year % 4 == 0 && year % 100 != 0) || year % 400 == 0)
    {
        printf("윤년입니다.\n");
    }
    else
    {
        printf("윤년이 아닙니다.\n");
    }

    return 0;
}

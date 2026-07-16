#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>

// 과제5_로빛_21기_수습단원_박도연
// 
// 입력값 n에 따라 위아래가 대칭인 나비 모양의 별을 출력한다.
int main(void)
{
    int n;
    int i;
    int j;

    printf("값을 입력하세요: ");

    if (scanf("%d", &n) != 1)
    {
        printf("입력 오류: 정수를 입력해야 합니다.\n");
        return 1;
    }

    if (n < 1)
    {
        printf("입력 오류: 1 이상의 정수를 입력해야 합니다.\n");
        return 1;
    }

    // 윗부분과 가운데 줄
    for (i = 1; i <= n; i++)
    {
        for (j = 1; j <= i; j++)
        {
            printf("*");
        }

        for (j = 1; j <= 2 * (n - i); j++)
        {
            printf(" ");
        }

        for (j = 1; j <= i; j++)
        {
            printf("*");
        }

        printf("\n");
    }

    // 아랫부분
    for (i = n - 1; i >= 1; i--)
    {
        for (j = 1; j <= i; j++)
        {
            printf("*");
        }

        for (j = 1; j <= 2 * (n - i); j++)
        {
            printf(" ");
        }

        for (j = 1; j <= i; j++)
        {
            printf("*");
        }

        printf("\n");
    }

    return 0;
}

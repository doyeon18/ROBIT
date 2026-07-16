#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>

// 과제2_로빛_21기_수습단원_박도연

// n을 입력받아 n번째 피보나치 수를 출력한다.
int main(void)
{
    int n;
    int i;
    int first = 0;
    int second = 1;
    int next = 0;

    printf("n을 입력하세요(1~47): ");

    if (scanf("%d", &n) != 1)
    {
        printf("입력 오류: 정수를 입력해야 합니다.\n");
        return 1;
    }

    // int 자료형의 범위를 넘지 않도록 n의 범위를 제한한다.
    if (n < 1 || n > 47)
    {
        printf("입력 오류: n은 1 이상 47 이하이어야 합니다.\n");
        return 1;
    }

    if (n == 1)
    {
        next = 0;
    }
    else if (n == 2)
    {
        next = 1;
    }
    else
    {
        for (i = 3; i <= n; i++)
        {
            next = first + second;
            first = second;
            second = next;
        }
    }

    printf("%d번째 피보나치 수는 %d입니다.\n", n, next);

    return 0;
}

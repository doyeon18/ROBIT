#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>

// 과제6_로빛_21기_수습단원_박도연
// 
// 입력값 n에 따라 속이 빈 피라미드 모양의 별을 출력한다.
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

    for (i = 1; i <= n; i++)
    {
        // 왼쪽 공백
        for (j = 1; j <= n - i; j++)
        {
            printf(" ");
        }

        if (i == 1)
        {
            printf("*");
        }
        else if (i == n)
        {
            // 마지막 줄은 별을 모두 채운다.
            for (j = 1; j <= 2 * n - 1; j++)
            {
                printf("*");
            }
        }
        else
        {
            printf("*");

            for (j = 1; j <= 2 * i - 3; j++)
            {
                printf(" ");
            }

            printf("*");
        }

        printf("\n");
    }

    return 0;
}

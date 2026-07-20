#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>

int main(void)
{
    int array[5][5] = { 0 };
    int number = 1;
    int i, j;

    // 모래시계의 위쪽과 가운데 부분에 숫자를 저장
    for (i = 0; i <= 2; i++)
    {
        for (j = i; j <= 4 - i; j++)
        {
            array[i][j] = number;
            number++;
        }
    }

    // 모래시계의 아래쪽 부분에 숫자를 저장
    for (i = 3; i < 5; i++)
    {
        for (j = 4 - i; j <= i; j++)
        {
            array[i][j] = number;
            number++;
        }
    }

    // 완성된 5×5 배열 출력
    for (i = 0; i < 5; i++)
    {
        for (j = 0; j < 5; j++)
        {
            printf("%3d", array[i][j]);
        }

        printf("\n");
    }

    return 0;
}
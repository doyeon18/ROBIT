#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>

// 과제1_로빛_21기_수습단원_박도연

// 5개의 실수를 입력받아 평균, 최댓값, 최솟값을 출력한다.
int main(void)
{
    int i;
    double number;
    double sum = 0.0;
    double max = 0.0;
    double min = 0.0;
    double average;

    for (i = 0; i < 5; i++)
    {
        printf("%d번째 실수를 입력하세요: ", i + 1);

        if (scanf("%lf", &number) != 1)
        {
            printf("입력 오류: 실수를 입력해야 합니다.\n");
            return 1;
        }

        sum += number;

        // 첫 번째 입력값을 최댓값과 최솟값의 기준으로 정한다.
        if (i == 0)
        {
            max = number;
            min = number;
        }
        else
        {
            if (number > max)
            {
                max = number;
            }

            if (number < min)
            {
                min = number;
            }
        }
    }

    average = sum / 5.0;

    printf("---- 결과 ----\n");
    printf("평균은 %.6f입니다.\n", average);
    printf("최댓값은 %.6f입니다.\n", max);
    printf("최솟값은 %.6f입니다.\n", min);

    return 0;
}
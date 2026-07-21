#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>

#define ROWS 3
#define COLS 4
#define INPUT_COUNT 3

int main(void)
{
    int numbers[INPUT_COUNT];
    int pattern[ROWS * COLS];

    int* number_pointer = numbers;
    int* pattern_pointer = pattern;

    int stage;
    int row;
    int column;
    int used_count;

    // 예시에 따라 숫자를 총 세 번 입력받음
    for (stage = 0; stage < INPUT_COUNT; stage++)
    {
        // 첫 번째 입력과 이후 입력의 안내 문구를 구분함
        if (stage == 0)
        {
            printf("input : ");
        }
        else
        {
            printf("next input : ");
        }

        // 포인터를 이용해 숫자를 배열에 저장함
        if (scanf("%d", number_pointer + stage) != 1)
        {
            printf("입력 오류입니다.\n");
            return 1;
        }

        // 현재까지 입력된 숫자의 개수를 저장함
        used_count = stage + 1;

        // 입력된 숫자들을 순서대로 반복해 도형을 만듦
        for (row = 0; row < ROWS; row++)
        {
            for (column = 0; column < COLS; column++)
            {
                *(pattern_pointer + row * COLS + column)
                    = *(number_pointer
                        + (row + column) % used_count);
            }
        }

        // 포인터를 이용해 완성된 도형을 출력함
        for (row = 0; row < ROWS; row++)
        {
            for (column = 0; column < COLS; column++)
            {
                printf("%d",
                    *(pattern_pointer
                        + row * COLS + column));
            }

            printf("\n");
        }

        // 다음 입력 결과와 구분하기 위해 빈 줄을 출력함
        if (stage < INPUT_COUNT - 1)
        {
            printf("\n");
        }
    }

    return 0;
}
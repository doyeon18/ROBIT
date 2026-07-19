#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>

#define MAX_NUMBER 10000

// 해당 숫자가 제곱 ㄴㄴ수인지 판별하는 함수
int isSquareFree(int number);

int main(void)
{
    int min;
    int max;
    int number;
    int count = 0;
    int results[MAX_NUMBER];
    int i;

    printf("min : ");

    // min이 정수로 정상 입력되었는지 확인
    if (scanf("%d", &min) != 1)
    {
        printf("입력 오류: 정수를 입력해야 합니다.\n");
        return 1;
    }

    printf("max : ");

    // max가 정수로 정상 입력되었는지 확인
    if (scanf("%d", &max) != 1)
    {
        printf("입력 오류: 정수를 입력해야 합니다.\n");
        return 1;
    }

    // 입력 범위가 1부터 10000 사이인지 확인
    if (min < 1 || min > MAX_NUMBER ||
        max < 1 || max > MAX_NUMBER)
    {
        printf("입력 오류: 1부터 10000 사이의 정수를 입력해야 합니다.\n");
        return 1;
    }

    // 최솟값이 최댓값보다 큰 경우 처리
    if (min > max)
    {
        printf("입력 오류: min은 max보다 작거나 같아야 합니다.\n");
        return 1;
    }

    // min부터 max까지 각 숫자를 검사한다.
    for (number = min; number <= max; number++)
    {
        if (isSquareFree(number) == 1)
        {
            // 제곱 ㄴㄴ수를 배열에 저장하고 개수를 증가시킨다.
            results[count] = number;
            count++;
        }
    }

    printf("제곱 ㄴㄴ수 : %d개\n", count);

    // 배열에 저장된 제곱 ㄴㄴ수를 출력한다.
    for (i = 0; i < count; i++)
    {
        printf("%d ", results[i]);
    }

    printf("\n");

    return 0;
}

// number가 1보다 큰 제곱수로 나누어지는지 검사하는 함수
int isSquareFree(int number)
{
    int base;
    int square;

    /*
        2의 제곱부터 검사한다.
        제곱값이 number보다 커지면 더 이상 검사할 필요가 없다.
    */
    for (base = 2; base * base <= number; base++)
    {
        square = base * base;

        // 제곱수로 나누어떨어지면 제곱 ㄴㄴ수가 아니다.
        if (number % square == 0)
        {
            return 0;
        }
    }

    // 어떤 제곱수로도 나누어떨어지지 않은 경우
    return 1;
}
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>

#define MAX_SIZE 100

int main(void)
{
    int arrayA[MAX_SIZE][MAX_SIZE] = { 0 };
    int arrayB[MAX_SIZE][MAX_SIZE] = { 0 };

    int n1, n2;
    int number = 1;
    int i, j;

    printf("배열의 행과 열을 입력하세요: ");

    // 정수 두 개가 정상적으로 입력되었는지 확인
    if (scanf("%d %d", &n1, &n2) != 2)
    {
        printf("입력 오류: 정수 두 개를 입력해야 합니다.\n");
        return 1;
    }

    // 배열 크기가 올바른 범위인지 확인
    if (n1 < 1 || n1 > MAX_SIZE ||
        n2 < 1 || n2 > MAX_SIZE)
    {
        printf("입력 오류: 행과 열은 1부터 %d 사이여야 합니다.\n",
            MAX_SIZE);
        return 1;
    }

    // N1×N2 크기의 배열 A에 1부터 차례대로 저장
    for (i = 0; i < n1; i++)
    {
        for (j = 0; j < n2; j++)
        {
            arrayA[i][j] = number;
            number++;
        }
    }

    // 배열 A의 행과 열을 바꾸어 배열 B에 저장
    for (i = 0; i < n1; i++)
    {
        for (j = 0; j < n2; j++)
        {
            arrayB[j][i] = arrayA[i][j];
        }
    }

    // 배열 A 출력
    printf("\n배열 A\n");

    for (i = 0; i < n1; i++)
    {
        for (j = 0; j < n2; j++)
        {
            printf("%4d", arrayA[i][j]);
        }

        printf("\n");
    }

    // 배열 B는 N2×N1 크기로 출력
    printf("\n배열 B\n");

    for (i = 0; i < n2; i++)
    {
        for (j = 0; j < n1; j++)
        {
            printf("%4d", arrayB[i][j]);
        }

        printf("\n");
    }

    return 0;
}
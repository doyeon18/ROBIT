#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    int** array;
    int n;
    int i, j;
    int diagonal;
    int startRow;
    int endRow;
    int number = 1;

    printf("배열의 크기 N을 입력하세요: ");

    // 정수가 정상적으로 입력되었는지 확인
    if (scanf("%d", &n) != 1)
    {
        printf("입력 오류: 정수를 입력해야 합니다.\n");
        return 1;
    }

    // 배열의 크기는 1 이상이어야 함
    if (n <= 0)
    {
        printf("입력 오류: N은 1 이상이어야 합니다.\n");
        return 1;
    }

    // 각 행의 주소를 저장할 포인터 배열 동적 할당
    array = (int**)malloc(n * sizeof(int*));

    if (array == NULL)
    {
        printf("메모리 할당에 실패했습니다.\n");
        return 1;
    }

    // 각 행에 N개의 정수를 저장할 공간 동적 할당
    for (i = 0; i < n; i++)
    {
        array[i] = (int*)malloc(n * sizeof(int));

        if (array[i] == NULL)
        {
            printf("메모리 할당에 실패했습니다.\n");

            // 이전까지 할당된 메모리 해제
            for (j = 0; j < i; j++)
            {
                free(array[j]);
            }

            free(array);
            return 1;
        }
    }

    /*
        대각선 번호는 행 번호와 열 번호의 합으로 구분한다.

        diagonal = 0 : [0][0]
        diagonal = 1 : [0][1], [1][0]
        diagonal = 2 : [0][2], [1][1], [2][0]
    */
    for (diagonal = 0; diagonal <= 2 * n - 2; diagonal++)
    {
        // 현재 대각선에서 시작할 행 번호 결정
        if (diagonal < n)
        {
            startRow = 0;
        }
        else
        {
            startRow = diagonal - n + 1;
        }

        // 현재 대각선에서 마지막 행 번호 결정
        if (diagonal < n)
        {
            endRow = diagonal;
        }
        else
        {
            endRow = n - 1;
        }

        // 대각선을 위쪽에서 아래쪽 방향으로 채움
        for (i = startRow; i <= endRow; i++)
        {
            j = diagonal - i;

            array[i][j] = number;
            number++;
        }
    }

    // 완성된 배열 출력
    printf("\n");

    for (i = 0; i < n; i++)
    {
        for (j = 0; j < n; j++)
        {
            printf("%4d", array[i][j]);
        }

        printf("\n");
    }

    // 각 행에 할당한 메모리 해제
    for (i = 0; i < n; i++)
    {
        free(array[i]);
    }

    // 행 포인터 배열 해제
    free(array);

    return 0;
}
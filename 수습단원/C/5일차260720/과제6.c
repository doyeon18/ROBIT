#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    int** array;
    int n1, n2;
    int centerRow, centerCol;
    int maxRadius;
    int radius;
    int number = 1;
    int i, j, k;

    printf("N1과 N2를 입력하세요: ");

    // 정수 두 개가 정상적으로 입력되었는지 확인
    if (scanf("%d %d", &n1, &n2) != 2)
    {
        printf("입력 오류: 정수 두 개를 입력해야 합니다.\n");
        return 1;
    }

    // 배열 크기는 양수이면서 홀수여야 함
    if (n1 <= 0 || n2 <= 0 || n1 % 2 == 0 || n2 % 2 == 0)
    {
        printf("입력 오류: N1과 N2는 양의 홀수여야 합니다.\n");
        return 1;
    }

    // 행의 주소를 저장할 포인터 배열 동적 할당
    array = (int**)malloc(n1 * sizeof(int*));

    if (array == NULL)
    {
        printf("메모리 할당에 실패했습니다.\n");
        return 1;
    }

    // 각 행에 N2개의 정수를 저장할 공간 동적 할당
    for (i = 0; i < n1; i++)
    {
        array[i] = (int*)malloc(n2 * sizeof(int));

        if (array[i] == NULL)
        {
            printf("메모리 할당에 실패했습니다.\n");

            // 이전에 할당된 행의 메모리 해제
            for (j = 0; j < i; j++)
            {
                free(array[j]);
            }

            free(array);
            return 1;
        }
    }

    // 배열의 중앙 위치 계산
    centerRow = n1 / 2;
    centerCol = n2 / 2;

    // 중앙에 1 저장
    array[centerRow][centerCol] = number;
    number++;

    /*
        중앙에서 배열의 가장 먼 모서리까지의
        맨해튼 거리를 최대 반지름으로 사용
    */
    maxRadius = centerRow + centerCol;

    if (centerRow + (n2 - 1 - centerCol) > maxRadius)
    {
        maxRadius = centerRow + (n2 - 1 - centerCol);
    }

    if ((n1 - 1 - centerRow) + centerCol > maxRadius)
    {
        maxRadius = (n1 - 1 - centerRow) + centerCol;
    }

    if ((n1 - 1 - centerRow) +
        (n2 - 1 - centerCol) > maxRadius)
    {
        maxRadius = (n1 - 1 - centerRow) +
            (n2 - 1 - centerCol);
    }

    // 중앙에서 바깥쪽으로 마름모를 확장하며 숫자 저장
    for (radius = 1; radius <= maxRadius; radius++)
    {
        /*
            왼쪽 지점에서 아래쪽 지점으로 이동
            이동 방향: 오른쪽 아래
        */
        for (k = 1; k <= radius; k++)
        {
            i = centerRow + k;
            j = centerCol - radius + k;

            if (i >= 0 && i < n1 && j >= 0 && j < n2)
            {
                array[i][j] = number;
                number++;
            }
        }

        /*
            아래쪽 지점에서 오른쪽 지점으로 이동
            이동 방향: 오른쪽 위
        */
        for (k = 1; k <= radius; k++)
        {
            i = centerRow + radius - k;
            j = centerCol + k;

            if (i >= 0 && i < n1 && j >= 0 && j < n2)
            {
                array[i][j] = number;
                number++;
            }
        }

        /*
            오른쪽 지점에서 위쪽 지점으로 이동
            이동 방향: 왼쪽 위
        */
        for (k = 1; k <= radius; k++)
        {
            i = centerRow - k;
            j = centerCol + radius - k;

            if (i >= 0 && i < n1 && j >= 0 && j < n2)
            {
                array[i][j] = number;
                number++;
            }
        }

        /*
            위쪽 지점에서 왼쪽 지점으로 이동
            이동 방향: 왼쪽 아래
        */
        for (k = 1; k <= radius; k++)
        {
            i = centerRow - radius + k;
            j = centerCol - k;

            if (i >= 0 && i < n1 && j >= 0 && j < n2)
            {
                array[i][j] = number;
                number++;
            }
        }
    }

    // 완성된 배열 출력
    printf("\n");

    for (i = 0; i < n1; i++)
    {
        for (j = 0; j < n2; j++)
        {
            printf("%4d", array[i][j]);
        }

        printf("\n");
    }

    // 각 행에 할당한 메모리 해제
    for (i = 0; i < n1; i++)
    {
        free(array[i]);
    }

    // 행 포인터 배열 해제
    free(array);

    return 0;
}
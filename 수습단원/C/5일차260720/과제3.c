#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    int n1, n2;
    int** array;

    int i, j;
    int number = 1;

    int top;
    int bottom;
    int left;
    int right;

    printf("배열의 행과 열을 입력하세요: ");

    // 정수 두 개가 정상적으로 입력되었는지 확인
    if (scanf("%d %d", &n1, &n2) != 2)
    {
        printf("입력 오류: 정수 두 개를 입력해야 합니다.\n");
        return 1;
    }

    // 배열의 행과 열은 반드시 1 이상이어야 함
    if (n1 <= 0 || n2 <= 0)
    {
        printf("입력 오류: 행과 열은 1 이상이어야 합니다.\n");
        return 1;
    }

    // 각 행의 주소를 저장할 포인터 배열을 동적 할당
    array = (int**)malloc(n1 * sizeof(int*));

    if (array == NULL)
    {
        printf("메모리 할당에 실패했습니다.\n");
        return 1;
    }

    // 각 행마다 N2개의 정수를 저장할 공간을 동적 할당
    for (i = 0; i < n1; i++)
    {
        array[i] = (int*)malloc(n2 * sizeof(int));

        // 특정 행의 메모리 할당에 실패한 경우
        if (array[i] == NULL)
        {
            printf("메모리 할당에 실패했습니다.\n");

            // 이전에 성공적으로 할당한 행들을 해제
            for (j = 0; j < i; j++)
            {
                free(array[j]);
            }

            free(array);

            return 1;
        }
    }

    // 아직 숫자를 넣지 않은 배열 영역의 경계 설정
    top = 0;
    bottom = n1 - 1;
    left = 0;
    right = n2 - 1;

    // 배열의 모든 칸이 채워질 때까지 반복
    while (top <= bottom && left <= right)
    {
        // 위쪽 행을 왼쪽에서 오른쪽으로 채움
        for (j = left; j <= right; j++)
        {
            array[top][j] = number;
            number++;
        }

        top++;

        // 오른쪽 열을 위에서 아래쪽으로 채움
        for (i = top; i <= bottom; i++)
        {
            array[i][right] = number;
            number++;
        }

        right--;

        // 아직 아래쪽 행이 남아 있는 경우
        if (top <= bottom)
        {
            // 아래쪽 행을 오른쪽에서 왼쪽으로 채움
            for (j = right; j >= left; j--)
            {
                array[bottom][j] = number;
                number++;
            }

            bottom--;
        }

        // 아직 왼쪽 열이 남아 있는 경우
        if (left <= right)
        {
            // 왼쪽 열을 아래에서 위쪽으로 채움
            for (i = bottom; i >= top; i--)
            {
                array[i][left] = number;
                number++;
            }

            left++;
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

    // 각 행에 동적 할당한 메모리를 먼저 해제
    for (i = 0; i < n1; i++)
    {
        free(array[i]);
    }

    // 행의 주소를 저장한 포인터 배열 해제
    free(array);

    return 0;
}
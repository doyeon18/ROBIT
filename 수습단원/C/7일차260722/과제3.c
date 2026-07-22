#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>

void print(int* row, int* col, int** pArr);
void arr_ij(int* sizeRow, int* sizeCol, int** pArr);

int main(void)
{
    int** arr = NULL;
    int row, col;
    int sizeRow, sizeCol;
    int i;

    printf("열의 수를 입력하세요: ");

    if (scanf("%d", &sizeCol) != 1 || sizeCol <= 0)
    {
        printf("1 이상의 정수를 입력해야 합니다.\n");
        return 1;
    }

    printf("행의 수를 입력하세요: ");

    if (scanf("%d", &sizeRow) != 1 || sizeRow <= 0)
    {
        printf("1 이상의 정수를 입력해야 합니다.\n");
        return 1;
    }

    row = sizeRow;
    col = sizeCol;

    // 행의 개수만큼 포인터 배열을 할당
    arr = (int**)malloc(sizeof(int*) * row);

    if (arr == NULL)
    {
        printf("메모리 할당에 실패했습니다.\n");
        return 1;
    }

    // 각 행에 열의 개수만큼 정수 공간을 할당
    for (i = 0; i < row; i++)
    {
        arr[i] = (int*)malloc(sizeof(int) * col);

        if (arr[i] == NULL)
        {
            int j;

            printf("메모리 할당에 실패했습니다.\n");

            // 앞에서 할당한 메모리를 해제
            for (j = 0; j < i; j++)
            {
                free(arr[j]);
            }

            free(arr);

            return 1;
        }
    }

    // 배열을 달팽이 모양으로 채움
    arr_ij(&sizeRow, &sizeCol, arr);

    // 완성된 배열을 출력
    print(&row, &col, arr);

    // 각 행의 메모리를 해제
    for (i = 0; i < row; i++)
    {
        free(arr[i]);
    }

    // 포인터 배열의 메모리를 해제
    free(arr);

    return 0;
}

void arr_ij(int* sizeRow, int* sizeCol, int** pArr)
{
    int top = 0;
    int bottom = *sizeRow - 1;
    int left = 0;
    int right = *sizeCol - 1;
    int number = 1;
    int i, j;

    while (top <= bottom && left <= right)
    {
        // 위쪽 행을 왼쪽에서 오른쪽으로 채움
        for (j = left; j <= right; j++)
        {
            pArr[top][j] = number;
            number++;
        }

        top++;

        // 오른쪽 열을 위에서 아래쪽으로 채움
        for (i = top; i <= bottom; i++)
        {
            pArr[i][right] = number;
            number++;
        }

        right--;

        // 아래쪽 행을 오른쪽에서 왼쪽으로 채움
        if (top <= bottom)
        {
            for (j = right; j >= left; j--)
            {
                pArr[bottom][j] = number;
                number++;
            }

            bottom--;
        }

        // 왼쪽 열을 아래에서 위쪽으로 채움
        if (left <= right)
        {
            for (i = bottom; i >= top; i--)
            {
                pArr[i][left] = number;
                number++;
            }

            left++;
        }
    }
}

void print(int* row, int* col, int** pArr)
{
    int i, j;

    for (i = 0; i < *row; i++)
    {
        for (j = 0; j < *col; j++)
        {
            printf("%4d", pArr[i][j]);
        }

        printf("\n");
    }
}
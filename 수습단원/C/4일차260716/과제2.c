#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>

void printArray(int numbers[], int size);
void selectionSort(int numbers[], int size);

int main(void)
{
    int numbers[8];
    int i;

    printf("정수 8개를 입력하시오: ");

    // 정수 8개를 배열에 입력받는다.
    // ex) 8 3 5 1 9 2 7 4
    for (i = 0; i < 8; i++)
    {
        // 정수가 아닌 값이 입력되면 프로그램 종료
        if (scanf("%d", &numbers[i]) != 1)
        {
            printf("입력 오류: 정수 8개를 입력해야 합니다.\n");
            return 1;
        }
    }

    printf("\n초기 배열: ");
    printArray(numbers, 8);

    // 선택 정렬을 수행
    selectionSort(numbers, 8);

    printf("\n최종 배열: ");
    printArray(numbers, 8);

    return 0;
}

// 배열의 모든 원소를 한 줄로 출력하는 함수
void printArray(int numbers[], int size)
{
    int i;

    for (i = 0; i < size; i++)
    {
        printf("%d ", numbers[i]);
    }

    printf("\n");
}

// 최댓값을 배열의 뒤쪽으로 이동시키는 선택 정렬 함수
void selectionSort(int numbers[], int size)
{
    int end;
    int j;
    int maxIndex;
    int temp;
    int step = 1;

    // end는 현재 정렬되지 않은 범위의 마지막 인덱스이므로 정렬이 한 번 끝날 때마다 범위를 하나씩 줄인다
    for (end = size - 1; end > 0; end--)
    {
        // 첫 번째 원소를 최댓값으로 가정
        maxIndex = 0;

        // 정렬되지 않은 범위에서 최댓값을 찾는다.
        for (j = 1; j <= end; j++)
        {
            if (numbers[j] > numbers[maxIndex])
            {
                maxIndex = j;
            }
        }

        // 최댓값과 현재 범위의 마지막 원소를 교환한다
        if (maxIndex != end)
        {
            temp = numbers[maxIndex];
            numbers[maxIndex] = numbers[end];
            numbers[end] = temp;
        }

        // 정렬이 진행되는 중간 과정을 출력한다.
        printf("%d단계: ", step);
        printArray(numbers, size);

        step++;
    }
}
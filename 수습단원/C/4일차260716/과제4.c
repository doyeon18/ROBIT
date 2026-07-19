#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>

#define MAX_SIZE 1000

int validateSequence(int numbers[], int size);
int findIndex(int numbers[], int size, int target);
int countMedianSubsequences(int numbers[], int size, int bIndex, int target);

int main(void)
{
    int numbers[MAX_SIZE];
    int size;
    int target;
    int bIndex;
    int result;
    int i;

    printf("N: ");

    // N이 정상적으로 입력되었는지 확인
    if (scanf("%d", &size) != 1)
    {
        printf("입력 오류: N은 정수여야 합니다.\n");
        return 1;
    }

    // 배열의 크기 범위 확인
    if (size < 1 || size > MAX_SIZE)
    {
        printf("입력 오류: N은 1부터 %d까지 가능합니다.\n", MAX_SIZE);
        return 1;
    }

    printf("B: ");

    // B가 정상적으로 입력되었는지 확인
    if (scanf("%d", &target) != 1)
    {
        printf("입력 오류: B는 정수여야 합니다.\n");
        return 1;
    }

    // B는 1부터 N 사이의 자연수여야 한다.
    if (target < 1 || target > size)
    {
        printf("입력 오류: B는 1부터 N 사이여야 합니다.\n");
        return 1;
    }

    printf("수열 A의 원소 %d개를 입력하시오: ", size);

    // 수열의 원소 N개 입력
    for (i = 0; i < size; i++)
    {
        if (scanf("%d", &numbers[i]) != 1)
        {
            printf("입력 오류: 수열에는 정수만 입력해야 합니다.\n");
            return 1;
        }
    }

    // 수열이 1부터 N까지의 수로 올바르게 구성되었는지 확인
    if (validateSequence(numbers, size) == 0)
    {
        printf("입력 오류: 1부터 N까지의 수가 한 번씩 등장해야 합니다.\n");
        return 1;
    }

    // 수열에서 B의 위치를 찾는다.
    bIndex = findIndex(numbers, size, target);

    if (bIndex == -1)
    {
        printf("입력 오류: 수열에 B가 존재하지 않습니다.\n");
        return 1;
    }

    // 중앙값이 B인 부분 수열의 개수를 구한다.
    result = countMedianSubsequences(
        numbers,
        size,
        bIndex,
        target
    );

    printf("중앙값이 %d인 부분 수열의 개수: %d\n", target, result);

    return 0;
}

// 수열에 1부터 N까지의 숫자가 한 번씩 등장하는지 확인하는 함수
int validateSequence(int numbers[], int size)
{
    int used[MAX_SIZE + 1] = { 0 };
    int i;

    for (i = 0; i < size; i++)
    {
        // 원소가 1부터 N 사이인지 확인
        if (numbers[i] < 1 || numbers[i] > size)
        {
            return 0;
        }

        // 같은 숫자가 이미 입력되었는지 확인
        if (used[numbers[i]] == 1)
        {
            return 0;
        }

        used[numbers[i]] = 1;
    }

    return 1;
}

// 수열에서 target의 인덱스를 찾아 반환하는 함수
int findIndex(int numbers[], int size, int target)
{
    int i;

    for (i = 0; i < size; i++)
    {
        if (numbers[i] == target)
        {
            return i;
        }
    }

    // target을 찾지 못한 경우
    return -1;
}

// 중앙값이 target인 연속 부분 수열의 개수를 구하는 함수
int countMedianSubsequences(
    int numbers[],
    int size,
    int bIndex,
    int target
)
{
    int left;
    int right;
    int smallerLeft = 0;
    int greaterLeft = 0;
    int smaller;
    int greater;
    int count = 0;

    /*
        시작 위치를 B의 위치부터 왼쪽으로 이동한다.
        모든 구간이 B를 포함하도록 한다.
    */
    for (left = bIndex; left >= 0; left--)
    {
        // 왼쪽으로 새롭게 포함된 숫자를 확인
        if (left < bIndex)
        {
            if (numbers[left] < target)
            {
                smallerLeft++;
            }
            else
            {
                greaterLeft++;
            }
        }

        smaller = smallerLeft;
        greater = greaterLeft;

        /*
            끝 위치를 B의 위치부터 오른쪽으로 이동한다.
            left부터 right까지가 현재 부분 수열이다.
        */
        for (right = bIndex; right < size; right++)
        {
            // 오른쪽으로 새롭게 포함된 숫자를 확인
            if (right > bIndex)
            {
                if (numbers[right] < target)
                {
                    smaller++;
                }
                else
                {
                    greater++;
                }
            }

            
            // B보다 작은 수와 큰 수의 개수가 같으면 해당 부분 수열의 중앙값은 B이다.
            if (smaller == greater)
            {
                count++;
            }
        }
    }

    return count;
}
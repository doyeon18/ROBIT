#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>

#define MAX_LENGTH 99

// 문자열의 길이를 구하는 함수
int getLength(char str[]);

// inputStr에서 subStr의 모든 위치를 찾아 출력하는 함수
int findSubString(char inputStr[], char subStr[]);

int main(void)
{
    char inputStr[MAX_LENGTH + 1];
    char subStr[MAX_LENGTH + 1];
    int count;

    printf("최대 99개 문자 입력 <inputStr> : ");

    // 입력 문자열이 정상적으로 입력되었는지 확인
    if (scanf("%99s", inputStr) != 1)
    {
        printf("입력 오류: 문자열을 입력해야 합니다.\n");
        return 1;
    }

    printf("찾는 문자열 <subStr> : ");

    // 찾는 문자열이 정상적으로 입력되었는지 확인
    if (scanf("%99s", subStr) != 1)
    {
        printf("입력 오류: 찾는 문자열을 입력해야 합니다.\n");
        return 1;
    }

    // 입력 문자열에서 찾는 문자열의 모든 위치를 출력한다.
    count = findSubString(inputStr, subStr);

    // 문자열을 찾지 못한 경우
    if (count == 0)
    {
        printf("찾는 문자열이 없습니다.\n");
    }

    return 0;
}

// 널 문자가 나올 때까지 세어 문자열의 길이를 반환하는 함수
int getLength(char str[])
{
    int length = 0;

    while (str[length] != '\0')
    {
        length++;
    }

    return length;
}

// inputStr 안에서 subStr이 시작되는 모든 위치를 찾는 함수
int findSubString(char inputStr[], char subStr[])
{
    int inputLength;
    int subLength;
    int i;
    int j;
    int isMatch;
    int count = 0;

    inputLength = getLength(inputStr);
    subLength = getLength(subStr);

    // 찾는 문자열이 입력 문자열보다 길면 찾을 수 없다.
    if (subLength > inputLength)
    {
        return 0;
    }

    /*
        subStr이 들어갈 수 있는 모든 시작 위치를 확인한다.
        배열 인덱스는 0부터 시작한다.
    */
    for (i = 0; i <= inputLength - subLength; i++)
    {
        // 처음에는 두 문자열이 같다고 가정한다.
        isMatch = 1;

        // 현재 시작 위치부터 subStr의 모든 문자를 비교한다.
        for (j = 0; j < subLength; j++)
        {
            if (inputStr[i + j] != subStr[j])
            {
                isMatch = 0;
                break;
            }
        }

        // 모든 문자가 같으면 해당 시작 위치를 출력한다.
        if (isMatch == 1)
        {
            if (count == 0)
            {
                printf("%s의 위치 : ", subStr);
            }

            // 문제 예시에 맞게 위치는 1부터 출력한다.
            printf("%d ", i + 1);
            count++;
        }
    }

    if (count > 0)
    {
        printf("\n");
    }

    return count;
}
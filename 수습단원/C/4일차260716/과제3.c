#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>

// 입력된 음계의 연주 순서를 판별하여 결과를 반환하는 함수
int checkScale(char notes[]);

int main(void)
{
    char notes[20];
    int result;

    printf("음 입력: ");

    // 문자열이 정상적으로 입력되었는지 확인
    if (scanf("%19s", notes) != 1)
    {
        printf("입력 오류: 음계를 입력해야 합니다.\n");
        return 1;
    }

    // 함수의 반환값을 result에 저장
    result = checkScale(notes);

    // 반환값에 따라 결과를 출력
    switch (result)
    {
    case 0:
        printf("입력 오류: 올바른 음 8개를 입력해야 합니다.\n");
        return 1;

    case 1:
        printf("결과 : ascending\n");
        break;

    case 2:
        printf("결과 : descending\n");
        break;

    case 3:
        printf("결과 : mixed\n");
        break;
    }

    return 0;
}

// 입력된 음계를 ascending, descending, mixed로 판별하는 함수
int checkScale(char notes[])
{
    char ascending[9] = "cdefgabC";
    char descending[9] = "Cbagfedc";

    int length = 0;
    int isAscending = 1;
    int isDescending = 1;
    int isValid;
    int i;
    int j;

    // 입력된 문자열의 길이를 구한다.
    while (notes[length] != '\0')
    {
        length++;
    }

    // 음은 정확히 8개가 입력되어야 한다.
    if (length != 8)
    {
        return 0;
    }

    // 입력된 문자가 다장조의 음인지 확인한다.
    for (i = 0; i < 8; i++)
    {
        isValid = 0;

        for (j = 0; j < 8; j++)
        {
            if (notes[i] == ascending[j])
            {
                isValid = 1;
                break;
            }
        }

        // 올바르지 않은 문자가 발견되면 입력 오류를 반환
        if (isValid == 0)
        {
            return 0;
        }
    }

    // 입력된 음계와 ascending, descending 순서를 비교한다.
    for (i = 0; i < 8; i++)
    {
        if (notes[i] != ascending[i])
        {
            isAscending = 0;
        }

        if (notes[i] != descending[i])
        {
            isDescending = 0;
        }
    }

    if (isAscending == 1)
    {
        return 1;
    }
    else if (isDescending == 1)
    {
        return 2;
    }
    else
    {
        return 3;
    }
}
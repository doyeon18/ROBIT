#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>

void printChars(char ch, int count);
void printPattern(int size, int type);

int main(void)
{
    int size;
    int type;

    printf("사이즈와 종류를 입력하시오: ");

   
    if (scanf("%d %d", &size, &type) != 2)
    {
        printf("입력 오류: 정수 2개를 입력해야 합니다.\n");
        return 1;
    }

   
    if (size <= 0 || size % 2 == 0)
    {
        printf("입력 오류: 사이즈는 1 이상의 홀수여야 합니다.\n");
        return 1;
    }

    if (type < 1 || type > 4)
    {
        printf("입력 오류: 종류는 1부터 4까지만 가능합니다.\n");
        return 1;
    }

    printPattern(size, type);

    return 0;
}


 // 전달받은 문자를 count개만큼 반복하여 출력하는 함수
void printChars(char ch, int count)
{
    int i;

    for (i = 0; i < count; i++)
    {
        printf("%c", ch);
    }
}


// 각 행의 위치에 따라 level을 계산한 뒤, 종류별로 공백과 별의 개수를 결정한다.
void printPattern(int size, int type)
{
    int row;
    int level;
    int spaces;
    int stars;

    for (row = 0; row < size; row++)
    {
        
        // 위쪽에서는 level이 증가하고, 아래쪽에서는 level이 감소한다
        if (row < size / 2)
        {
            level = row;
        }
        else
        {
            level = size - 1 - row;
        }

        // 각 행을 계산하기 전 초기화
        spaces = 0;
        stars = 0;

        switch (type)
        {
        case 1:
            /*
                종류 1
                별 개수: 1, 2, 3, 2, 1
            */
            stars = level + 1;
            break;

        case 2:
            /*
                종류 2
                종류 1을 오른쪽으로 정렬한 모양
            */
            stars = level + 1;
            spaces = (size + 1) / 2 - stars;
            break;

        case 3:
            /*
                종류 3
                별이 한 줄마다 2개씩 감소하거나 증가
            */
            spaces = level;
            stars = size - 2 * level;
            break;

        case 4:
            /*
                종류 4
                별이 한 줄마다 1개씩 감소하거나 증가
            */
            spaces = level;
            stars = (size + 1) / 2 - level;
            break;
        }

        printChars(' ', spaces);
        printChars('*', stars);
        printf("\n");
    }
}
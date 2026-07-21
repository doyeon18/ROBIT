#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>

#define MAX_LENGTH 100
#define CHARACTER_COUNT 256

int main(void)
{
    char sentence[MAX_LENGTH];
    char* pointer;
    char* end;

    int count[CHARACTER_COUNT] = { 0 };
    int max_count = 0;
    char max_character = '\0';

    // 공백을 포함한 문장을 한 줄 입력받음
    printf("입력 : ");

    if (scanf("%99[^\n]", sentence) != 1)
    {
        printf("입력 오류입니다.\n");
        return 1;
    }

    // 포인터를 이동하며 문장 끝의 널 문자 위치를 찾음
    end = sentence;

    while (*end != '\0')
    {
        end++;
    }

    // 문장 끝에서 처음 방향으로 포인터를 이동하며 출력
    printf("출력 : ");

    pointer = end;

    while (pointer != sentence)
    {
        pointer--;
        printf("%c", *pointer);
    }

    printf("\n");

    // 문장의 처음부터 끝까지 이동하며 문자 등장 횟수를 저장
    pointer = sentence;

    while (*pointer != '\0')
    {
        // 과제 예시에 따라 공백은 계산에서 제외
        if (*pointer != ' ')
        {
            count[(unsigned char)*pointer]++;
        }

        pointer++;
    }

    // 각 문자의 등장 횟수를 확인해 최다 등장 문자를 찾음
    pointer = sentence;

    while (*pointer != '\0')
    {
        if (*pointer != ' '
            && count[(unsigned char)*pointer] > max_count)
        {
            max_count = count[(unsigned char)*pointer];
            max_character = *pointer;
        }

        pointer++;
    }

    // 공백 이외의 문자가 없는 경우를 처리
    if (max_count == 0)
    {
        printf("최다등장문자 : 없음\n");
    }
    else
    {
        printf("최다등장문자 : %c\n", max_character);
    }

    return 0;
}
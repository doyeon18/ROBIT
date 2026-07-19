#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>

#define MAX_NUMBER 20

// 두 문자열이 같은지 비교하는 함수
int isSameString(char first[], char second[]);

// 집합에 숫자를 추가하는 함수
void addNumber(int set[], int number);

// 집합에서 숫자를 제거하는 함수
void removeNumber(int set[], int number);

// 집합에 숫자가 있는지 확인하는 함수
int checkNumber(int set[], int number);

// 숫자의 존재 상태를 반대로 바꾸는 함수
void toggleNumber(int set[], int number);

// 집합을 1부터 20까지의 모든 숫자로 채우는 함수
void setAll(int set[]);

// 집합을 공집합으로 만드는 함수
void setEmpty(int set[]);

// 현재 집합의 내용을 출력하는 함수
void printSet(int set[]);

int main(void)
{
    int set[MAX_NUMBER + 1] = { 0 };
    char operation[10];
    int number;

    printf("연산을 입력하시오.\n");
    printf("add x\n");
    printf("remove x\n");
    printf("check x\n");
    printf("toggle x\n");
    printf("all 0\n");
    printf("empty 0\n");

    /*
        문제에 종료 명령이 따로 없으므로 입력을 계속 받는다.
        입력이 더 이상 들어오지 않으면 반복문을 종료한다.
    */
    while (1)
    {
        printf("\ninput : ");

        // 명령어와 숫자가 정상적으로 입력되었는지 확인
        if (scanf("%9s %d", operation, &number) != 2)
        {
            printf("\n입력을 종료합니다.\n");
            break;
        }

        if (isSameString(operation, "add") == 1)
        {
            // add의 숫자는 1부터 20까지만 허용
            if (number < 1 || number > MAX_NUMBER)
            {
                printf("입력 오류: 숫자는 1부터 20까지 가능합니다.\n");
            }
            else
            {
                addNumber(set, number);
                printSet(set);
            }
        }
        else if (isSameString(operation, "remove") == 1)
        {
            // remove의 숫자는 1부터 20까지만 허용
            if (number < 1 || number > MAX_NUMBER)
            {
                printf("입력 오류: 숫자는 1부터 20까지 가능합니다.\n");
            }
            else
            {
                removeNumber(set, number);
                printSet(set);
            }
        }
        else if (isSameString(operation, "check") == 1)
        {
            // check의 숫자는 1부터 20까지만 허용
            if (number < 1 || number > MAX_NUMBER)
            {
                printf("입력 오류: 숫자는 1부터 20까지 가능합니다.\n");
            }
            else
            {
                printf("%d\n", checkNumber(set, number));
                printSet(set);
            }
        }
        else if (isSameString(operation, "toggle") == 1)
        {
            // toggle의 숫자는 1부터 20까지만 허용
            if (number < 1 || number > MAX_NUMBER)
            {
                printf("입력 오류: 숫자는 1부터 20까지 가능합니다.\n");
            }
            else
            {
                toggleNumber(set, number);
                printSet(set);
            }
        }
        else if (isSameString(operation, "all") == 1)
        {
            // all 명령 뒤에는 0을 입력해야 한다.
            if (number != 0)
            {
                printf("입력 오류: all은 'all 0'으로 입력해야 합니다.\n");
            }
            else
            {
                setAll(set);
                printSet(set);
            }
        }
        else if (isSameString(operation, "empty") == 1)
        {
            // empty 명령 뒤에는 0을 입력해야 한다.
            if (number != 0)
            {
                printf("입력 오류: empty는 'empty 0'으로 입력해야 합니다.\n");
            }
            else
            {
                setEmpty(set);
                printSet(set);
            }
        }
        else
        {
            printf("입력 오류: 존재하지 않는 연산입니다.\n");
        }
    }

    return 0;
}

// 두 문자열의 각 문자를 비교하는 함수
int isSameString(char first[], char second[])
{
    int i = 0;

    while (first[i] != '\0' && second[i] != '\0')
    {
        if (first[i] != second[i])
        {
            return 0;
        }

        i++;
    }

    // 두 문자열이 같은 위치에서 끝나야 같은 문자열이다.
    if (first[i] == second[i])
    {
        return 1;
    }

    return 0;
}

// 집합에 숫자를 추가
void addNumber(int set[], int number)
{
    set[number] = 1;
}

// 집합에서 숫자를 제거
void removeNumber(int set[], int number)
{
    set[number] = 0;
}

// 집합에 숫자가 있으면 1, 없으면 0을 반환
int checkNumber(int set[], int number)
{
    return set[number];
}

// 숫자가 있으면 제거하고, 없으면 추가
void toggleNumber(int set[], int number)
{
    if (set[number] == 1)
    {
        set[number] = 0;
    }
    else
    {
        set[number] = 1;
    }
}

// 집합에 1부터 20까지의 모든 숫자를 추가
void setAll(int set[])
{
    int i;

    for (i = 1; i <= MAX_NUMBER; i++)
    {
        set[i] = 1;
    }
}

// 집합의 모든 숫자를 제거
void setEmpty(int set[])
{
    int i;

    for (i = 1; i <= MAX_NUMBER; i++)
    {
        set[i] = 0;
    }
}

// 현재 집합에 들어 있는 숫자를 출력
void printSet(int set[])
{
    int i;
    int first = 1;

    printf("집합 : { ");

    for (i = 1; i <= MAX_NUMBER; i++)
    {
        if (set[i] == 1)
        {
            // 첫 번째 숫자가 아니라면 앞에 쉼표를 출력
            if (first == 0)
            {
                printf(", ");
            }

            printf("%d", i);
            first = 0;
        }
    }

    printf(" }\n");
}
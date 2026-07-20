#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 현재 깊이에 맞춰 들여쓰기 출력
void printIndent(int depth)
{
    int i;

    // 깊이 한 단계마다 공백 4칸 출력
    for (i = 0; i < depth; i++)
    {
        printf("    ");
    }
}

// 태그 이름에 사용할 수 있는 문자인지 확인
int isTagCharacter(char ch)
{
    // 소문자 허용
    if (ch >= 'a' && ch <= 'z')
    {
        return 1;
    }

    // 대문자 허용
    if (ch >= 'A' && ch <= 'Z')
    {
        return 1;
    }

    // 숫자 허용
    if (ch >= '0' && ch <= '9')
    {
        return 1;
    }

    // 하이픈과 밑줄 허용
    if (ch == '-' || ch == '_')
    {
        return 1;
    }

    // 사용할 수 없는 문자
    return 0;
}

// 태그 이름 형식 확인
int isValidTagName(const char* tag)
{
    int i;
    int length = (int)strlen(tag);

    // 태그 이름이 비어 있는 경우
    if (length == 0)
    {
        return 0;
    }

    // 첫 글자는 영문자만 가능
    if (!((tag[0] >= 'a' && tag[0] <= 'z') ||
        (tag[0] >= 'A' && tag[0] <= 'Z')))
    {
        return 0;
    }

    // 나머지 글자가 사용 가능한 문자인지 확인
    for (i = 1; i < length; i++)
    {
        if (isTagCharacter(tag[i]) == 0)
        {
            return 0;
        }
    }

    return 1;
}

// 태그 사이의 일반 문장 출력
void printText(const char* input, int start, int end, int depth)
{
    int i;

    // 문장 앞쪽 공백 제거
    while (start < end &&
        (input[start] == ' ' ||
            input[start] == '\t' ||
            input[start] == '\r' ||
            input[start] == '\n'))
    {
        start++;
    }

    // 문장 뒤쪽 공백 제거
    while (end > start &&
        (input[end - 1] == ' ' ||
            input[end - 1] == '\t' ||
            input[end - 1] == '\r' ||
            input[end - 1] == '\n'))
    {
        end--;
    }

    // 출력할 내용이 없으면 종료
    if (start >= end)
    {
        return;
    }

    // 현재 깊이에 맞춰 들여쓰기
    printIndent(depth);

    // 문장 내용 출력
    for (i = start; i < end; i++)
    {
        putchar(input[i]);
    }

    printf("\n");
}

// 스택에 남은 태그와 스택 메모리 해제
void freeStack(char** stack, int top)
{
    int i;

    // 스택에 저장된 태그 문자열 해제
    for (i = 0; i < top; i++)
    {
        free(stack[i]);
    }

    // 스택 자체 해제
    free(stack);
}

// HTML 태그 구조 검사 및 출력
int processHtml(const char* input, int printMode)
{
    char** stack;
    char* tag;

    int inputLength = (int)strlen(input);
    int top = 0;

    int i = 0;
    int textStart = 0;

    int tagStart;
    int tagEnd;
    int tagLength;
    int isClosing;
    int k;

    // 태그 주소를 저장할 스택 할당
    stack = (char**)malloc(
        (inputLength + 1) * sizeof(char*)
    );

    // 메모리 할당 실패
    if (stack == NULL)
    {
        return -1;
    }

    // 입력 문자열의 처음부터 끝까지 확인
    while (i < inputLength)
    {
        // '<'가 아니면 다음 문자로 이동
        if (input[i] != '<')
        {
            i++;
            continue;
        }

        // 태그 앞에 일반 문장이 있으면 출력
        if (printMode == 1 && textStart < i)
        {
            printText(input, textStart, i, top);
        }

        // 닫는 괄호 '>' 위치 찾기
        tagEnd = i + 1;

        while (tagEnd < inputLength &&
            input[tagEnd] != '>')
        {
            tagEnd++;
        }

        // '>'가 없으면 잘못된 태그
        if (tagEnd >= inputLength)
        {
            freeStack(stack, top);
            return 0;
        }

        // '<' 다음 위치부터 태그 이름 시작
        tagStart = i + 1;

        // 기본값은 여는 태그
        isClosing = 0;

        // '/'가 있으면 닫는 태그
        if (input[tagStart] == '/')
        {
            isClosing = 1;
            tagStart++;
        }

        // 태그 이름 길이 계산
        tagLength = tagEnd - tagStart;

        // 태그 이름을 저장할 공간 할당
        tag = (char*)malloc(
            (tagLength + 1) * sizeof(char)
        );

        // 메모리 할당 실패
        if (tag == NULL)
        {
            freeStack(stack, top);
            return -1;
        }

        // 입력 문자열에서 태그 이름 복사
        for (k = 0; k < tagLength; k++)
        {
            tag[k] = input[tagStart + k];
        }

        // 문자열 마지막에 널 문자 추가
        tag[tagLength] = '\0';

        // 잘못된 태그 이름인지 확인
        if (isValidTagName(tag) == 0)
        {
            free(tag);
            freeStack(stack, top);

            return 0;
        }

        // 여는 태그 처리
        if (isClosing == 0)
        {
            // 출력 모드일 때 태그 출력
            if (printMode == 1)
            {
                printIndent(top);
                printf("<%s>\n", tag);
            }

            // 여는 태그를 스택에 저장
            stack[top] = tag;
            top++;
        }
        else
        {
            // 스택이 비어 있으면 짝이 없는 닫는 태그
            if (top == 0)
            {
                free(tag);
                freeStack(stack, top);

                return 0;
            }

            // 최근에 연 태그와 현재 닫는 태그 비교
            if (strcmp(stack[top - 1], tag) != 0)
            {
                free(tag);
                freeStack(stack, top);

                return 0;
            }

            // 짝이 맞는 여는 태그 제거
            free(stack[top - 1]);
            top--;

            // 출력 모드일 때 닫는 태그 출력
            if (printMode == 1)
            {
                printIndent(top);
                printf("</%s>\n", tag);
            }

            // 현재 닫는 태그 메모리 해제
            free(tag);
        }

        // 현재 태그 다음 위치로 이동
        i = tagEnd + 1;

        // 다음 일반 문장의 시작 위치 저장
        textStart = i;
    }

    // 마지막 태그 뒤에 일반 문장이 있으면 출력
    if (printMode == 1 &&
        textStart < inputLength)
    {
        printText(
            input,
            textStart,
            inputLength,
            top
        );
    }

    // 스택에 태그가 남으면 닫히지 않은 태그 존재
    if (top != 0)
    {
        freeStack(stack, top);
        return 0;
    }

    // 사용이 끝난 스택 메모리 해제
    free(stack);

    return 1;
}

int main(void)
{
    char input[1000];
    int result;

    printf("HTML 태그 구조를 입력하세요: ");

    // 한 줄 전체 입력
    if (fgets(input, sizeof(input), stdin) == NULL)
    {
        printf("입력 오류가 발생했습니다.\n");
        return 1;
    }

    // 입력 끝의 줄바꿈 문자 제거
    input[strcspn(input, "\n")] = '\0';

    // 빈 문자열 입력 확인
    if (input[0] == '\0')
    {
        printf("입력 오류: HTML 태그 구조를 입력해야 합니다.\n");
        return 1;
    }

    // 출력 없이 태그 구조부터 검사
    result = processHtml(input, 0);

    // 메모리 할당 실패 확인
    if (result == -1)
    {
        printf("메모리 할당에 실패했습니다.\n");
        return 1;
    }

    // 잘못된 태그 구조 확인
    if (result == 0)
    {
        printf("오류: 잘못된 HTML 태그 구조입니다.\n");
        return 1;
    }

    printf("\n출력\n");

    // 올바른 구조를 들여쓰기해서 출력
    result = processHtml(input, 1);

    // 출력 중 메모리 할당 실패 확인
    if (result == -1)
    {
        printf("메모리 할당에 실패했습니다.\n");
        return 1;
    }

    return 0;
}
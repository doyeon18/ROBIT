#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>

typedef int Data;

// 하나의 노드에 데이터와 다음 노드의 주소를 저장한다.
typedef struct Node
{
    Data data;
    struct Node* next;
} Node;

// 입력 버퍼에 남아 있는 문자를 비운다.
void clear_input_buffer(void)
{
    int ch;

    while ((ch = getchar()) != '\n' && ch != EOF)
    {
    }
}

// 정수가 입력될 때까지 다시 입력받는다.
int input_int(const char* message)
{
    int value;
    int result;

    while (1)
    {
        printf("%s", message);
        result = scanf("%d", &value);

        if (result == 1)
        {
            clear_input_buffer();
            return value;
        }

        if (result == EOF)
        {
            printf("\n입력이 종료되었습니다.\n");
            exit(0);
        }

        printf("정수를 입력해주세요.\n");
        clear_input_buffer();
    }
}

// 스택에 데이터가 없으면 1을 반환하고 있으면 0을 반환한다.
int isEmpty(Node* stack)
{
    return stack == NULL;
}

// 새로운 노드를 만들어 스택의 맨 위에 추가한다.
void push(Node** stack, Data data)
{
    Node* new_node = (Node*)malloc(sizeof(Node));

    if (new_node == NULL)
    {
        printf("메모리 할당에 실패했습니다.\n");
        return;
    }

    new_node->data = data;
    new_node->next = *stack;
    *stack = new_node;

    printf("%d을(를) push했습니다.\n", data);
}

// 스택의 맨 위 노드를 삭제하고 삭제된 값을 출력한다.
void pop(Node** stack)
{
    Node* delete_node;
    Data pop_data;

    if (isEmpty(*stack))
    {
        printf("스택이 비어 있습니다.\n");
        return;
    }

    delete_node = *stack;
    pop_data = delete_node->data;
    *stack = delete_node->next;

    free(delete_node);

    printf("pop된 값: %d\n", pop_data);
}

// 스택에 저장된 노드의 개수를 세어 출력한다.
void size(Node* stack)
{
    Node* current = stack;
    int count = 0;

    while (current != NULL)
    {
        count++;
        current = current->next;
    }

    printf("스택 크기: %d\n", count);
}

// 스택의 맨 위에 저장된 값을 반환한다.
Data top(Node* stack)
{
    return stack->data;
}

// 스택의 맨 위부터 아래까지 모든 값을 출력한다.
void printStack(Node* stack)
{
    Node* current = stack;

    if (isEmpty(stack))
    {
        printf("스택이 비어 있습니다.\n");
        return;
    }

    printf("TOP -> ");

    while (current != NULL)
    {
        printf("%d", current->data);

        if (current->next != NULL)
        {
            printf(" -> ");
        }

        current = current->next;
    }

    printf(" -> NULL\n");
}

// 프로그램이 끝나기 전에 남아 있는 모든 노드의 메모리를 해제한다.
void freeStack(Node** stack)
{
    Node* delete_node;

    while (*stack != NULL)
    {
        delete_node = *stack;
        *stack = (*stack)->next;
        free(delete_node);
    }
}

// 사용자가 선택할 수 있는 기능 목록을 출력한다.
void print_menu(void)
{
    printf("\n========== 연결 리스트 스택 프로그램 ==========\n");
    printf("1. push\n");
    printf("2. pop\n");
    printf("3. size\n");
    printf("4. top\n");
    printf("5. isEmpty\n");
    printf("6. printStack\n");
    printf("0. 종료\n");
    printf("=======================================\n");
}

int main(void)
{
    Node* stack = NULL;
    int menu;
    Data data;


    while (1)
    {
        print_menu();
        menu = input_int("메뉴 선택: ");

        switch (menu)
        {
        case 1:
            data = input_int("push할 정수: ");
            push(&stack, data);
            printStack(stack);
            break;

        case 2:
            pop(&stack);
            printStack(stack);
            break;

        case 3:
            size(stack);
            break;

        case 4:
            if (isEmpty(stack))
            {
                printf("스택이 비어 있습니다.\n");
            }
            else
            {
                printf("top에 위치한 값: %d\n", top(stack));
            }
            break;

        case 5:
            if (isEmpty(stack))
            {
                printf("true: 스택이 비어 있습니다.\n");
            }
            else
            {
                printf("false: 스택에 데이터가 있습니다.\n");
            }
            break;

        case 6:
            printStack(stack);
            break;

        case 0:
            freeStack(&stack);
            printf("프로그램을 종료합니다.\n");
            return 0;

        default:
            printf("0부터 6 사이의 메뉴를 입력해주세요.\n");
        }
    }
}
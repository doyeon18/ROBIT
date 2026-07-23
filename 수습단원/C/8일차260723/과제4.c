#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>

#define MAX_LENGTH 1000

typedef char Data;

// 스택과 큐에서 사용할 하나의 노드를 정의한다.
typedef struct Node
{
    Data data;
    struct Node* next;
} Node;

// 큐의 맨 앞과 맨 뒤 노드의 주소를 저장한다.
typedef struct Queue
{
    Node* front;
    Node* rear;
} Queue;

// 스택이 비어 있으면 1을 반환하고 아니면 0을 반환한다.
int isStackEmpty(Node* stack)
{
    return stack == NULL;
}

// 큐가 비어 있으면 1을 반환하고 아니면 0을 반환한다.
int isQueueEmpty(Queue* queue)
{
    return queue->front == NULL;
}

// 스택의 맨 위에 문자를 추가한다.
int push(Node** stack, Data data)
{
    Node* new_node = (Node*)malloc(sizeof(Node));

    if (new_node == NULL)
    {
        return 0;
    }

    new_node->data = data;
    new_node->next = *stack;
    *stack = new_node;

    return 1;
}

// 스택의 맨 위 문자를 삭제하고 반환한다.
int pop(Node** stack, Data* data)
{
    Node* delete_node;

    if (isStackEmpty(*stack))
    {
        return 0;
    }

    delete_node = *stack;
    *data = delete_node->data;
    *stack = delete_node->next;

    free(delete_node);

    return 1;
}

// 큐의 맨 뒤에 문자를 추가한다.
int Enqueue(Queue* queue, Data data)
{
    Node* new_node = (Node*)malloc(sizeof(Node));

    if (new_node == NULL)
    {
        return 0;
    }

    new_node->data = data;
    new_node->next = NULL;

    if (isQueueEmpty(queue))
    {
        queue->front = new_node;
        queue->rear = new_node;
    }
    else
    {
        queue->rear->next = new_node;
        queue->rear = new_node;
    }

    return 1;
}

// 큐의 맨 앞 문자를 삭제하고 반환한다.
int Dequeue(Queue* queue, Data* data)
{
    Node* delete_node;

    if (isQueueEmpty(queue))
    {
        return 0;
    }

    delete_node = queue->front;
    *data = delete_node->data;
    queue->front = delete_node->next;

    if (queue->front == NULL)
    {
        queue->rear = NULL;
    }

    free(delete_node);

    return 1;
}

// 영문 대문자를 소문자로 변환한다.
char to_lower_character(char ch)
{
    if (ch >= 'A' && ch <= 'Z')
    {
        return ch + ('a' - 'A');
    }

    return ch;
}

// 공백 문자인지 확인한다.
int is_space_character(char ch)
{
    return ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r';
}

// 스택에 남아 있는 모든 노드의 메모리를 해제한다.
void freeStack(Node** stack)
{
    Data data;

    while (pop(stack, &data))
    {
    }
}

// 큐에 남아 있는 모든 노드의 메모리를 해제한다.
void freeQueue(Queue* queue)
{
    Data data;

    while (Dequeue(queue, &data))
    {
    }
}

// 문자열을 스택과 큐에 저장한 뒤 회문인지 판별한다.
int isPalindrome(const char* text, int* character_count)
{
    Node* stack = NULL;
    Queue queue = { NULL, NULL };
    Data stack_data;
    Data queue_data;
    int index = 0;
    int is_palindrome = 1;

    *character_count = 0;

    // 공백을 제외하고 영문 대문자는 소문자로 바꿔 저장한다.
    while (text[index] != '\0')
    {
        char ch = text[index];

        if (!is_space_character(ch))
        {
            ch = to_lower_character(ch);

            if (!push(&stack, ch) || !Enqueue(&queue, ch))
            {
                printf("메모리 할당에 실패했습니다.\n");
                freeStack(&stack);
                freeQueue(&queue);
                return -1;
            }

            (*character_count)++;
        }

        index++;
    }

    // 스택은 뒤에서부터, 큐는 앞에서부터 문자를 꺼내 비교한다.
    while (!isStackEmpty(stack) && !isQueueEmpty(&queue))
    {
        pop(&stack, &stack_data);
        Dequeue(&queue, &queue_data);

        if (stack_data != queue_data)
        {
            is_palindrome = 0;
            break;
        }
    }

    freeStack(&stack);
    freeQueue(&queue);

    return is_palindrome;
}

int main(void)
{
    char text[MAX_LENGTH];
    int character_count;
    int result;

    printf("회문을 판별할 문자열을 입력하세요.\n");
    printf("문자열 입력: ");

    if (fgets(text, sizeof(text), stdin) == NULL)
    {
        printf("문자열 입력에 실패했습니다.\n");
        return 1;
    }

    result = isPalindrome(text, &character_count);

    if (result == -1)
    {
        return 1;
    }

    if (character_count == 0)
    {
        printf("판별할 문자가 없습니다.\n");
    }
    else if (result)
    {
        printf("입력한 문자열은 회문입니다.\n");
    }
    else
    {
        printf("입력한 문자열은 회문이 아닙니다.\n");
    }

    return 0;
}
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

// 큐의 맨 앞과 맨 뒤 노드의 주소를 저장한다.
typedef struct Queue
{
    Node* frontNode;
    Node* rearNode;
} Queue;

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

// 큐에 데이터가 없으면 1을 반환하고 있으면 0을 반환한다.
int isEmpty(Queue* queue)
{
    return queue->frontNode == NULL;
}

// 새로운 노드를 만들어 큐의 맨 뒤에 추가한다.
void Enqueue(Queue* queue, Data data)
{
    Node* new_node = (Node*)malloc(sizeof(Node));

    if (new_node == NULL)
    {
        printf("메모리 할당에 실패했습니다.\n");
        return;
    }

    new_node->data = data;
    new_node->next = NULL;

    // 큐가 비어 있으면 새 노드가 맨 앞과 맨 뒤가 된다.
    if (isEmpty(queue))
    {
        queue->frontNode = new_node;
        queue->rearNode = new_node;
    }
    else
    {
        queue->rearNode->next = new_node;
        queue->rearNode = new_node;
    }

    printf("%d을(를) Enqueue했습니다.\n", data);
}

// 큐의 맨 앞 노드를 삭제하고 삭제된 값을 출력한다.
void Dequeue(Queue* queue)
{
    Node* delete_node;
    Data dequeue_data;

    if (isEmpty(queue))
    {
        printf("큐가 비어 있습니다.\n");
        return;
    }

    delete_node = queue->frontNode;
    dequeue_data = delete_node->data;
    queue->frontNode = delete_node->next;

    // 마지막 노드가 삭제되면 rearNode도 NULL로 바꾼다.
    if (queue->frontNode == NULL)
    {
        queue->rearNode = NULL;
    }

    free(delete_node);

    printf("Dequeue된 값: %d\n", dequeue_data);
}

// 큐에 저장된 노드의 개수를 세어 출력한다.
void size(Queue* queue)
{
    Node* current = queue->frontNode;
    int count = 0;

    while (current != NULL)
    {
        count++;
        current = current->next;
    }

    printf("큐 크기: %d\n", count);
}

// 큐의 맨 앞에 저장된 값을 반환한다.
Data front(Queue* queue)
{
    return queue->frontNode->data;
}

// 큐의 맨 뒤에 저장된 값을 반환한다.
Data rear(Queue* queue)
{
    return queue->rearNode->data;
}

// 큐의 맨 앞부터 맨 뒤까지 모든 값을 출력한다.
void printQueue(Queue* queue)
{
    Node* current = queue->frontNode;

    if (isEmpty(queue))
    {
        printf("큐가 비어 있습니다.\n");
        return;
    }

    printf("FRONT -> ");

    while (current != NULL)
    {
        printf("%d", current->data);

        if (current->next != NULL)
        {
            printf(" -> ");
        }

        current = current->next;
    }

    printf(" <- REAR\n");
}

// 프로그램이 끝나기 전에 남아 있는 모든 노드의 메모리를 해제한다.
void freeQueue(Queue* queue)
{
    Node* delete_node;

    while (queue->frontNode != NULL)
    {
        delete_node = queue->frontNode;
        queue->frontNode = queue->frontNode->next;
        free(delete_node);
    }

    queue->rearNode = NULL;
}

// 사용자가 선택할 수 있는 기능 목록을 출력한다.
void print_menu(void)
{
    printf("\n========== 연결 리스트 큐 ==========\n");
    printf("1. Enqueue\n");
    printf("2. Dequeue\n");
    printf("3. size\n");
    printf("4. front\n");
    printf("5. rear\n");
    printf("6. isEmpty\n");
    printf("7. printQueue\n");
    printf("0. 종료\n");
    printf("=====================================\n");
}

int main(void)
{
    Queue queue = { NULL, NULL };
    int menu;
    Data data;

    printf("단순 연결 리스트를 이용한 큐 프로그램입니다.\n");

    while (1)
    {
        print_menu();
        menu = input_int("메뉴 선택: ");

        switch (menu)
        {
        case 1:
            data = input_int("Enqueue할 정수: ");
            Enqueue(&queue, data);
            printQueue(&queue);
            break;

        case 2:
            Dequeue(&queue);
            printQueue(&queue);
            break;

        case 3:
            size(&queue);
            break;

        case 4:
            if (isEmpty(&queue))
            {
                printf("큐가 비어 있습니다.\n");
            }
            else
            {
                printf("front에 위치한 값: %d\n", front(&queue));
            }
            break;

        case 5:
            if (isEmpty(&queue))
            {
                printf("큐가 비어 있습니다.\n");
            }
            else
            {
                printf("rear에 위치한 값: %d\n", rear(&queue));
            }
            break;

        case 6:
            if (isEmpty(&queue))
            {
                printf("true: 큐가 비어 있습니다.\n");
            }
            else
            {
                printf("false: 큐에 데이터가 있습니다.\n");
            }
            break;

        case 7:
            printQueue(&queue);
            break;

        case 0:
            freeQueue(&queue);
            printf("프로그램을 종료합니다.\n");
            return 0;

        default:
            printf("0부터 7 사이의 메뉴를 입력해주세요.\n");
        }
    }
}
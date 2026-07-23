#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>

#define BY_DATA 1
#define BY_INDEX 2

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

// 데이터 기준 또는 인덱스 기준을 선택받는다.
int input_mode(void)
{
    int mode;

    while (1)
    {
        printf("1. 데이터 기준\n");
        printf("2. 인덱스 기준\n");
        mode = input_int("기준 선택: ");

        if (mode == BY_DATA || mode == BY_INDEX)
        {
            return mode;
        }

        printf("1 또는 2를 입력해주세요.\n");
    }
}

// 전달받은 데이터로 새로운 노드를 만든다.
Node* create_node(Data data)
{
    Node* new_node = (Node*)malloc(sizeof(Node));

    if (new_node == NULL)
    {
        printf("메모리 할당에 실패했습니다.\n");
        return NULL;
    }

    new_node->data = data;
    new_node->next = NULL;

    return new_node;
}

// 리스트의 맨 처음에 새 노드를 추가한다.
int insert_first(Node** head, Data data)
{
    Node* new_node = create_node(data);

    if (new_node == NULL)
    {
        return 0;
    }

    new_node->next = *head;
    *head = new_node;

    return 1;
}

// 리스트의 맨 마지막에 새 노드를 추가한다.
int insert_back(Node** head, Data data)
{
    Node* new_node = create_node(data);
    Node* current;

    if (new_node == NULL)
    {
        return 0;
    }

    if (*head == NULL)
    {
        *head = new_node;
        return 1;
    }

    current = *head;

    while (current->next != NULL)
    {
        current = current->next;
    }

    current->next = new_node;

    return 1;
}

// 데이터나 인덱스로 찾은 노드 뒤에 새 노드를 추가한다.
int insert(Node** head, int mode, int target, Data data)
{
    Node* current = *head;
    Node* new_node;
    int index = 0;

    if (*head == NULL)
    {
        printf("리스트가 비어 있습니다.\n");
        return 0;
    }

    if (mode == BY_INDEX)
    {
        if (target < 0)
        {
            printf("인덱스는 0 이상이어야 합니다.\n");
            return 0;
        }

        while (current != NULL && index < target)
        {
            current = current->next;
            index++;
        }
    }
    else
    {
        while (current != NULL && current->data != target)
        {
            current = current->next;
        }
    }

    if (current == NULL)
    {
        printf("기준이 되는 노드를 찾을 수 없습니다.\n");
        return 0;
    }

    new_node = create_node(data);

    if (new_node == NULL)
    {
        return 0;
    }

    new_node->next = current->next;
    current->next = new_node;

    return 1;
}

// 리스트의 맨 처음 노드를 삭제한다.
int delete_first(Node** head)
{
    Node* delete_node;

    if (*head == NULL)
    {
        printf("리스트가 비어 있습니다.\n");
        return 0;
    }

    delete_node = *head;
    *head = (*head)->next;
    free(delete_node);

    return 1;
}

// 리스트의 맨 마지막 노드를 삭제한다.
int delete_back(Node** head)
{
    Node* current;
    Node* previous = NULL;

    if (*head == NULL)
    {
        printf("리스트가 비어 있습니다.\n");
        return 0;
    }

    current = *head;

    while (current->next != NULL)
    {
        previous = current;
        current = current->next;
    }

    if (previous == NULL)
    {
        *head = NULL;
    }
    else
    {
        previous->next = NULL;
    }

    free(current);

    return 1;
}

// 데이터나 인덱스로 원하는 노드를 찾아 삭제한다.
int delete(Node** head, int mode, int target)
{
    Node* current = *head;
    Node* previous = NULL;
    int index = 0;

    if (*head == NULL)
    {
        printf("리스트가 비어 있습니다.\n");
        return 0;
    }

    if (mode == BY_INDEX && target < 0)
    {
        printf("인덱스는 0 이상이어야 합니다.\n");
        return 0;
    }

    while (current != NULL)
    {
        if ((mode == BY_DATA && current->data == target) ||
            (mode == BY_INDEX && index == target))
        {
            if (previous == NULL)
            {
                *head = current->next;
            }
            else
            {
                previous->next = current->next;
            }

            free(current);
            return 1;
        }

        previous = current;
        current = current->next;
        index++;
    }

    printf("삭제할 노드를 찾을 수 없습니다.\n");
    return 0;
}

// 데이터로 찾으면 인덱스를 반환하고 인덱스로 찾으면 데이터를 반환한다.
long long get_entry(Node* head, int mode, int target, int* found)
{
    Node* current = head;
    int index = 0;

    *found = 0;

    if (mode == BY_INDEX && target < 0)
    {
        return 0;
    }

    while (current != NULL)
    {
        if (mode == BY_DATA && current->data == target)
        {
            *found = 1;
            return index;
        }

        if (mode == BY_INDEX && index == target)
        {
            *found = 1;
            return current->data;
        }

        current = current->next;
        index++;
    }

    return 0;
}

// 리스트에 저장된 전체 노드 수를 반환한다.
int get_length(Node* head)
{
    Node* current = head;
    int length = 0;

    while (current != NULL)
    {
        length++;
        current = current->next;
    }

    return length;
}

// 리스트의 모든 노드를 인덱스와 함께 출력한다.
void print_list(Node* head)
{
    Node* current = head;
    int index = 0;

    if (head == NULL)
    {
        printf("[빈 리스트]\n");
        return;
    }

    while (current != NULL)
    {
        printf("[%d] %d -> ", index, current->data);
        current = current->next;
        index++;
    }

    printf("NULL\n");
}

// 각 노드의 연결 방향을 바꿔 리스트를 역순으로 만든다.
void reverse(Node** head)
{
    Node* previous = NULL;
    Node* current = *head;
    Node* next_node;

    while (current != NULL)
    {
        next_node = current->next;
        current->next = previous;
        previous = current;
        current = next_node;
    }

    *head = previous;
}

// 프로그램 종료 전에 남아 있는 모든 노드의 메모리를 해제한다.
void free_list(Node** head)
{
    Node* delete_node;

    while (*head != NULL)
    {
        delete_node = *head;
        *head = (*head)->next;
        free(delete_node);
    }
}

// 프로그램의 기능 목록을 출력한다.
void print_menu(void)
{
    printf("\n========== 단순 연결 리스트 프로그램 ==========\n");
    printf("1. 원하는 노드 뒤에 추가\n");
    printf("2. 맨 마지막에 추가\n");
    printf("3. 맨 처음에 추가\n");
    printf("4. 원하는 노드 삭제\n");
    printf("5. 맨 처음 노드 삭제\n");
    printf("6. 맨 마지막 노드 삭제\n");
    printf("7. 요소 찾기\n");
    printf("8. 리스트 길이 확인\n");
    printf("9. 리스트 전체 출력\n");
    printf("10. 리스트 역순 변환\n");
    printf("0. 종료\n");
    printf("======================================\n");
}

int main(void)
{
    Node* head = NULL;
    int menu;
    int mode;
    int target;
    int found;
    Data data;
    long long result;

    printf("데이터가 중복되면 앞에서 처음 찾은 노드를 사용\n");

    while (1)
    {
        print_menu();
        menu = input_int("메뉴 선택: ");

        switch (menu)
        {
        case 1:
            mode = input_mode();

            if (mode == BY_DATA)
            {
                target = input_int("기준이 되는 앞 노드의 데이터: ");
            }
            else
            {
                target = input_int("기준이 되는 앞 노드의 인덱스: ");
            }

            data = input_int("추가할 데이터: ");

            if (insert(&head, mode, target, data))
            {
                printf("노드를 추가했습니다.\n");
                print_list(head);
            }
            break;

        case 2:
            data = input_int("맨 마지막에 추가할 데이터: ");

            if (insert_back(&head, data))
            {
                printf("노드를 추가했습니다.\n");
                print_list(head);
            }
            break;

        case 3:
            data = input_int("맨 처음에 추가할 데이터: ");

            if (insert_first(&head, data))
            {
                printf("노드를 추가했습니다.\n");
                print_list(head);
            }
            break;

        case 4:
            mode = input_mode();

            if (mode == BY_DATA)
            {
                target = input_int("삭제할 데이터: ");
            }
            else
            {
                target = input_int("삭제할 노드의 인덱스: ");
            }

            if (delete(&head, mode, target))
            {
                printf("노드를 삭제했습니다.\n");
                print_list(head);
            }
            break;

        case 5:
            if (delete_first(&head))
            {
                printf("맨 처음 노드를 삭제했습니다.\n");
                print_list(head);
            }
            break;

        case 6:
            if (delete_back(&head))
            {
                printf("맨 마지막 노드를 삭제했습니다.\n");
                print_list(head);
            }
            break;

        case 7:
            mode = input_mode();

            if (mode == BY_DATA)
            {
                target = input_int("찾을 데이터: ");
                result = get_entry(head, mode, target, &found);

                if (found)
                {
                    printf("데이터 %d의 인덱스는 %lld입니다.\n", target, result);
                }
                else
                {
                    printf("해당 데이터를 찾을 수 없습니다.\n");
                }
            }
            else
            {
                target = input_int("찾을 인덱스: ");
                result = get_entry(head, mode, target, &found);

                if (found)
                {
                    printf("인덱스 %d의 데이터는 %lld입니다.\n", target, result);
                }
                else
                {
                    printf("해당 인덱스를 찾을 수 없습니다.\n");
                }
            }
            break;

        case 8:
            printf("리스트의 전체 길이는 %d입니다.\n", get_length(head));
            break;

        case 9:
            print_list(head);
            break;

        case 10:
            reverse(&head);
            printf("리스트를 역순으로 변경했습니다.\n");
            print_list(head);
            break;

        case 0:
            free_list(&head);
            printf("프로그램을 종료합니다.\n");
            return 0;

        default:
            printf("0부터 10 사이의 메뉴를 입력해주세요.\n");
        }
    }
}
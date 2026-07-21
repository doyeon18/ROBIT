#include <stdio.h>
#include <stdlib.h>

#define BY_INDEX 1
#define BY_DATA 2
#define NOT_FOUND -1

typedef struct _Node
{
    int data;
    struct _Node* next;
} Node;

typedef struct _LinkedList
{
    Node* head;
    Node* tail;
    Node* cur;
    int size;
} LinkedList;

// 연결 리스트를 빈 상태로 초기화
void init_list(LinkedList* list)
{
    list->head = NULL;
    list->tail = NULL;
    list->cur = NULL;
    list->size = 0;
}

// 데이터를 저장할 새로운 노드를 동적 할당
Node* create_node(int data)
{
    Node* new_node;

    new_node = (Node*)malloc(sizeof(Node));

    // 메모리 할당에 실패하면 NULL 반환
    if (new_node == NULL)
    {
        return NULL;
    }

    new_node->data = data;
    new_node->next = NULL;

    return new_node;
}

// 리스트의 맨 처음에 노드 추가
int insert_first(LinkedList* list, int data)
{
    Node* new_node;

    new_node = create_node(data);

    if (new_node == NULL)
    {
        return 0;
    }

    // 리스트가 비어 있으면 새 노드가 처음이자 마지막 노드
    if (list->head == NULL)
    {
        list->head = new_node;
        list->tail = new_node;
    }
    else
    {
        new_node->next = list->head;
        list->head = new_node;
    }

    list->size++;
    list->cur = NULL;

    return 1;
}

// 리스트의 맨 마지막에 노드 추가
int insert_back(LinkedList* list, int data)
{
    Node* new_node;

    new_node = create_node(data);

    if (new_node == NULL)
    {
        return 0;
    }

    // 리스트가 비어 있으면 새 노드가 처음이자 마지막 노드
    if (list->tail == NULL)
    {
        list->head = new_node;
        list->tail = new_node;
    }
    else
    {
        list->tail->next = new_node;
        list->tail = new_node;
    }

    list->size++;
    list->cur = NULL;

    return 1;
}

// 인덱스 또는 이전 데이터를 기준으로 원하는 위치에 노드 추가
int insert(LinkedList* list, int mode, int target, int data)
{
    Node* new_node;
    int index;

    // 인덱스를 기준으로 삽입
    if (mode == BY_INDEX)
    {
        // 삽입 인덱스는 0부터 현재 리스트 크기까지 가능
        if (target < 0 || target > list->size)
        {
            return 0;
        }

        if (target == 0)
        {
            return insert_first(list, data);
        }

        if (target == list->size)
        {
            return insert_back(list, data);
        }

        // 삽입 위치 바로 전 노드까지 이동
        list->cur = list->head;

        for (index = 0; index < target - 1; index++)
        {
            list->cur = list->cur->next;
        }
    }

    // 이전 데이터를 기준으로 삽입
    else if (mode == BY_DATA)
    {
        list->cur = list->head;

        // 기준 데이터가 있는 노드를 탐색
        while (list->cur != NULL
            && list->cur->data != target)
        {
            list->cur = list->cur->next;
        }

        // 기준 데이터를 찾지 못한 경우
        if (list->cur == NULL)
        {
            return 0;
        }

        // 기준 데이터가 마지막 노드에 있으면 맨 뒤에 삽입
        if (list->cur == list->tail)
        {
            return insert_back(list, data);
        }
    }
    else
    {
        return 0;
    }

    new_node = create_node(data);

    if (new_node == NULL)
    {
        return 0;
    }

    // 새 노드를 현재 노드와 다음 노드 사이에 연결
    new_node->next = list->cur->next;
    list->cur->next = new_node;

    list->size++;
    list->cur = NULL;

    return 1;
}

// 리스트의 첫 번째 노드 삭제
int delete_first(LinkedList* list)
{
    Node* delete_node;

    // 리스트가 비어 있으면 삭제할 수 없음
    if (list->head == NULL)
    {
        return 0;
    }

    delete_node = list->head;
    list->head = list->head->next;

    free(delete_node);

    list->size--;

    // 모든 노드가 삭제되면 tail도 NULL로 변경
    if (list->size == 0)
    {
        list->tail = NULL;
    }

    list->cur = NULL;

    return 1;
}

// 리스트의 마지막 노드 삭제
int delete_back(LinkedList* list)
{
    // 리스트가 비어 있으면 삭제할 수 없음
    if (list->head == NULL)
    {
        return 0;
    }

    // 노드가 하나뿐이면 첫 번째 삭제 함수 사용
    if (list->head == list->tail)
    {
        return delete_first(list);
    }

    list->cur = list->head;

    // 마지막 노드 바로 전 노드까지 이동
    while (list->cur->next != list->tail)
    {
        list->cur = list->cur->next;
    }

    free(list->tail);

    list->tail = list->cur;
    list->tail->next = NULL;

    list->size--;
    list->cur = NULL;

    return 1;
}

// 인덱스 또는 데이터를 기준으로 원하는 노드 삭제
int delete(LinkedList* list, int mode, int target)
{
    Node* delete_node;
    int index;

    // 리스트가 비어 있으면 삭제할 수 없음
    if (list->head == NULL)
    {
        return 0;
    }

    // 인덱스를 기준으로 삭제
    if (mode == BY_INDEX)
    {
        if (target < 0 || target >= list->size)
        {
            return 0;
        }

        if (target == 0)
        {
            return delete_first(list);
        }

        if (target == list->size - 1)
        {
            return delete_back(list);
        }

        // 삭제할 노드 바로 전 노드까지 이동
        list->cur = list->head;

        for (index = 0; index < target - 1; index++)
        {
            list->cur = list->cur->next;
        }

        delete_node = list->cur->next;
    }

    // 데이터를 기준으로 삭제
    else if (mode == BY_DATA)
    {
        // 첫 번째 노드가 삭제 대상인 경우
        if (list->head->data == target)
        {
            return delete_first(list);
        }

        list->cur = list->head;

        // 삭제할 노드 바로 전 노드까지 이동
        while (list->cur->next != NULL
            && list->cur->next->data != target)
        {
            list->cur = list->cur->next;
        }

        // 삭제할 데이터를 찾지 못한 경우
        if (list->cur->next == NULL)
        {
            return 0;
        }

        delete_node = list->cur->next;
    }
    else
    {
        return 0;
    }

    // 삭제할 노드를 건너뛰도록 연결 변경
    list->cur->next = delete_node->next;

    // 마지막 노드를 삭제한 경우 tail 변경
    if (delete_node == list->tail)
    {
        list->tail = list->cur;
    }

    free(delete_node);

    list->size--;
    list->cur = NULL;

    return 1;
}

// 데이터로 검색하면 인덱스를 반환하고 인덱스로 검색하면 데이터를 반환
int get_entry(LinkedList* list, int mode, int target)
{
    int index;

    // 인덱스를 기준으로 데이터 검색
    if (mode == BY_INDEX)
    {
        if (target < 0 || target >= list->size)
        {
            return NOT_FOUND;
        }

        list->cur = list->head;

        for (index = 0; index < target; index++)
        {
            list->cur = list->cur->next;
        }

        target = list->cur->data;
        list->cur = NULL;

        return target;
    }

    // 데이터를 기준으로 인덱스 검색
    if (mode == BY_DATA)
    {
        list->cur = list->head;
        index = 0;

        while (list->cur != NULL)
        {
            if (list->cur->data == target)
            {
                list->cur = NULL;

                return index;
            }

            list->cur = list->cur->next;
            index++;
        }

        list->cur = NULL;
    }

    return NOT_FOUND;
}

// 현재 리스트에 저장된 노드 개수 반환
int get_length(LinkedList* list)
{
    return list->size;
}

// 리스트의 모든 데이터 출력
void print_list(LinkedList* list)
{
    list->cur = list->head;

    if (list->cur == NULL)
    {
        printf("리스트가 비어 있습니다.\n");
        return;
    }

    while (list->cur != NULL)
    {
        printf("%d", list->cur->data);

        if (list->cur->next != NULL)
        {
            printf(" -> ");
        }

        list->cur = list->cur->next;
    }

    printf(" -> NULL\n");

    list->cur = NULL;
}

// 각 노드의 연결 방향을 반대로 변경
void reverse(LinkedList* list)
{
    Node* previous;
    Node* next_node;

    previous = NULL;
    list->cur = list->head;

    // 기존 첫 번째 노드는 역순 처리 후 마지막 노드가 됨
    list->tail = list->head;

    while (list->cur != NULL)
    {
        // 다음 노드의 위치를 먼저 저장
        next_node = list->cur->next;

        // 현재 노드가 이전 노드를 가리키게 변경
        list->cur->next = previous;

        // 포인터들을 다음 위치로 이동
        previous = list->cur;
        list->cur = next_node;
    }

    // 마지막으로 처리한 노드가 새로운 head가 됨
    list->head = previous;
    list->cur = NULL;
}

int main(void)
{
    LinkedList list;
    int result;

    // 연결 리스트를 빈 상태로 초기화
    init_list(&list);

    // 맨 뒤에 100, 200, 300을 순서대로 삽입
    if (insert_back(&list, 100) == 0
        || insert_back(&list, 200) == 0
        || insert_back(&list, 300) == 0)
    {
        printf("메모리 할당에 실패했습니다.\n");

        while (list.size > 0)
        {
            delete_first(&list);
        }

        return 1;
    }

    printf("초기 리스트: ");
    print_list(&list);

    // 처음, 특정 인덱스, 특정 데이터 뒤에 노드를 삽입
    if (insert_first(&list, 50) == 0
        || insert(&list, BY_INDEX, 2, 150) == 0
        || insert(&list, BY_DATA, 200, 250) == 0)
    {
        printf("노드 삽입에 실패했습니다.\n");

        while (list.size > 0)
        {
            delete_first(&list);
        }

        return 1;
    }

    printf("삽입 후: ");
    print_list(&list);

    // 데이터 250의 인덱스를 검색
    result = get_entry(&list, BY_DATA, 250);
    printf("데이터 250의 인덱스: %d\n", result);

    // 인덱스 3에 저장된 데이터를 검색
    result = get_entry(&list, BY_INDEX, 3);
    printf("인덱스 3의 데이터: %d\n", result);

    // 데이터와 인덱스를 기준으로 각각 노드 삭제
    delete(&list, BY_DATA, 150);
    delete(&list, BY_INDEX, 2);

    printf("일반 삭제 후: ");
    print_list(&list);

    // 첫 번째와 마지막 노드를 삭제
    delete_first(&list);
    delete_back(&list);

    printf("처음과 마지막 삭제 후: ");
    print_list(&list);

    // 리스트 순서를 반대로 변경
    reverse(&list);

    printf("역순 변환 후: ");
    print_list(&list);

    printf("리스트 길이: %d\n", get_length(&list));

    // 프로그램 종료 전 남은 모든 노드의 메모리 해제
    while (list.size > 0)
    {
        delete_first(&list);
    }

    return 0;
}
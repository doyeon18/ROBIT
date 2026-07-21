#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>

#define NAME_SIZE 20

typedef struct
{
    char name[NAME_SIZE];
    int price;
} Product;

int main(void)
{
    Product* products;
    char target[NAME_SIZE];

    int count;
    int i;
    int k;
    int same;
    int total = 0;

    // 구매할 물품의 개수를 입력받음
    if (scanf("%d", &count) != 1 || count <= 0)
    {
        return 1;
    }

    // 입력받은 개수만큼 구조체 배열을 동적 할당함
    products = (Product*)malloc(sizeof(Product) * count);

    // 메모리 할당에 실패하면 프로그램을 종료함
    if (products == NULL)
    {
        return 1;
    }

    // 각 물품의 이름과 금액을 구조체 배열에 저장함
    for (i = 0; i < count; i++)
    {
        if (scanf("%19s %d",
            products[i].name,
            &products[i].price) != 2)
        {
            free(products);
            return 1;
        }
    }

    // 금액의 총합을 구할 물품 이름을 입력받음
    if (scanf("%19s", target) != 1)
    {
        free(products);
        return 1;
    }

    // 입력된 이름과 같은 모든 물품의 금액을 더함
    for (i = 0; i < count; i++)
    {
        same = 1;
        k = 0;

        // 두 이름을 첫 글자부터 직접 비교함
        while (products[i].name[k] != '\0'
            || target[k] != '\0')
        {
            if (products[i].name[k] != target[k])
            {
                same = 0;
                break;
            }

            k++;
        }

        // 이름이 같으면 해당 물품의 금액을 더함
        if (same == 1)
        {
            total += products[i].price;
        }
    }

    // 같은 이름이 없으면 total의 초깃값인 0이 출력됨
    printf("%d\n", total);

    // 동적으로 할당한 메모리를 해제함
    free(products);

    return 0;
}
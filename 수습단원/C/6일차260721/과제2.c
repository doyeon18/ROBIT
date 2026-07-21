#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef struct
{
    int x;
    int y;
} Point;

int main(void)
{
    Point* points;

    int count;
    int i;
    int j;
    int farthest_index = 0;

    double dx;
    double dy;
    double distance;
    double total_distance;
    double max_distance = 0.0;

    // 입력받을 좌표의 개수를 입력
    printf("입력할 좌표 수: ");

    if (scanf("%d", &count) != 1 || count <= 0)
    {
        printf("올바른 좌표 수를 입력하세요.\n");
        return 1;
    }

    // 입력받은 좌표 개수만큼 구조체 배열을 동적 할당
    points = (Point*)malloc(sizeof(Point) * count);

    // 메모리 할당에 실패했는지 확인
    if (points == NULL)
    {
        printf("메모리 할당에 실패했습니다.\n");
        return 1;
    }

    // 모든 좌표를 입력
    for (i = 0; i < count; i++)
    {
        printf("%d번째 좌표 입력(x y): ", i + 1);

        if (scanf("%d %d", &points[i].x, &points[i].y) != 2)
        {
            printf("입력 오류입니다.\n");

            free(points);

            return 1;
        }
    }

    // 각 좌표에서 다른 모든 좌표까지의 거리 합을 계산
    for (i = 0; i < count; i++)
    {
        total_distance = 0.0;

        for (j = 0; j < count; j++)
        {
            // 자기 자신과의 거리는 계산하지 않음
            if (i != j)
            {
                dx = points[i].x - points[j].x;
                dy = points[i].y - points[j].y;

                distance = sqrt(dx * dx + dy * dy);

                total_distance += distance;
            }
        }

        // 거리 합이 가장 큰 좌표의 위치를 저장
        if (i == 0 || total_distance > max_distance)
        {
            max_distance = total_distance;
            farthest_index = i;
        }
    }

    // 가장 멀리 떨어진 좌표와 거리 총합을 출력
    printf(
        "가장 거리가 먼 좌표는 (%d, %d)이며, 다른 좌표의 거리 총합은 약 %.1f입니다.\n",
        points[farthest_index].x,
        points[farthest_index].y,
        max_distance
    );

    // 동적으로 할당한 메모리를 해제
    free(points);

    return 0;
}
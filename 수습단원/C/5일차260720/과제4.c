#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    int* array;
    int count;
    int i;

    int max;
    int min;
    long long sum = 0;
    double average;

    printf("몇 개의 원소를 할당하겠습니까? : ");

    // 배열 원소 개수 입력 및 예외 처리
    if (scanf("%d", &count) != 1)
    {
        printf("입력 오류: 정수를 입력해야 합니다.\n");
        return 1;
    }

    if (count <= 0)
    {
        printf("입력 오류: 원소의 개수는 1 이상이어야 합니다.\n");
        return 1;
    }

    // 입력받은 개수만큼 정수형 배열을 동적 할당
    array = (int*)malloc(count * sizeof(int));

    // 메모리 할당 실패 확인
    if (array == NULL)
    {
        printf("메모리 할당에 실패했습니다.\n");
        return 1;
    }

    // 배열에 정수 데이터 입력
    for (i = 0; i < count; i++)
    {
        printf("정수형 데이터 입력: ");

        if (scanf("%d", &array[i]) != 1)
        {
            printf("입력 오류: 정수를 입력해야 합니다.\n");

            // 프로그램 종료 전 동적 메모리 해제
            free(array);

            return 1;
        }
    }

    // 첫 번째 원소를 최댓값과 최솟값의 초기값으로 설정
    max = array[0];
    min = array[0];

    // 최댓값, 최솟값, 전체 합 계산
    for (i = 0; i < count; i++)
    {
        sum += array[i];

        if (array[i] > max)
        {
            max = array[i];
        }

        if (array[i] < min)
        {
            min = array[i];
        }
    }

    // 정수 나눗셈을 방지하기 위해 double로 형변환
    average = (double)sum / count;

    // 계산 결과 출력
    printf("\n최대값: %d\n", max);
    printf("최소값: %d\n", min);
    printf("전체합: %lld\n", sum);
    printf("평균: %f\n", average);

    // 사용이 끝난 동적 메모리 해제
    free(array);

    return 0;
}
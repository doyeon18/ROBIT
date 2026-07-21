#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>

#define STUDENT_COUNT 5
#define NAME_SIZE 20

typedef struct
{
    int year;
    double score;
    char name[NAME_SIZE];
} Student;

int main(void)
{
    Student students[STUDENT_COUNT];
    Student temp;

    int i;
    int j;
    int k;
    int need_swap;

    // 학생 5명의 정보를 입력
    for (i = 0; i < STUDENT_COUNT; i++)
    {
        if (scanf("%d %lf %19s",
            &students[i].year,
            &students[i].score,
            students[i].name) != 3)
        {
            printf("입력 오류입니다.\n");
            return 1;
        }
    }

    // 버블 정렬로 학생들의 순서를 정렬
    for (i = 0; i < STUDENT_COUNT - 1; i++)
    {
        for (j = 0; j < STUDENT_COUNT - 1 - i; j++)
        {
            need_swap = 0;

            // 학년이 큰 학생을 뒤로 이동
            if (students[j].year > students[j + 1].year)
            {
                need_swap = 1;
            }

            // 학년이 같으면 성적을 비교
            else if (students[j].year == students[j + 1].year)
            {
                // 성적이 높은 학생을 뒤로 이동
                if (students[j].score > students[j + 1].score)
                {
                    need_swap = 1;
                }

                // 학년과 성적이 같으면 이름을 비교
                else if (students[j].score == students[j + 1].score)
                {
                    k = 0;

                    // 서로 다른 문자가 나올 때까지 한 글자씩 비교
                    while (students[j].name[k] == students[j + 1].name[k]
                        && students[j].name[k] != '\0')
                    {
                        k++;
                    }

                    // 사전순으로 뒤에 있는 이름을 뒤로 이동
                    if (students[j].name[k] > students[j + 1].name[k])
                    {
                        need_swap = 1;
                    }
                }
            }

            // 순서가 잘못된 두 구조체를 교환
            if (need_swap == 1)
            {
                temp = students[j];
                students[j] = students[j + 1];
                students[j + 1] = temp;
            }
        }
    }

    // 정렬된 학생 정보를 출력
    for (i = 0; i < STUDENT_COUNT; i++)
    {
        printf("%d %.1f %s\n",
            students[i].year,
            students[i].score,
            students[i].name);
    }

    return 0;
}
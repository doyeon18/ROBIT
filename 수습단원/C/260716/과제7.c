#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>

// 과제7_로빛_21기_수습단원_박도연
// n과 r을 입력받아 순열, 중복순열, 조합, 중복조합을 계산한다.
int main(void)
{
    int n;
    int r;
    int i;

    double permutation = 1.0;
    double repeatedPermutation = 1.0;
    double combination = 1.0;
    double repeatedCombination = 1.0;

    printf("n을 입력하세요: ");

    if (scanf("%d", &n) != 1)
    {
        printf("입력 오류: n은 정수이어야 합니다.\n");
        return 1;
    }

    printf("r을 입력하세요: ");

    if (scanf("%d", &r) != 1)
    {
        printf("입력 오류: r은 정수이어야 합니다.\n");
        return 1;
    }

    if (n < 1 || r < 0 || r > n)
    {
        printf("입력 오류: n은 1 이상이고, r은 0 이상 n 이하이어야 합니다.\n");
        return 1;
    }

    // 순열 nPr
    for (i = 0; i < r; i++)
    {
        permutation *= n - i;
    }

    // 중복순열 nΠr
    for (i = 0; i < r; i++)
    {
        repeatedPermutation *= n;
    }

    // 조합 nCr
    for (i = 1; i <= r; i++)
    {
        combination = combination * (n - r + i) / i;
    }

    // 중복조합 nHr
    for (i = 1; i <= r; i++)
    {
        repeatedCombination =
            repeatedCombination * (n + i - 1) / i;
    }

    printf("순열 nPr = %.0f\n", permutation);
    printf("중복순열 nΠr = %.0f\n", repeatedPermutation);
    printf("조합 nCr = %.0f\n", combination);
    printf("중복조합 nHr = %.0f\n", repeatedCombination);

    return 0;
}
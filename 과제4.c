#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>


// 과제4_로빛_21기_수습단원_박도연

// 사칙연산과 정수 지수의 거듭제곱을 계산한다.
int main(void)
{
    double number1;
    double number2;
    double result = 0.0;
    char operator;
    int exponent;
    int i;

    printf("Input Arithmetic Operation\n");
    printf("ex) 3.4 * 8.5\n");
    printf("    2.9 - 5.4\n");
    printf("    3.9 / 8.0\n");
    printf("    3.9 ^ 8\n\n");
    printf("input: ");

    if (scanf("%lf %c", &number1, &operator) != 2)
    {
        printf("입력 오류: 숫자와 연산자를 확인하세요.\n");
        return 1;
    }

    if (operator == '^')
    {
        if (scanf("%d", &exponent) != 1)
        {
            printf("입력 오류: 거듭제곱의 지수는 정수이어야 합니다.\n");
            return 1;
        }

        if (number1 == 0.0 && exponent < 0)
        {
            printf("계산 오류: 0의 음수 거듭제곱은 계산할 수 없습니다.\n");
            return 1;
        }

        result = 1.0;

        if (exponent >= 0)
        {
            for (i = 0; i < exponent; i++)
            {
                result *= number1;
            }
        }
        else
        {
            for (i = 0; i < -exponent; i++)
            {
                result *= number1;
            }

            result = 1.0 / result;
        }

        printf("%.2f %c %d = %.2f\n", number1, operator, exponent, result);
    }
    else
    {
        if (scanf("%lf", &number2) != 1)
        {
            printf("입력 오류: 두 번째 숫자를 입력해야 합니다.\n");
            return 1;
        }

        if (operator == '+')
        {
            result = number1 + number2;
        }
        else if (operator == '-')
        {
            result = number1 - number2;
        }
        else if (operator == '*')
        {
            result = number1 * number2;
        }
        else if (operator == '/')
        {
            if (number2 == 0.0)
            {
                printf("계산 오류: 0으로 나눌 수 없습니다.\n");
                return 1;
            }

            result = number1 / number2;
        }
        else
        {
            printf("입력 오류: +, -, *, /, ^ 중 하나를 사용하세요.\n");
            return 1;
        }

        printf("%.2f %c %.2f = %.2f\n", number1, operator, number2, result);
    }

    return 0;
}

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>

typedef struct
{
    int year;
    int month;
    int day;
} DATE;

typedef struct
{
    int hour;
    int minute;
    int second;
} TIME;

typedef struct
{
    DATE date;
    TIME time;
} TIMESTAMP;

// 입력받은 연도가 윤년인지 확인
int is_leap_year(int year)
{
    if (year % 400 == 0)
    {
        return 1;
    }

    if (year % 100 == 0)
    {
        return 0;
    }

    if (year % 4 == 0)
    {
        return 1;
    }

    return 0;
}

// 입력받은 연도와 월에 해당하는 날짜 수를 반환
int get_days_in_month(int year, int month)
{
    int days[12] =
    {
        31, 28, 31, 30, 31, 30,
        31, 31, 30, 31, 30, 31
    };

    // 윤년이면 2월의 날짜 수는 29일
    if (month == 2 && is_leap_year(year) == 1)
    {
        return 29;
    }

    return days[month - 1];
}

// 날짜와 시간의 범위가 올바른지 확인
int is_valid_timestamp(TIMESTAMP value)
{
    int last_day;

    // 연도는 1 이상이어야 함
    if (value.date.year < 1)
    {
        return 0;
    }

    // 월은 1부터 12까지 입력할 수 있음
    if (value.date.month < 1 || value.date.month > 12)
    {
        return 0;
    }

    last_day = get_days_in_month(
        value.date.year,
        value.date.month
    );

    // 일은 해당 월의 날짜 범위 안에 있어야 함
    if (value.date.day < 1 || value.date.day > last_day)
    {
        return 0;
    }

    // 시는 0부터 23까지 입력할 수 있음
    if (value.time.hour < 0 || value.time.hour > 23)
    {
        return 0;
    }

    // 분은 0부터 59까지 입력할 수 있음
    if (value.time.minute < 0 || value.time.minute > 59)
    {
        return 0;
    }

    // 초는 0부터 59까지 입력할 수 있음
    if (value.time.second < 0 || value.time.second > 59)
    {
        return 0;
    }

    return 1;
}

// 날짜와 시간을 초 단위의 값으로 변환
long long convert_to_seconds(TIMESTAMP value)
{
    long long total_days = 0;
    long long total_seconds;

    int year;
    int month;

    // 입력 연도 이전까지의 모든 날짜 수를 계산
    for (year = 1; year < value.date.year; year++)
    {
        if (is_leap_year(year) == 1)
        {
            total_days += 366;
        }
        else
        {
            total_days += 365;
        }
    }

    // 입력 월 이전까지의 날짜 수를 계산
    for (month = 1; month < value.date.month; month++)
    {
        total_days += get_days_in_month(
            value.date.year,
            month
        );
    }

    // 현재 월에서 지나간 날짜 수를 계산
    total_days += value.date.day - 1;

    // 전체 날짜와 시간을 초 단위로 변환
    total_seconds = total_days * 24 * 60 * 60;
    total_seconds += value.time.hour * 60 * 60;
    total_seconds += value.time.minute * 60;
    total_seconds += value.time.second;

    return total_seconds;
}

int main(void)
{
    TIMESTAMP first;
    TIMESTAMP second;

    long long first_seconds;
    long long second_seconds;
    long long difference;

    long long hours;
    long long minutes;
    long long seconds;

    // 첫 번째 날짜와 시간을 입력
    if (scanf("%d %d %d %d %d %d",
        &first.date.year,
        &first.date.month,
        &first.date.day,
        &first.time.hour,
        &first.time.minute,
        &first.time.second) != 6)
    {
        printf("입력 오류입니다.\n");
        return 1;
    }

    // 두 번째 날짜와 시간을 입력
    if (scanf("%d %d %d %d %d %d",
        &second.date.year,
        &second.date.month,
        &second.date.day,
        &second.time.hour,
        &second.time.minute,
        &second.time.second) != 6)
    {
        printf("입력 오류입니다.\n");
        return 1;
    }

    // 입력된 날짜와 시간의 범위를 검사
    if (is_valid_timestamp(first) == 0
        || is_valid_timestamp(second) == 0)
    {
        printf("올바르지 않은 날짜 또는 시간입니다.\n");
        return 1;
    }

    // 두 날짜와 시간을 각각 초 단위로 변환
    first_seconds = convert_to_seconds(first);
    second_seconds = convert_to_seconds(second);

    // 입력 순서와 관계없이 두 시각의 차이를 계산
    if (first_seconds > second_seconds)
    {
        difference = first_seconds - second_seconds;
    }
    else
    {
        difference = second_seconds - first_seconds;
    }

    // 전체 초를 시, 분, 초로 나누어 저장
    hours = difference / 3600;
    minutes = (difference % 3600) / 60;
    seconds = difference % 60;

    // 두 시각의 차이를 출력
    printf("%lld시 %lld분 %lld초\n",
        hours,
        minutes,
        seconds);

    return 0;
}
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define MAX_STUDENTS 100
#define NAME_LENGTH 50
#define ADDRESS_LENGTH 50
#define LINE_LENGTH 512
#define FILE_NAME_LENGTH 200

#define SORT_NUMBER 1
#define SORT_NAME 2
#define SORT_COUNTRY 3
#define SORT_PROVINCE 4
#define SORT_CITY 5
#define SORT_DISTRICT 6
#define SORT_GRADE 7

#define ORDER_ASCENDING 1
#define ORDER_DESCENDING 2

typedef struct Address
{
    char country[ADDRESS_LENGTH];
    char province[ADDRESS_LENGTH];
    char city[ADDRESS_LENGTH];
    char district[ADDRESS_LENGTH];
} Address;

// 학생의 번호, 이름, 주소, 성적을 저장한다.
typedef struct Student
{
    int number;
    char name[NAME_LENGTH];
    Address address;
    double grade;
} Student;

// 입력 버퍼에 남아 있는 문자를 비운다.
void clear_input_buffer(void)
{
    int ch;

    while ((ch = getchar()) != '\n' && ch != EOF)
    {
    }
}

// 문자열의 앞뒤 공백을 제거한다.
void trim_spaces(char* text)
{
    int start = 0;
    int end;
    int length;

    while (text[start] == ' ' || text[start] == '\t')
    {
        start++;
    }

    if (start > 0)
    {
        memmove(text, text + start, strlen(text + start) + 1);
    }

    length = (int)strlen(text);
    end = length - 1;

    while (end >= 0 && (text[end] == ' ' || text[end] == '\t'))
    {
        text[end] = '\0';
        end--;
    }
}

// 한 줄을 안전하게 입력받는다.
int read_line(const char* message, char* buffer, int size)
{
    char* newline;

    printf("%s", message);

    if (fgets(buffer, size, stdin) == NULL)
    {
        printf("\n입력이 종료되었습니다.\n");
        exit(0);
    }

    newline = strchr(buffer, '\n');

    if (newline != NULL)
    {
        *newline = '\0';

        if (newline > buffer && *(newline - 1) == '\r')
        {
            *(newline - 1) = '\0';
        }

        return 1;
    }

    clear_input_buffer();
    printf("입력이 너무 깁니다. 다시 입력해주세요.\n");
    return 0;
}

// 문자열이 비어 있는지 확인한다.
int is_blank(const char* text)
{
    int index = 0;

    while (text[index] != '\0')
    {
        if (text[index] != ' ' && text[index] != '\t')
        {
            return 0;
        }

        index++;
    }

    return 1;
}

// 문자열이 숫자와 공백으로만 이루어졌는지 확인한다.
int is_number_only(const char* text)
{
    int index = 0;
    int has_number = 0;

    while (text[index] != '\0')
    {
        if (text[index] >= '0' && text[index] <= '9')
        {
            has_number = 1;
        }
        else if (text[index] != ' ' && text[index] != '\t')
        {
            return 0;
        }

        index++;
    }

    return has_number;
}

// 파일 구분자로 사용하는 문자가 포함되어 있는지 확인한다.
int contains_separator(const char* text)
{
    return strchr(text, '|') != NULL;
}

// 이름과 주소에 사용할 문자열이 올바른지 확인한다.
int is_valid_text(const char* text)
{
    if (is_blank(text))
    {
        return 0;
    }

    if (is_number_only(text))
    {
        return 0;
    }

    if (contains_separator(text))
    {
        return 0;
    }

    return 1;
}

// 지정한 범위의 정수를 입력받는다.
int input_int_range(const char* message, int minimum, int maximum)
{
    char input[100];
    char* end;
    long value;

    while (1)
    {
        if (!read_line(message, input, sizeof(input)))
        {
            continue;
        }

        trim_spaces(input);

        if (is_blank(input))
        {
            printf("값을 입력해주세요.\n");
            continue;
        }

        value = strtol(input, &end, 10);

        while (*end == ' ' || *end == '\t')
        {
            end++;
        }

        if (*end != '\0')
        {
            printf("정수만 입력해주세요.\n");
            continue;
        }

        if (value < minimum || value > maximum || value < INT_MIN || value > INT_MAX)
        {
            printf("%d부터 %d 사이의 값을 입력해주세요.\n", minimum, maximum);
            continue;
        }

        return (int)value;
    }
}

// 지정한 범위의 실수를 입력받는다.
double input_double_range(const char* message, double minimum, double maximum)
{
    char input[100];
    char* end;
    double value;

    while (1)
    {
        if (!read_line(message, input, sizeof(input)))
        {
            continue;
        }

        trim_spaces(input);

        if (is_blank(input))
        {
            printf("값을 입력해주세요.\n");
            continue;
        }

        value = strtod(input, &end);

        while (*end == ' ' || *end == '\t')
        {
            end++;
        }

        if (*end != '\0' || value != value)
        {
            printf("숫자만 입력해주세요.\n");
            continue;
        }

        if (value < minimum || value > maximum)
        {
            printf("%.1f부터 %.1f 사이의 값을 입력해주세요.\n", minimum, maximum);
            continue;
        }

        return value;
    }
}

// 이름이나 주소에 사용할 문자열을 입력받는다.
void input_text(const char* message, char* destination, int size)
{
    while (1)
    {
        if (!read_line(message, destination, size))
        {
            continue;
        }

        trim_spaces(destination);

        if (is_blank(destination))
        {
            printf("문자를 입력해주세요.\n");
            continue;
        }

        if (is_number_only(destination))
        {
            printf("숫자만으로 구성된 값은 사용할 수 없습니다.\n");
            continue;
        }

        if (contains_separator(destination))
        {
            printf("'|' 문자는 사용할 수 없습니다.\n");
            continue;
        }

        return;
    }
}

// 학생 한 명의 모든 정보를 입력받는다.
void input_student(Student* student)
{
    student->number = input_int_range("번호: ", 1, INT_MAX);
    input_text("이름: ", student->name, NAME_LENGTH);
    input_text("주소 - 나라: ", student->address.country, ADDRESS_LENGTH);
    input_text("주소 - 도/특별시/광역시: ", student->address.province, ADDRESS_LENGTH);
    input_text("주소 - 시: ", student->address.city, ADDRESS_LENGTH);
    input_text("주소 - 구: ", student->address.district, ADDRESS_LENGTH);
    student->grade = input_double_range("성적(0~100): ", 0.0, 100.0);
}

// 학생 한 명의 정보를 출력한다.
void print_student(const Student* student, int position)
{
    printf("[%d] 번호: %d | 이름: %s | 주소: %s, %s, %s, %s | 성적: %.2f\n",
        position,
        student->number,
        student->name,
        student->address.country,
        student->address.province,
        student->address.city,
        student->address.district,
        student->grade);
}

// 출석부에 저장된 모든 학생을 출력한다.
void print_all_students(const Student* students, int count)
{
    int index;

    if (count == 0)
    {
        printf("출석부에 학생이 없습니다.\n");
        return;
    }

    printf("\n========== 출석부 ==========\n");

    for (index = 0; index < count; index++)
    {
        print_student(students + index, index + 1);
    }

    printf("============================\n");
}

// 두 실수의 차이가 매우 작은지 확인한다.
int is_same_grade(double first, double second)
{
    double difference = first - second;

    if (difference < 0)
    {
        difference = -difference;
    }

    return difference < 0.000001;
}

// 두 학생의 모든 정보가 같은지 확인한다.
int is_same_student(const Student* first, const Student* second)
{
    return first->number == second->number &&
        strcmp(first->name, second->name) == 0 &&
        strcmp(first->address.country, second->address.country) == 0 &&
        strcmp(first->address.province, second->address.province) == 0 &&
        strcmp(first->address.city, second->address.city) == 0 &&
        strcmp(first->address.district, second->address.district) == 0 &&
        is_same_grade(first->grade, second->grade);
}

// 출석부의 맨 뒤에 학생을 추가한다.
void add_student(Student* students, int* count)
{
    if (*count >= MAX_STUDENTS)
    {
        printf("출석부가 가득 찼습니다.\n");
        return;
    }

    printf("\n추가할 학생 정보를 입력하세요.\n");
    input_student(students + *count);
    (*count)++;

    printf("학생을 추가했습니다.\n");
    print_student(students + (*count - 1), *count);
}

// 지정한 위치의 학생을 삭제하고 뒤의 학생을 앞으로 이동한다.
void remove_student_at(Student* students, int* count, int index)
{
    int move_index;

    for (move_index = index; move_index < *count - 1; move_index++)
    {
        students[move_index] = students[move_index + 1];
    }

    (*count)--;
}

// 모든 정보를 입력받아 일치하는 학생을 삭제한다.
void delete_student(Student* students, int* count)
{
    Student target;
    int matches[MAX_STUDENTS];
    int match_count = 0;
    int index;
    int choice;
    int delete_index;

    if (*count == 0)
    {
        printf("출석부에 학생이 없습니다.\n");
        return;
    }

    printf("\n삭제할 학생의 모든 정보를 입력하세요.\n");
    input_student(&target);

    for (index = 0; index < *count; index++)
    {
        if (is_same_student(students + index, &target))
        {
            matches[match_count] = index;
            match_count++;
        }
    }

    if (match_count == 0)
    {
        printf("입력한 정보와 일치하는 학생이 없습니다.\n");
        return;
    }

    if (match_count == 1)
    {
        delete_index = matches[0];
    }
    else
    {
        printf("일치하는 학생이 여러 명입니다.\n");

        for (index = 0; index < match_count; index++)
        {
            printf("%d. 출석부 위치 %d\n", index + 1, matches[index] + 1);
            print_student(students + matches[index], matches[index] + 1);
        }

        choice = input_int_range("삭제할 학생 선택: ", 1, match_count);
        delete_index = matches[choice - 1];
    }

    print_student(students + delete_index, delete_index + 1);
    remove_student_at(students, count, delete_index);
    printf("학생을 삭제했습니다.\n");
}

// 선택한 기준에 따라 두 학생을 비교한다.
int compare_students(const Student* first, const Student* second, int sort_type)
{
    int result = 0;

    switch (sort_type)
    {
    case SORT_NUMBER:
        if (first->number < second->number)
        {
            result = -1;
        }
        else if (first->number > second->number)
        {
            result = 1;
        }
        break;

    case SORT_NAME:
        result = strcmp(first->name, second->name);
        break;

    case SORT_COUNTRY:
        result = strcmp(first->address.country, second->address.country);
        break;

    case SORT_PROVINCE:
        result = strcmp(first->address.province, second->address.province);
        break;

    case SORT_CITY:
        result = strcmp(first->address.city, second->address.city);
        break;

    case SORT_DISTRICT:
        result = strcmp(first->address.district, second->address.district);
        break;

    case SORT_GRADE:
        if (first->grade < second->grade)
        {
            result = -1;
        }
        else if (first->grade > second->grade)
        {
            result = 1;
        }
        break;
    }

    // 선택한 값이 같을 때는 번호가 작은 학생을 먼저 배치한다.
    if (result == 0 && sort_type != SORT_NUMBER)
    {
        if (first->number < second->number)
        {
            result = -1;
        }
        else if (first->number > second->number)
        {
            result = 1;
        }
    }

    return result;
}

// 구조체 배열의 복사본을 버블 정렬하여 출력한다.
void sort_students(const Student* students, int count)
{
    Student sorted[MAX_STUDENTS];
    Student temporary;
    int sort_type;
    int order;
    int index;
    int compare_index;
    int result;

    if (count == 0)
    {
        printf("출석부에 학생이 없습니다.\n");
        return;
    }

    printf("\n정렬 기준을 선택하세요.\n");
    printf("1. 번호순\n");
    printf("2. 이름순\n");
    printf("3. 주소 - 나라순\n");
    printf("4. 주소 - 도/특별시/광역시순\n");
    printf("5. 주소 - 시순\n");
    printf("6. 주소 - 구순\n");
    printf("7. 성적순\n");

    sort_type = input_int_range("정렬 기준: ", 1, 7);

    printf("1. 오름차순\n");
    printf("2. 내림차순\n");
    order = input_int_range("정렬 방향: ", 1, 2);

    for (index = 0; index < count; index++)
    {
        sorted[index] = students[index];
    }

    for (index = 0; index < count - 1; index++)
    {
        for (compare_index = 0; compare_index < count - 1 - index; compare_index++)
        {
            result = compare_students(
                sorted + compare_index,
                sorted + compare_index + 1,
                sort_type);

            if ((order == ORDER_ASCENDING && result > 0) ||
                (order == ORDER_DESCENDING && result < 0))
            {
                temporary = sorted[compare_index];
                sorted[compare_index] = sorted[compare_index + 1];
                sorted[compare_index + 1] = temporary;
            }
        }
    }

    printf("\n정렬 결과\n");
    print_all_students(sorted, count);
}

// 학생 찾기 결과에 해당하는 학생 이름을 출력한다.
void print_search_result(const Student* students, const int* matches, int match_count)
{
    int index;

    if (match_count == 0)
    {
        printf("조건에 맞는 학생이 없습니다.\n");
        return;
    }

    printf("조건에 맞는 학생 이름\n");

    for (index = 0; index < match_count; index++)
    {
        printf("%d. %s\n", index + 1, students[matches[index]].name);
    }
}

// 번호, 주소의 각 항목 또는 성적으로 학생을 찾는다.
void search_students(const Student* students, int count)
{
    int search_type;
    int matches[MAX_STUDENTS];
    int match_count = 0;
    int index;
    int number = 0;
    double grade = 0.0;
    char text[ADDRESS_LENGTH];

    if (count == 0)
    {
        printf("출석부에 학생이 없습니다.\n");
        return;
    }

    printf("\n찾기 기준을 선택하세요.\n");
    printf("1. 번호\n");
    printf("2. 주소 - 나라\n");
    printf("3. 주소 - 도/특별시/광역시\n");
    printf("4. 주소 - 시\n");
    printf("5. 주소 - 구\n");
    printf("6. 성적\n");

    search_type = input_int_range("찾기 기준: ", 1, 6);

    if (search_type == 1)
    {
        number = input_int_range("찾을 번호: ", 1, INT_MAX);
    }
    else if (search_type == 6)
    {
        grade = input_double_range("찾을 성적(0~100): ", 0.0, 100.0);
    }
    else
    {
        input_text("찾을 주소 항목: ", text, ADDRESS_LENGTH);
    }

    for (index = 0; index < count; index++)
    {
        int matched = 0;

        switch (search_type)
        {
        case 1:
            matched = students[index].number == number;
            break;

        case 2:
            matched = strcmp(students[index].address.country, text) == 0;
            break;

        case 3:
            matched = strcmp(students[index].address.province, text) == 0;
            break;

        case 4:
            matched = strcmp(students[index].address.city, text) == 0;
            break;

        case 5:
            matched = strcmp(students[index].address.district, text) == 0;
            break;

        case 6:
            matched = is_same_grade(students[index].grade, grade);
            break;
        }

        if (matched)
        {
            matches[match_count] = index;
            match_count++;
        }
    }

    print_search_result(students, matches, match_count);
}

// 출석부를 텍스트 파일에 저장한다.
void save_students(const Student* students, int count)
{
    char file_name[FILE_NAME_LENGTH];
    FILE* file;
    int index;

    while (1)
    {
        if (!read_line("저장할 파일 이름: ", file_name, sizeof(file_name)))
        {
            continue;
        }

        trim_spaces(file_name);

        if (is_blank(file_name))
        {
            printf("파일 이름을 입력해주세요.\n");
            continue;
        }

        break;
    }

    file = fopen(file_name, "w");

    if (file == NULL)
    {
        printf("파일을 열 수 없습니다.\n");
        return;
    }

    for (index = 0; index < count; index++)
    {
        const Student* student = students + index;

        fprintf(file, "%d|%s|%s|%s|%s|%s|%.17g\n",
            student->number,
            student->name,
            student->address.country,
            student->address.province,
            student->address.city,
            student->address.district,
            student->grade);
    }

    if (fclose(file) != 0)
    {
        printf("파일을 닫는 중 오류가 발생했습니다.\n");
        return;
    }

    printf("%d명의 학생 정보를 저장했습니다.\n", count);
}

// 파일에서 읽은 학생 정보가 올바른지 확인한다.
int is_valid_loaded_student(Student* student)
{
    trim_spaces(student->name);
    trim_spaces(student->address.country);
    trim_spaces(student->address.province);
    trim_spaces(student->address.city);
    trim_spaces(student->address.district);

    return student->number > 0 &&
        is_valid_text(student->name) &&
        is_valid_text(student->address.country) &&
        is_valid_text(student->address.province) &&
        is_valid_text(student->address.city) &&
        is_valid_text(student->address.district) &&
        student->grade >= 0.0 &&
        student->grade <= 100.0 &&
        student->grade == student->grade;
}

// 텍스트 한 줄을 Student 구조체로 변환한다.
int parse_student_line(const char* line, Student* student)
{
    char extra;
    int result;

    result = sscanf(line,
        "%d|%49[^|]|%49[^|]|%49[^|]|%49[^|]|%49[^|]|%lf%c",
        &student->number,
        student->name,
        student->address.country,
        student->address.province,
        student->address.city,
        student->address.district,
        &student->grade,
        &extra);

    if (result != 7)
    {
        return 0;
    }

    return is_valid_loaded_student(student);
}

// 파일에서 출석부를 불러와 현재 출석부를 교체한다.
void load_students(Student* students, int* count)
{
    char file_name[FILE_NAME_LENGTH];
    char line[LINE_LENGTH];
    Student loaded[MAX_STUDENTS];
    Student student;
    FILE* file;
    int loaded_count = 0;
    int invalid_count = 0;
    int index;
    int has_newline;

    while (1)
    {
        if (!read_line("불러올 파일 이름: ", file_name, sizeof(file_name)))
        {
            continue;
        }

        trim_spaces(file_name);

        if (is_blank(file_name))
        {
            printf("파일 이름을 입력해주세요.\n");
            continue;
        }

        break;
    }

    file = fopen(file_name, "r");

    if (file == NULL)
    {
        printf("파일을 열 수 없습니다.\n");
        return;
    }

    while (fgets(line, sizeof(line), file) != NULL)
    {
        has_newline = strchr(line, '\n') != NULL;

        if (!has_newline && !feof(file))
        {
            clearerr(file);

            while (fgetc(file) != '\n' && !feof(file))
            {
            }

            invalid_count++;
            continue;
        }

        line[strcspn(line, "\r\n")] = '\0';

        if (is_blank(line))
        {
            continue;
        }

        if (loaded_count >= MAX_STUDENTS)
        {
            invalid_count++;
            continue;
        }

        if (parse_student_line(line, &student))
        {
            loaded[loaded_count] = student;
            loaded_count++;
        }
        else
        {
            invalid_count++;
        }
    }

    if (ferror(file))
    {
        printf("파일을 읽는 중 오류가 발생했습니다.\n");
        fclose(file);
        return;
    }

    fclose(file);

    for (index = 0; index < loaded_count; index++)
    {
        students[index] = loaded[index];
    }

    *count = loaded_count;

    printf("%d명의 학생 정보를 불러왔습니다.\n", loaded_count);

    if (invalid_count > 0)
    {
        printf("형식이 잘못되거나 최대 인원을 초과한 %d개 항목은 제외했습니다.\n",
            invalid_count);
    }
}

// 프로그램의 기능 목록을 출력한다.
void print_menu(void)
{
    printf("\n========== 출석부 프로그램 ==========\n");
    printf("1. 출석부 전체 출력\n");
    printf("2. 학생 추가\n");
    printf("3. 학생 삭제\n");
    printf("4. 학생 정렬 출력\n");
    printf("5. 학생 찾기\n");
    printf("6. 출석부 파일 저장\n");
    printf("7. 출석부 파일 불러오기\n");
    printf("0. 종료\n");
    printf("=====================================\n");
}

int main(void)
{
    Student students[MAX_STUDENTS];
    int count = 0;
    int menu;

    printf("최대 %d명의 학생을 저장할 수 있습니다.\n", MAX_STUDENTS);

    while (1)
    {
        print_menu();
        menu = input_int_range("메뉴 선택: ", 0, 7);

        switch (menu)
        {
        case 1:
            print_all_students(students, count);
            break;

        case 2:
            add_student(students, &count);
            break;

        case 3:
            delete_student(students, &count);
            break;

        case 4:
            sort_students(students, count);
            break;

        case 5:
            search_students(students, count);
            break;

        case 6:
            save_students(students, count);
            break;

        case 7:
            load_students(students, &count);
            break;

        case 0:
            printf("프로그램을 종료합니다.\n");
            return 0;
        }
    }
}
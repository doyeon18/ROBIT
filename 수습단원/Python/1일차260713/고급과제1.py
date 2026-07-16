# 연속된 문자를 문자와 반복 횟수 형태로 압축하는 함수
def compress(text):
    if text == "":
        return ""

    compressed = ""

    # 첫 번째 문자를 기준으로 시작
    current_char = text[0]
    count = 1

    # 두 번째 문자부터 앞의 문자와 비교
    for i in range(1, len(text)):
        if text[i] == current_char:
            count = count + 1

        else:
            # 지금까지 센 문자와 반복 횟수를 저장
            compressed = compressed + current_char + str(count)

            # 새로운 문자부터 다시 세기 시작
            current_char = text[i]
            count = 1

    # 반복문에서 저장되지 않은 마지막 문자 묶음 추가
    compressed = compressed + current_char + str(count)

    # 압축 결과가 원본보다 길면 원본 반환
    if len(compressed) > len(text):
        return text
    else:
        return compressed


# 압축 문자열의 형식이 올바른지 검사하는 함수
def is_valid_code(code):
    if code == "":
        return False

    i = 0

    while i < len(code):
        # 먼저 알파벳이 나와야 함
        if not code[i].isalpha():
            return False

        i = i + 1

        # 알파벳 뒤에는 반드시 숫자가 있어야 함
        if i >= len(code) or not code[i].isdigit():
            return False

        number = ""

        # 반복 횟수가 두 자리 이상일 수 있으므로 숫자가 이어지는 동안 모두 저장한다.
        while i < len(code) and code[i].isdigit():
            number = number + code[i]
            i = i + 1

        # 반복 횟수는 1 이상이어야 한다.
        if int(number) < 1:
            return False

    return True


# 압축 문자열을 원래 문자열로 복원하는 함수
def decompress(code):
    # 잘못된 압축 문자열이면 ERROR 반환
    if not is_valid_code(code):
        return "ERROR"

    result = ""
    i = 0

    while i < len(code):
        # 반복할 문자
        char = code[i]
        i = i + 1

        # 반복 횟수
        number = ""

        while i < len(code) and code[i].isdigit():
            number = number + code[i]
            i = i + 1

        count = int(number)

        # 문자를 count번 반복
        result = result + char * count

    return result


# 프로그램 실행 함수
def main():
    print(compress("aaabbccccd"))
    print(decompress("a3b2c4d1"))
    print(decompress("a12b3"))
    print(decompress("a0"))
    print(decompress("3a"))
    print(compress("abcde"))


main()
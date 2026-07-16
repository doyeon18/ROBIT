# 리스트 맨 뒤에 값을 추가하는 함수
def append_item(items, value):
    items.append(value)


# 리스트의 특정 위치에 값을 추가하는 함수
def insert_item(items, index, value):
    # insert는 리스트의 맨 뒤에도 추가할 수 있으므로
    # index가 len(items)인 경우도 허용한다.
    if 0 <= index <= len(items):
        items.insert(index, value)
    else:
        print("인덱스 범위를 벗어났습니다.")


# 리스트에서 특정 값을 삭제하는 함수
def remove_item(items, value):
    # 존재하지 않는 값을 remove하면 ValueError가 발생하므로
    # 값이 리스트에 존재하는지 먼저 확인한다.
    if value in items:
        items.remove(value)
    else:
        print(f"'{value}'은(는) 리스트에 존재하지 않습니다.")


# 리스트의 특정 위치에 있는 값을 삭제하는 함수
def pop_item(items, index):
    # 빈 리스트에서 pop을 실행하면 IndexError가 발생한다.
    if len(items) == 0:
        print("리스트가 비어 있어 pop을 실행할 수 없습니다.")

    # pop에서 유효한 인덱스는 0부터 len(items) - 1까지이다.
    elif 0 <= index < len(items):
        removed_value = items.pop(index)
        print(f"삭제된 값: {removed_value}")

    else:
        print("인덱스 범위를 벗어났습니다.")


# 사용자가 입력한 명령어를 분석하고 실행하는 함수
def process_command(items, command_line):
    parts = command_line.split()

    # 아무것도 입력하지 않았을 때
    if len(parts) == 0:
        print("명령어를 입력해주세요.")
        return True

    command = parts[0]

    # append 값
    if command == "append":
        if len(parts) == 2:
            append_item(items, parts[1])
        else:
            print("사용법: append 값")

    # insert 인덱스 값
    elif command == "insert":
        if len(parts) == 3:
            try:
                index = int(parts[1])
                value = parts[2]
                insert_item(items, index, value)

            except ValueError:
                print("인덱스에는 정수를 입력해야 합니다.")
        else:
            print("사용법: insert 인덱스 값")

    # remove 값
    elif command == "remove":
        if len(parts) == 2:
            remove_item(items, parts[1])
        else:
            print("사용법: remove 값")

    # pop 인덱스
    elif command == "pop":
        if len(parts) == 2:
            try:
                index = int(parts[1])
                pop_item(items, index)

            except ValueError:
                print("인덱스에는 정수를 입력해야 합니다.")

        # 인덱스를 생략하면 마지막 값을 삭제한다.
        elif len(parts) == 1:
            if len(items) == 0:
                print("리스트가 비어 있어 pop을 실행할 수 없습니다.")
            else:
                removed_value = items.pop()
                print(f"삭제된 값: {removed_value}")

        else:
            print("사용법: pop 또는 pop 인덱스")

    # 리스트 길이 출력
    elif command == "len":
        if len(parts) == 1:
            print(len(items))
        else:
            print("사용법: len")

    # 현재 리스트 출력
    elif command == "print":
        if len(parts) == 1:
            print(items)
        else:
            print("사용법: print")

    # 리스트의 모든 값 삭제
    elif command == "clear":
        if len(parts) == 1:
            items.clear()
        else:
            print("사용법: clear")

    # 프로그램 종료
    elif command == "exit":
        return False

    # 지원하지 않는 명령어
    else:
        print("지원하지 않는 명령어입니다.")

    return True


# 프로그램 전체 실행을 담당하는 함수
def main():
    items = []

    print("리스트 조작 프로그램")
    print("종료하려면 exit를 입력하세요.")

    while True:
        command_line = input("> ")

        continue_program = process_command(items, command_line)

        if continue_program == False:
            break


main()
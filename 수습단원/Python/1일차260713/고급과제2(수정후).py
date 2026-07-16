# 책을 등록하거나 기존 책의 수량을 추가하는 함수
def add_book(books, name, count):
    if name in books:
        books[name] += count
    else:
        books[name] = count


# 책을 대출하는 함수
def borrow_book(books, users, user, book):
    # 책이 등록되어 있지 않은 경우
    if book not in books:
        print("ERROR: no such book")
        return

    # 남은 책이 없는 경우
    if books[book] <= 0:
        print("ERROR: no stock")
        return

    # 같은 사용자가 같은 책을 이미 대출한 경우
    if user in users and book in users[user]:
        print("ERROR: already borrowed")
        return

    # 책의 남은 수량 감소
    books[book] -= 1

    # 처음 대출하는 사용자라면 빈 대출 목록 생성 set 대신 list를 사용하여 대출 순서를 유지한다.
    if user not in users:
        users[user] = []

    users[user].append(book)


# 책을 반납하는 함수
def return_book(books, users, user, book):
    # 실제로 대출 중인 책이 아닌 경우
    if user not in users or book not in users[user]:
        print("ERROR: not borrowed")
        return

    users[user].remove(book)
    books[book] += 1


# 특정 책의 남은 수량을 출력하는 함수
def status_book(books, book):
    if book not in books:
        print("ERROR")
    else:
        print(books[book])


# 사용자의 대출 목록을 출력하는 함수
def user_info(users, user):
    if user not in users or len(users[user]) == 0:
        print("EMPTY")
    else:
        print(" ".join(users[user]))


# 전체 책을 등록 순서대로 출력하는 함수
def list_books(books):
    for book in books:
        print(book)


# 입력된 명령어를 분석하고 실행하는 함수
def process_command(books, users, cmd):
    # 아무것도 입력하지 않은 경우
    if len(cmd) == 0:
        print("ERROR")
        return True

    command = cmd[0]

    # add 책이름 수량
    if command == "add":
        if len(cmd) != 3 or not cmd[2].isdigit():
            print("ERROR")
            return True

        name = cmd[1]
        count = int(cmd[2])

        add_book(books, name, count)

    # borrow 사용자 책이름
    elif command == "borrow":
        if len(cmd) != 3:
            print("ERROR")
            return True

        user = cmd[1]
        book = cmd[2]

        borrow_book(books, users, user, book)

    # return 사용자 책이름
    elif command == "return":
        if len(cmd) != 3:
            print("ERROR")
            return True

        user = cmd[1]
        book = cmd[2]

        return_book(books, users, user, book)

    # status 책이름
    elif command == "status":
        if len(cmd) != 2:
            print("ERROR")
            return True

        status_book(books, cmd[1])

    # user 사용자
    elif command == "user":
        if len(cmd) != 2:
            print("ERROR")
            return True

        user_info(users, cmd[1])

    # list
    elif command == "list":
        if len(cmd) != 1:
            print("ERROR")
            return True

        list_books(books)

    # Exit
    elif command == "Exit":
        if len(cmd) != 1:
            print("ERROR")
            return True

        return False

    # 존재하지 않는 명령어
    else:
        print("ERROR")

    return True


# 프로그램 전체 실행 함수
def main():
    # 책 이름을 key, 남은 수량을 value로 저장
    books = {}

    # 사용자 이름을 key, 대출한 책 목록을 value로 저장
    users = {}

    while True:
        try:
            cmd = input().split()

        # 입력이 종료된 경우에만 프로그램 종료
        except EOFError:
            break

        continue_program = process_command(books, users, cmd)

        if continue_program == False:
            break


main()

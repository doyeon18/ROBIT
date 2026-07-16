def add_book(books, name, count):
    if name in books:
        books[name] += count
    else:
        books[name] = count


def borrow_book(books, users, user, book):
    if book not in books:
        print("ERROR: no such book")
        return
    
    if books[book] <= 0:
        print("ERROR: no stock")
        return
    
    if user in users and book in users[user]:
        print("ERROR: already borrowed")
        return
    
    books[book] -= 1
    
    if user not in users:
        users[user] = set()
    
    users[user].add(book)


def return_book(books, users, user, book):
    if user not in users or book not in users[user]:
        print("ERROR: not borrowed")
        return
    
    users[user].remove(book)
    books[book] += 1


def status_book(books, book):
    if book not in books:
        print("ERROR")
    else:
        print(books[book])


def user_info(users, user):
    if user not in users or len(users[user]) == 0:
        print("EMPTY")
    else:
        print(" ".join(users[user]))


def list_books(books):
    for book in books:
        print(book)


def process():
    books = {}
    users = {}

    while True:
        try:
            cmd = input().split()
        except:
            break
        
        if len(cmd) == 0:
            print("ERROR")
            continue

        if cmd[0] == "add":
            if len(cmd) != 3 or not cmd[2].isdigit():
                print("ERROR")
                continue
            add_book(books, cmd[1], int(cmd[2]))

        elif cmd[0] == "borrow":
            if len(cmd) != 3:
                print("ERROR")
                continue
            borrow_book(books, users, cmd[1], cmd[2])

        elif cmd[0] == "return":
            if len(cmd) != 3:
                print("ERROR")
                continue
            return_book(books, users, cmd[1], cmd[2])

        elif cmd[0] == "status":
            if len(cmd) != 2:
                print("ERROR")
                continue
            status_book(books, cmd[1])

        elif cmd[0] == "user":
            if len(cmd) != 2:
                print("ERROR")
                continue
            user_info(users, cmd[1])

        elif cmd[0] == "list":
            if len(cmd) != 1:
                print("ERROR")
                continue
            list_books(books)

        elif cmd[0] == "Exit":
            if len(cmd) != 1:
                print("ERROR")
                continue
            break

        else:
            print("ERROR")


process()
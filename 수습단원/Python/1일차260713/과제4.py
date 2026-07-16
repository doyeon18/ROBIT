# 10 × 10 미로
maze = [
    [1, 1, 1, 1, 1, 1, 1, 1, 1, 1],
    [1, 0, 0, 1, 0, 0, 0, 0, 0, 1],
    [1, 0, 0, 1, 1, 1, 0, 0, 0, 1],
    [1, 0, 0, 0, 0, 0, 0, 1, 0, 1],
    [1, 0, 0, 0, 0, 0, 0, 1, 0, 1],
    [1, 0, 0, 0, 0, 1, 0, 1, 0, 1],
    [1, 0, 0, 0, 0, 1, 2, 1, 0, 1],
    [1, 0, 0, 0, 0, 1, 0, 0, 0, 1],
    [1, 0, 0, 0, 0, 0, 0, 0, 0, 1],
    [1, 1, 1, 1, 1, 1, 1, 1, 1, 1]
]


# 해당 위치로 이동할 수 있는지 확인하는 함수
def can_move(maze, row, column):
    if maze[row][column] == 0 or maze[row][column] == 2:
        return True
    else:
        return False


# 개미를 이동시키는 함수
def move_ant(maze):
    # 문제의 좌표 (2, 2)는 리스트 인덱스로 (1, 1)이다.
    row = 1
    column = 1

    while True:
        # 먹이를 찾은 경우 9로 표시한 후 종료한다.
        if maze[row][column] == 2:
            maze[row][column] = 9
            break

        # 개미가 이동한 현재 위치를 9로 표시한다.
        maze[row][column] = 9

        # 오른쪽으로 이동할 수 있으면 오른쪽으로 이동한다.
        if can_move(maze, row, column + 1):
            column = column + 1

        # 오른쪽이 막혀 있으면 아래쪽을 확인한다.
        elif can_move(maze, row + 1, column):
            row = row + 1

        # 오른쪽과 아래쪽이 모두 막혀 있으면 종료한다.
        else:
            break


# 완성된 미로를 출력하는 함수
def print_maze(maze):
    for row in maze:
        print(*row)


# 프로그램 실행을 담당하는 함수
def main():
    move_ant(maze)
    print_maze(maze)


main()
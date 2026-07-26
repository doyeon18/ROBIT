/*
 * 메뉴 방식의 다중 배송지 물류 로봇 경로 탐색 프로그램이다.
 *
 * 프로그램 시작 시 맵을 한 번만 생성하고, 프로그램이 종료될 때까지 유지한다.
 * 따라서 메뉴와 편집 화면을 오가더라도 장애물, 출발점, 배송지 상태가 사라지지 않는다.
 *
 * 주요 기능
 * 1. 마우스로 메뉴 버튼 선택
 * 2. 네모 격자 지도에서 마우스 클릭과 드래그로 장애물 설치·제거
 * 3. 빈 칸은 공백, 장애물은 채워진 블록으로 표시
 * 4. 마우스로 출발점 1개와 배송지 여러 개 설정
 * 5. A* 알고리즘으로 지점 사이의 실제 최단 비용 계산
 * 6. 완전탐색 TSP로 배송지의 정확한 최적 방문 순서 계산
 * 7. 최적 방문 순서대로 A* 경로를 연결해 전체 배송 경로 출력
 *
 * Windows 콘솔 마우스 입력 API를 사용하므로 Windows 전용 코드다.
 */

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdint.h>
#include <windows.h>

#define INF 1000000000

 /*
  * 완전탐색 TSP의 경우의 수가 너무 커지는 것을 막기 위해
  * 배송지는 최대 10개까지만 설정할 수 있도록 제한한다.
  */
#define MAX_DESTINATIONS 10
#define MAX_POINTS (MAX_DESTINATIONS + 1)

  /*
   * 격자 지도에서 한 칸이 차지하는 콘솔 크기다.
   *
   * 각 칸의 내부 너비는 3글자이고,
   * 오른쪽 구분선까지 포함하면 가로 간격은 4칸이다.
   * 한 행은 내용 줄 1개와 가로 경계선 1개로 구성되므로
   * 세로 간격은 2줄이다.
   */
#define MAP_CELL_WIDTH 4
#define MAP_CELL_HEIGHT 2
#define MAP_START_X 5

#define PATH_ERROR -1
#define PATH_NOT_FOUND 0
#define PATH_FOUND 1

   /*
    * 전체 배송 경로 계산 결과다.
    */
#define ROUTE_SUCCESS 1
#define ROUTE_MEMORY_ERROR -1
#define ROUTE_MISSING_START -2
#define ROUTE_MISSING_DESTINATION -3
#define ROUTE_UNREACHABLE -4
#define ROUTE_ORDER_ERROR -5
#define ROUTE_PATH_ERROR -6

    /*
     * 현재 어떤 화면을 표시할지 나타낸다.
     */
typedef enum
{
    SCREEN_MENU,
    SCREEN_OBSTACLE_EDITOR,
    SCREEN_LOCATION_EDITOR,
    SCREEN_CALCULATE,
    SCREEN_RESET,
    SCREEN_EXIT
} ScreenMode;

/*
 * 출발점·배송지 편집 화면에서 현재 선택한 편집 모드다.
 */
typedef enum
{
    LOCATION_MODE_START,
    LOCATION_MODE_DESTINATION
} LocationEditMode;

/*
 * 콘솔에서 발생한 입력 이벤트 종류다.
 */
typedef enum
{
    CONSOLE_EVENT_KEY,
    CONSOLE_EVENT_MOUSE
} ConsoleEventType;

typedef struct
{
    int row;
    int col;
} Position;

typedef struct
{
    int isObstacle;
    int height;
    int isPath;
} Map;

typedef struct
{
    int gCost;
    int hCost;
    int fCost;
    int isOpen;
    int isClosed;
    Position parent;
} AStarNode;

typedef struct
{
    Position position;
    int fCost;
    int hCost;
} QueueItem;

typedef struct
{
    QueueItem* items;
    int size;
    int capacity;
} PriorityQueue;

/*
 * 콘솔 화면에 표시할 클릭 가능한 버튼 영역이다.
 */
typedef struct
{
    int left;
    int top;
    int right;
    int bottom;
    const char* text;
} ConsoleButton;

/*
 * ReadConsoleInput()으로 읽은 키보드 또는 마우스 이벤트를
 * 프로그램에서 사용하기 쉬운 형태로 저장한다.
 */
typedef struct
{
    ConsoleEventType type;
    WORD virtualKeyCode;
    int mouseX;
    int mouseY;
    int leftPressed;
    DWORD mouseFlags;
} ConsoleInputEvent;

void runSimulation(void);
void inputMapSize(int* rows, int* cols);
Map** createMap(int rows, int cols);
AStarNode** createNodeMap(int rows, int cols);
void initializeMap(Map** map, int rows, int cols);
void initializeHeights(Map** map, int rows, int cols);
void resetMap(Map** map, int rows, int cols, Position* start, int* destinationCount, int* routeValid);
void clearPathMarks(Map** map, int rows, int cols);
void invalidateRoute(Map** map, int rows, int cols, int* routeValid);

void clearConsoleScreen(void);
void setConsoleCursorPosition(int x, int y);
int getConsoleCursorY(void);
int beginMouseInput(HANDLE* inputHandle, DWORD* originalMode);
void endMouseInput(HANDLE inputHandle, DWORD originalMode);
int readConsoleInputEvent(HANDLE inputHandle, ConsoleInputEvent* eventData);

void drawButton(ConsoleButton button);
int isPointInsideButton(int x, int y, ConsoleButton button);
int convertMouseToMapPosition(int mouseX, int mouseY, int mapStartY, int rows, int cols, Position* mapPosition);

ScreenMode showMainMenu(
    Map** map,
    int rows,
    int cols,
    Position start,
    Position* destinations,
    int destinationCount,
    int* visitOrder,
    int routeValid
);

int editObstaclesWithMouse(
    Map** map,
    int rows,
    int cols,
    Position start,
    Position* destinations,
    int destinationCount,
    int* routeValid
);

int editLocationsWithMouse(
    Map** map,
    int rows,
    int cols,
    Position* start,
    Position* destinations,
    int* destinationCount,
    int* routeValid
);

int isProtectedPosition(
    Position position,
    Position start,
    Position* destinations,
    int destinationCount
);

int countObstacles(Map** map, int rows, int cols);
void drawMapCell(
    Map** map,
    int row,
    int col,
    int mapStartY,
    Position start,
    Position* destinations,
    int destinationCount,
    int* visitOrder
);

int calculateDeliveryRoute(
    Map** map,
    AStarNode** nodeMap,
    int rows,
    int cols,
    Position start,
    Position* destinations,
    int destinationCount,
    int* visitOrder,
    int* segmentCosts,
    long long* optimalCost,
    long long* totalCost,
    Position** totalPath,
    size_t* totalPathLength
);

void showResultScreen(
    int routeResult,
    Map** map,
    int rows,
    int cols,
    Position start,
    Position* destinations,
    int destinationCount,
    int* visitOrder,
    int* segmentCosts,
    long long optimalCost,
    long long totalCost,
    Position* totalPath,
    size_t totalPathLength
);

int findPositionIndex(Position* positions, int count, Position target);
int getDestinationVisitNumber(int row, int col, Position* destinations, int destinationCount, int* visitOrder);
void printMap(Map** map, int rows, int cols, Position start, Position* destinations, int destinationCount, int* visitOrder);
int printInteractiveMap(Map** map, int rows, int cols, Position start, Position* destinations, int destinationCount, int* visitOrder, int mapTopY);
int isInsideMap(int row, int col, int rows, int cols);
int isWalkable(Map** map, int row, int col);
void initializeAStarNodes(AStarNode** nodeMap, int rows, int cols);
int calculateHeuristic(Position current, Position destination);
int calculateMoveCost(Map** map, Position current, Position next);
PriorityQueue* initializePriorityQueue(int initialCapacity);
int hasHigherPriority(QueueItem first, QueueItem second);
void swapQueueItem(QueueItem* first, QueueItem* second);
int pushPriorityQueue(PriorityQueue* queue, QueueItem item);
QueueItem popPriorityQueue(PriorityQueue* queue);
int findPath(Map** map, AStarNode** nodeMap, int rows, int cols, Position start, Position destination, Position* path, int* pathLength, PriorityQueue** createdQueue);
void reconstructPath(AStarNode** nodeMap, Position start, Position destination, Position* path, int* pathLength);
int buildCostMatrix(Map** map, AStarNode** nodeMap, int rows, int cols, Position start, Position* destinations, int destinationCount, int costMatrix[MAX_POINTS][MAX_POINTS]);
void searchOptimalOrder(int costMatrix[MAX_POINTS][MAX_POINTS], int destinationCount, int depth, int currentPointIndex, long long currentCost, int* used, int* currentOrder, int* bestOrder, long long* bestCost);
int determineOptimalVisitOrder(int costMatrix[MAX_POINTS][MAX_POINTS], int destinationCount, int* bestOrder, long long* bestCost);
int buildCompleteRoute(Map** map, AStarNode** nodeMap, int rows, int cols, Position start, Position* destinations, int destinationCount, int* visitOrder, Position* totalPath, size_t totalPathCapacity, size_t* totalPathLength, int* segmentCosts, long long* totalCost);
void printDeliveryResult(Position start, Position* destinations, int destinationCount, int* visitOrder, int* segmentCosts, long long totalCost, Position* totalPath, size_t totalPathLength);
void freePriorityQueue(PriorityQueue* queue);
void freeMap(Map** map, int rows);
void freeNodeMap(AStarNode** nodeMap, int rows);

int main(void)
{
    /*
     * 한글 출력이 가능한 UTF-8 코드 페이지를 사용한다.
     */
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    runSimulation();

    return 0;
}

/*
 * 맵 생성 후 화면 상태를 반복해서 전환하고,
 * 프로그램 종료 시점에만 동적 메모리를 해제한다.
 */
void runSimulation(void)
{
    Map** map = NULL;
    AStarNode** nodeMap = NULL;

    Position start;
    Position destinations[MAX_DESTINATIONS];
    Position* totalPath = NULL;

    int visitOrder[MAX_DESTINATIONS];
    int segmentCosts[MAX_DESTINATIONS];

    int rows;
    int cols;
    int destinationCount = 0;
    int routeValid = 0;
    int routeResult;

    long long optimalCost = 0;
    long long totalCost = 0;
    size_t totalPathLength = 0;

    ScreenMode screenMode;

    start.row = -1;
    start.col = -1;

    printf("===== 다중 배송지 물류 로봇 경로 탐색 =====\n");

    inputMapSize(
        &rows,
        &cols
    );

    map = createMap(
        rows,
        cols
    );

    nodeMap = createNodeMap(
        rows,
        cols
    );

    if (
        map == NULL ||
        nodeMap == NULL
        )
    {
        printf("메모리 할당에 실패했습니다.\n");

        freeMap(map, rows);
        freeNodeMap(nodeMap, rows);

        return;
    }

    initializeMap(
        map,
        rows,
        cols
    );

    screenMode = SCREEN_MENU;

    /*
     * 이 부분이 구조도에 표시한 [화면 전환 반복문]이다.
     * 별도의 함수가 아니라, 사용자가 종료 버튼을 누를 때까지
     * 메뉴와 여러 편집 화면을 계속 오가는 while 반복문이다.
     */
    while (screenMode != SCREEN_EXIT)
    {
        if (screenMode == SCREEN_MENU)
        {
            screenMode = showMainMenu(
                map,
                rows,
                cols,
                start,
                destinations,
                destinationCount,
                visitOrder,
                routeValid
            );
        }
        else if (screenMode == SCREEN_OBSTACLE_EDITOR)
        {
            editObstaclesWithMouse(
                map,
                rows,
                cols,
                start,
                destinations,
                destinationCount,
                &routeValid
            );

            screenMode = SCREEN_MENU;
        }
        else if (screenMode == SCREEN_LOCATION_EDITOR)
        {
            editLocationsWithMouse(
                map,
                rows,
                cols,
                &start,
                destinations,
                &destinationCount,
                &routeValid
            );

            screenMode = SCREEN_MENU;
        }
        else if (screenMode == SCREEN_CALCULATE)
        {
            free(totalPath);
            totalPath = NULL;
            totalPathLength = 0;

            routeResult = calculateDeliveryRoute(
                map,
                nodeMap,
                rows,
                cols,
                start,
                destinations,
                destinationCount,
                visitOrder,
                segmentCosts,
                &optimalCost,
                &totalCost,
                &totalPath,
                &totalPathLength
            );

            if (routeResult == ROUTE_SUCCESS)
            {
                routeValid = 1;
            }
            else
            {
                routeValid = 0;
            }

            showResultScreen(
                routeResult,
                map,
                rows,
                cols,
                start,
                destinations,
                destinationCount,
                visitOrder,
                segmentCosts,
                optimalCost,
                totalCost,
                totalPath,
                totalPathLength
            );

            free(totalPath);
            totalPath = NULL;

            screenMode = SCREEN_MENU;
        }
        else if (screenMode == SCREEN_RESET)
        {
            resetMap(
                map,
                rows,
                cols,
                &start,
                &destinationCount,
                &routeValid
            );

            screenMode = SCREEN_MENU;
        }
    }

    /*
     * 이 부분이 구조도에 표시한 [프로그램 종료 및 메모리 정리]다.
     * 별도의 함수 이름이 아니라, 화면 반복문이 끝난 뒤
     * 프로그램이 사용한 동적 메모리를 정리하는 과정이다.
     */
    free(totalPath);
    freeMap(map, rows);
    freeNodeMap(nodeMap, rows);
}

/*
 * 장애물, 경로, 출발점과 배송지 설정을 모두 초기 상태로 되돌린다.
 */
void resetMap(
    Map** map,
    int rows,
    int cols,
    Position* start,
    int* destinationCount,
    int* routeValid
)
{
    initializeMap(
        map,
        rows,
        cols
    );

    start->row = -1;
    start->col = -1;

    *destinationCount = 0;
    *routeValid = 0;
}

/*
 * 이전에 계산된 전체 경로의 별표 표시만 제거한다.
 */
void clearPathMarks(
    Map** map,
    int rows,
    int cols
)
{
    int row;
    int col;

    for (row = 0; row < rows; row++)
    {
        for (col = 0; col < cols; col++)
        {
            map[row][col].isPath = 0;
        }
    }
}

/*
 * 장애물이나 위치 설정이 바뀌면 기존 경로는 더 이상 유효하지 않다.
 * 기존 별표를 지우고 routeValid를 0으로 변경한다.
 */
void invalidateRoute(
    Map** map,
    int rows,
    int cols,
    int* routeValid
)
{
    clearPathMarks(
        map,
        rows,
        cols
    );

    *routeValid = 0;
}

/*
 * 콘솔 커서를 원하는 문자 셀 좌표로 이동한다.
 */
void setConsoleCursorPosition(
    int x,
    int y
)
{
    HANDLE outputHandle;
    COORD position;

    outputHandle =
        GetStdHandle(STD_OUTPUT_HANDLE);

    if (outputHandle == INVALID_HANDLE_VALUE)
    {
        return;
    }

    position.X = (SHORT)x;
    position.Y = (SHORT)y;

    SetConsoleCursorPosition(
        outputHandle,
        position
    );
}

/*
 * 현재 콘솔 커서의 Y 좌표를 반환한다.
 */
int getConsoleCursorY(void)
{
    HANDLE outputHandle;
    CONSOLE_SCREEN_BUFFER_INFO screenInfo;

    outputHandle =
        GetStdHandle(STD_OUTPUT_HANDLE);

    if (outputHandle == INVALID_HANDLE_VALUE)
    {
        return 0;
    }

    if (
        GetConsoleScreenBufferInfo(
            outputHandle,
            &screenInfo
        ) == 0
        )
    {
        return 0;
    }

    return screenInfo.dwCursorPosition.Y;
}

/*
 * 키보드 키 누름 또는 마우스 상태 변화 이벤트 하나를 읽는다.
 * 장애물 드래그를 위해 마우스 이동 이벤트도 그대로 반환한다.
 */
int readConsoleInputEvent(
    HANDLE inputHandle,
    ConsoleInputEvent* eventData
)
{
    INPUT_RECORD inputRecord;
    DWORD eventCount;

    while (1)
    {
        if (
            ReadConsoleInput(
                inputHandle,
                &inputRecord,
                1,
                &eventCount
            ) == 0
            )
        {
            return 0;
        }

        if (
            inputRecord.EventType == KEY_EVENT &&
            inputRecord.Event.KeyEvent.bKeyDown == 1
            )
        {
            eventData->type = CONSOLE_EVENT_KEY;
            eventData->virtualKeyCode =
                inputRecord.Event.KeyEvent.wVirtualKeyCode;

            return 1;
        }

        if (inputRecord.EventType == MOUSE_EVENT)
        {
            eventData->type = CONSOLE_EVENT_MOUSE;
            eventData->mouseX =
                inputRecord.Event.MouseEvent.dwMousePosition.X;
            eventData->mouseY =
                inputRecord.Event.MouseEvent.dwMousePosition.Y;
            eventData->leftPressed =
                (
                    inputRecord.Event.MouseEvent.dwButtonState &
                    FROM_LEFT_1ST_BUTTON_PRESSED
                    ) != 0;
            eventData->mouseFlags =
                inputRecord.Event.MouseEvent.dwEventFlags;

            return 1;
        }
    }
}

/*
 * 고정된 사각형 영역 안에 콘솔 버튼 모양을 출력한다.
 */
void drawButton(ConsoleButton button)
{
    int x;

    setConsoleCursorPosition(
        button.left,
        button.top
    );

    putchar('+');

    for (x = button.left + 1; x < button.right; x++)
    {
        putchar('-');
    }

    putchar('+');

    setConsoleCursorPosition(
        button.left,
        button.top + 1
    );

    putchar('|');

    setConsoleCursorPosition(
        button.left + 2,
        button.top + 1
    );

    printf("%s", button.text);

    setConsoleCursorPosition(
        button.right,
        button.top + 1
    );

    putchar('|');

    setConsoleCursorPosition(
        button.left,
        button.bottom
    );

    putchar('+');

    for (x = button.left + 1; x < button.right; x++)
    {
        putchar('-');
    }

    putchar('+');
}

/*
 * 마우스 좌표가 버튼의 사각형 범위 안에 있는지 검사한다.
 */
int isPointInsideButton(
    int x,
    int y,
    ConsoleButton button
)
{
    if (
        x >= button.left &&
        x <= button.right &&
        y >= button.top &&
        y <= button.bottom
        )
    {
        return 1;
    }

    return 0;
}

/*
 * 콘솔의 마우스 좌표를 실제 맵의 행과 열 좌표로 변환한다.
 */
int convertMouseToMapPosition(
    int mouseX,
    int mouseY,
    int mapStartY,
    int rows,
    int cols,
    Position* mapPosition
)
{
    int relativeX;
    int relativeY;

    /*
     * mapStartY는 첫 번째 맵 칸의 내용이 출력된 줄이다.
     * 격자선 바깥을 클릭한 경우에는 맵 좌표로 처리하지 않는다.
     */
    if (
        mouseX < MAP_START_X ||
        mouseX >= MAP_START_X + cols * MAP_CELL_WIDTH ||
        mouseY < mapStartY ||
        mouseY >= mapStartY + rows * MAP_CELL_HEIGHT
        )
    {
        return 0;
    }

    relativeX =
        mouseX - MAP_START_X;

    relativeY =
        mouseY - mapStartY;

    /*
     * 각 칸 오른쪽의 세로선과 각 행 아래의 가로선은
     * 실제 맵 칸이 아니므로 클릭을 무시한다.
     */
    if (
        relativeX % MAP_CELL_WIDTH == MAP_CELL_WIDTH - 1 ||
        relativeY % MAP_CELL_HEIGHT != 0
        )
    {
        return 0;
    }

    mapPosition->row =
        relativeY / MAP_CELL_HEIGHT;

    mapPosition->col =
        relativeX / MAP_CELL_WIDTH;

    return isInsideMap(
        mapPosition->row,
        mapPosition->col,
        rows,
        cols
    );
}

/*
 * 메뉴 화면에 현재 지도를 항상 표시하고,
 * 사용자가 클릭한 버튼에 해당하는 다음 화면을 반환한다.
 */
ScreenMode showMainMenu(
    Map** map,
    int rows,
    int cols,
    Position start,
    Position* destinations,
    int destinationCount,
    int* visitOrder,
    int routeValid
)
{
    HANDLE inputHandle;
    DWORD originalMode;
    ConsoleInputEvent eventData;

    ConsoleButton obstacleButton;
    ConsoleButton locationButton;
    ConsoleButton calculateButton;
    ConsoleButton resetButton;
    ConsoleButton exitButton;

    int buttonLeft;
    int buttonTop;
    int previousLeftPressed = 0;

    clearConsoleScreen();

    /*
     * 지도는 항상 화면 맨 위에서 시작한다.
     * 지도 위에 여러 줄을 출력하지 않으므로 10×10에서도
     * 화면이 아래로 스크롤되지 않는다.
     */
    printInteractiveMap(
        map,
        rows,
        cols,
        start,
        destinations,
        destinationCount,
        routeValid == 1 ? visitOrder : NULL,
        0
    );

    /*
     * 오른쪽 패널은 10×10 지도와 80칸 콘솔 안에 들어가도록
     * 기존 버튼보다 폭을 줄였다.
     */
    buttonLeft =
        MAP_START_X + cols * MAP_CELL_WIDTH + 2;

    buttonTop = 0;

    obstacleButton.left = buttonLeft;
    obstacleButton.top = buttonTop;
    obstacleButton.right = buttonLeft + 28;
    obstacleButton.bottom = buttonTop + 2;
    obstacleButton.text = "장애물 설치";

    locationButton.left = buttonLeft;
    locationButton.top = buttonTop + 4;
    locationButton.right = buttonLeft + 28;
    locationButton.bottom = buttonTop + 6;
    locationButton.text = "출발점·배송지 설치";

    calculateButton.left = buttonLeft;
    calculateButton.top = buttonTop + 8;
    calculateButton.right = buttonLeft + 28;
    calculateButton.bottom = buttonTop + 10;
    calculateButton.text = "최적 경로 계산";

    resetButton.left = buttonLeft;
    resetButton.top = buttonTop + 12;
    resetButton.right = buttonLeft + 28;
    resetButton.bottom = buttonTop + 14;
    resetButton.text = "지도 전체 초기화";

    exitButton.left = buttonLeft;
    exitButton.top = buttonTop + 16;
    exitButton.right = buttonLeft + 28;
    exitButton.bottom = buttonTop + 18;
    exitButton.text = "프로그램 종료";

    drawButton(obstacleButton);
    drawButton(locationButton);
    drawButton(calculateButton);
    drawButton(resetButton);
    drawButton(exitButton);

    setConsoleCursorPosition(
        buttonLeft,
        buttonTop + 20
    );

    printf(
        "장애물:%d 배송지:%d/%d",
        countObstacles(map, rows, cols),
        destinationCount,
        MAX_DESTINATIONS
    );

    setConsoleCursorPosition(
        buttonLeft,
        buttonTop + 21
    );

    if (start.row >= 0)
    {
        printf(
            "출발점:(%d,%d)",
            start.row,
            start.col
        );
    }
    else
    {
        printf("출발점:미설정");
    }

    if (
        beginMouseInput(
            &inputHandle,
            &originalMode
        ) == 0
        )
    {
        return SCREEN_EXIT;
    }

    while (1)
    {
        if (
            readConsoleInputEvent(
                inputHandle,
                &eventData
            ) == 0
            )
        {
            endMouseInput(
                inputHandle,
                originalMode
            );

            return SCREEN_EXIT;
        }

        if (
            eventData.type == CONSOLE_EVENT_KEY &&
            eventData.virtualKeyCode == VK_ESCAPE
            )
        {
            endMouseInput(
                inputHandle,
                originalMode
            );

            return SCREEN_EXIT;
        }

        if (eventData.type != CONSOLE_EVENT_MOUSE)
        {
            continue;
        }

        if (
            eventData.leftPressed == 1 &&
            previousLeftPressed == 0
            )
        {
            if (
                isPointInsideButton(
                    eventData.mouseX,
                    eventData.mouseY,
                    obstacleButton
                ) == 1
                )
            {
                endMouseInput(inputHandle, originalMode);
                return SCREEN_OBSTACLE_EDITOR;
            }

            if (
                isPointInsideButton(
                    eventData.mouseX,
                    eventData.mouseY,
                    locationButton
                ) == 1
                )
            {
                endMouseInput(inputHandle, originalMode);
                return SCREEN_LOCATION_EDITOR;
            }

            if (
                isPointInsideButton(
                    eventData.mouseX,
                    eventData.mouseY,
                    calculateButton
                ) == 1
                )
            {
                endMouseInput(inputHandle, originalMode);
                return SCREEN_CALCULATE;
            }

            if (
                isPointInsideButton(
                    eventData.mouseX,
                    eventData.mouseY,
                    resetButton
                ) == 1
                )
            {
                endMouseInput(inputHandle, originalMode);
                return SCREEN_RESET;
            }

            if (
                isPointInsideButton(
                    eventData.mouseX,
                    eventData.mouseY,
                    exitButton
                ) == 1
                )
            {
                endMouseInput(inputHandle, originalMode);
                return SCREEN_EXIT;
            }
        }

        previousLeftPressed =
            eventData.leftPressed;
    }
}

/*
 * 현재 좌표가 출발점 또는 배송지인지 검사한다.
 * 보호되는 위치에는 장애물을 설치하지 않는다.
 */
int isProtectedPosition(
    Position position,
    Position start,
    Position* destinations,
    int destinationCount
)
{
    if (
        position.row == start.row &&
        position.col == start.col
        )
    {
        return 1;
    }

    if (
        findPositionIndex(
            destinations,
            destinationCount,
            position
        ) >= 0
        )
    {
        return 1;
    }

    return 0;
}

/*
 * 현재 맵에 설치된 장애물 개수를 센다.
 */
int countObstacles(
    Map** map,
    int rows,
    int cols
)
{
    int row;
    int col;
    int count = 0;

    for (row = 0; row < rows; row++)
    {
        for (col = 0; col < cols; col++)
        {
            if (map[row][col].isObstacle == 1)
            {
                count++;
            }
        }
    }

    return count;
}

/*
 * 전체 맵을 다시 출력하지 않고 변경된 맵 한 칸만 다시 그린다.
 * 장애물 드래그 중 화면 깜빡임을 줄이기 위해 사용한다.
 */
void drawMapCell(
    Map** map,
    int row,
    int col,
    int mapStartY,
    Position start,
    Position* destinations,
    int destinationCount,
    int* visitOrder
)
{
    int destinationNumber;

    /*
     * 격자의 테두리는 그대로 두고,
     * 선택한 칸 내부의 3글자만 다시 출력한다.
     */
    setConsoleCursorPosition(
        MAP_START_X + col * MAP_CELL_WIDTH,
        mapStartY + row * MAP_CELL_HEIGHT
    );

    destinationNumber =
        getDestinationVisitNumber(
            row,
            col,
            destinations,
            destinationCount,
            visitOrder
        );

    if (map[row][col].isObstacle == 1)
    {
        printf("███");
    }
    else if (
        row == start.row &&
        col == start.col
        )
    {
        printf(" S ");
    }
    else if (destinationNumber == 0)
    {
        printf(" D ");
    }
    else if (destinationNumber > 0)
    {
        printf("%3d", destinationNumber);
    }
    else if (map[row][col].isPath == 1)
    {
        printf(" * ");
    }
    else
    {
        printf("   ");
    }
}

/*
 * 장애물 편집 화면이다.
 * 빈 칸에서 드래그를 시작하면 지나가는 칸에 장애물을 설치하고,
 * 장애물에서 드래그를 시작하면 지나가는 장애물을 제거한다.
 */
int editObstaclesWithMouse(
    Map** map,
    int rows,
    int cols,
    Position start,
    Position* destinations,
    int destinationCount,
    int* routeValid
)
{
    HANDLE inputHandle;
    DWORD originalMode;
    ConsoleInputEvent eventData;

    ConsoleButton backButton;
    Position selectedPosition;

    int mapStartY = 0;
    int buttonLeft = 0;
    int buttonTop = 0;
    int obstacleCount;
    int maxObstacleCount;
    int previousLeftPressed = 0;
    int isDragging = 0;
    int dragValue = 0;
    int lastRow = -1;
    int lastCol = -1;
    int changed = 0;
    int needsRedraw = 1;
    int messageCode = 0;

    maxObstacleCount =
        rows * cols - 2;

    obstacleCount =
        countObstacles(map, rows, cols);

    if (
        beginMouseInput(
            &inputHandle,
            &originalMode
        ) == 0
        )
    {
        return 0;
    }

    while (1)
    {
        if (needsRedraw == 1)
        {
            clearConsoleScreen();

            /*
             * 지도는 Y=0에서 바로 출력한다.
             * printInteractiveMap이 실제 첫 번째 칸의 Y좌표를 반환하므로
             * 0번 행 클릭이 정확히 0번 행으로 변환된다.
             */
            mapStartY =
                printInteractiveMap(
                    map,
                    rows,
                    cols,
                    start,
                    destinations,
                    destinationCount,
                    NULL,
                    0
                );

            buttonLeft =
                MAP_START_X + cols * MAP_CELL_WIDTH + 2;

            buttonTop = 0;

            backButton.left = buttonLeft;
            backButton.top = buttonTop;
            backButton.right = buttonLeft + 28;
            backButton.bottom = buttonTop + 2;
            backButton.text = "메뉴로 돌아가기";

            drawButton(backButton);

            setConsoleCursorPosition(
                buttonLeft,
                buttonTop + 4
            );

            printf("===== 장애물 편집 =====");

            setConsoleCursorPosition(
                buttonLeft,
                buttonTop + 6
            );

            printf("드래그: 설치 또는 제거");

            setConsoleCursorPosition(
                buttonLeft,
                buttonTop + 8
            );

            printf(
                "장애물: %d / %d",
                obstacleCount,
                maxObstacleCount
            );

            setConsoleCursorPosition(
                buttonLeft,
                buttonTop + 10
            );

            printf("Enter/Esc: 메뉴");

            setConsoleCursorPosition(
                buttonLeft,
                buttonTop + 12
            );

            if (messageCode == 1)
            {
                printf("출발점·배송지 설치 불가");
            }
            else if (messageCode == 2)
            {
                printf("빈 칸 두 개가 필요함");
            }
            else
            {
                printf("맨 윗줄도 선택 가능");
            }

            needsRedraw = 0;
            messageCode = 0;
        }

        if (
            readConsoleInputEvent(
                inputHandle,
                &eventData
            ) == 0
            )
        {
            break;
        }

        if (
            eventData.type == CONSOLE_EVENT_KEY &&
            (
                eventData.virtualKeyCode == VK_RETURN ||
                eventData.virtualKeyCode == VK_ESCAPE
                )
            )
        {
            break;
        }

        if (eventData.type != CONSOLE_EVENT_MOUSE)
        {
            continue;
        }

        if (
            eventData.leftPressed == 1 &&
            previousLeftPressed == 0
            )
        {
            if (
                isPointInsideButton(
                    eventData.mouseX,
                    eventData.mouseY,
                    backButton
                ) == 1
                )
            {
                break;
            }

            if (
                convertMouseToMapPosition(
                    eventData.mouseX,
                    eventData.mouseY,
                    mapStartY,
                    rows,
                    cols,
                    &selectedPosition
                ) == 1
                )
            {
                if (
                    map[selectedPosition.row][selectedPosition.col].isObstacle
                    == 1
                    )
                {
                    dragValue = 0;
                }
                else
                {
                    dragValue = 1;
                }

                isDragging = 1;
                lastRow = -1;
                lastCol = -1;
            }
        }

        if (
            eventData.leftPressed == 1 &&
            isDragging == 1 &&
            convertMouseToMapPosition(
                eventData.mouseX,
                eventData.mouseY,
                mapStartY,
                rows,
                cols,
                &selectedPosition
            ) == 1
            )
        {
            if (
                selectedPosition.row != lastRow ||
                selectedPosition.col != lastCol
                )
            {
                if (
                    dragValue == 1 &&
                    isProtectedPosition(
                        selectedPosition,
                        start,
                        destinations,
                        destinationCount
                    ) == 1
                    )
                {
                    messageCode = 1;
                    needsRedraw = 1;
                }
                else if (
                    dragValue == 1 &&
                    obstacleCount >= maxObstacleCount
                    )
                {
                    messageCode = 2;
                    needsRedraw = 1;
                }
                else if (
                    map[selectedPosition.row][selectedPosition.col].isObstacle
                    != dragValue
                    )
                {
                    if (*routeValid == 1)
                    {
                        invalidateRoute(
                            map,
                            rows,
                            cols,
                            routeValid
                        );

                        needsRedraw = 1;
                    }

                    map[selectedPosition.row][selectedPosition.col].isObstacle =
                        dragValue;

                    if (dragValue == 1)
                    {
                        obstacleCount++;
                    }
                    else
                    {
                        obstacleCount--;
                    }

                    changed = 1;

                    if (needsRedraw == 0)
                    {
                        drawMapCell(
                            map,
                            selectedPosition.row,
                            selectedPosition.col,
                            mapStartY,
                            start,
                            destinations,
                            destinationCount,
                            NULL
                        );

                        setConsoleCursorPosition(
                            buttonLeft,
                            buttonTop + 8
                        );

                        printf(
                            "장애물: %d / %d   ",
                            obstacleCount,
                            maxObstacleCount
                        );
                    }
                }

                lastRow = selectedPosition.row;
                lastCol = selectedPosition.col;
            }
        }

        if (eventData.leftPressed == 0)
        {
            isDragging = 0;
            lastRow = -1;
            lastCol = -1;
        }

        previousLeftPressed =
            eventData.leftPressed;
    }

    endMouseInput(
        inputHandle,
        originalMode
    );

    return changed;
}

/*
 * 출발점과 배송지를 같은 지도 화면에서 편집한다.
 * 먼저 아래 버튼으로 편집 모드를 고른 뒤 맵 칸을 클릭한다.
 */
int editLocationsWithMouse(
    Map** map,
    int rows,
    int cols,
    Position* start,
    Position* destinations,
    int* destinationCount,
    int* routeValid
)
{
    HANDLE inputHandle;
    DWORD originalMode;
    ConsoleInputEvent eventData;

    ConsoleButton startModeButton;
    ConsoleButton destinationModeButton;
    ConsoleButton clearButton;
    ConsoleButton backButton;

    Position selectedPosition;
    LocationEditMode editMode;

    int mapStartY;
    int buttonLeft;
    int buttonTop;
    int previousLeftPressed = 0;
    int existingIndex;
    int i;
    int changed = 0;
    int needsRedraw = 1;
    int messageCode = 0;

    if (start->row < 0)
    {
        editMode = LOCATION_MODE_START;
    }
    else
    {
        editMode = LOCATION_MODE_DESTINATION;
    }

    if (
        beginMouseInput(
            &inputHandle,
            &originalMode
        ) == 0
        )
    {
        return 0;
    }

    while (1)
    {
        if (needsRedraw == 1)
        {
            clearConsoleScreen();

            mapStartY =
                printInteractiveMap(
                    map,
                    rows,
                    cols,
                    *start,
                    destinations,
                    *destinationCount,
                    NULL,
                    0
                );

            buttonLeft =
                MAP_START_X + cols * MAP_CELL_WIDTH + 2;

            buttonTop = 0;

            startModeButton.left = buttonLeft;
            startModeButton.top = buttonTop;
            startModeButton.right = buttonLeft + 28;
            startModeButton.bottom = buttonTop + 2;

            if (editMode == LOCATION_MODE_START)
            {
                startModeButton.text = "출발점 모드 [현재]";
            }
            else
            {
                startModeButton.text = "출발점 모드";
            }

            destinationModeButton.left = buttonLeft;
            destinationModeButton.top = buttonTop + 4;
            destinationModeButton.right = buttonLeft + 28;
            destinationModeButton.bottom = buttonTop + 6;

            if (editMode == LOCATION_MODE_DESTINATION)
            {
                destinationModeButton.text = "배송지 모드 [현재]";
            }
            else
            {
                destinationModeButton.text = "배송지 모드";
            }

            clearButton.left = buttonLeft;
            clearButton.top = buttonTop + 8;
            clearButton.right = buttonLeft + 28;
            clearButton.bottom = buttonTop + 10;
            clearButton.text = "위치 모두 지우기";

            backButton.left = buttonLeft;
            backButton.top = buttonTop + 12;
            backButton.right = buttonLeft + 28;
            backButton.bottom = buttonTop + 14;
            backButton.text = "메뉴로 돌아가기";

            drawButton(startModeButton);
            drawButton(destinationModeButton);
            drawButton(clearButton);
            drawButton(backButton);

            setConsoleCursorPosition(
                buttonLeft,
                buttonTop + 16
            );

            printf(
                "배송지: %d / %d",
                *destinationCount,
                MAX_DESTINATIONS
            );

            setConsoleCursorPosition(
                buttonLeft,
                buttonTop + 18
            );

            if (messageCode == 1)
            {
                printf("장애물 칸 선택 불가");
            }
            else if (messageCode == 2)
            {
                printf("출발점과 겹칠 수 없음");
            }
            else if (messageCode == 3)
            {
                printf("배송지 최대 개수 도달");
            }
            else
            {
                printf("원하는 칸을 클릭하세요");
            }

            setConsoleCursorPosition(
                buttonLeft,
                buttonTop + 20
            );

            printf("Enter/Esc: 메뉴");

            needsRedraw = 0;
            messageCode = 0;
        }

        if (
            readConsoleInputEvent(
                inputHandle,
                &eventData
            ) == 0
            )
        {
            break;
        }

        if (
            eventData.type == CONSOLE_EVENT_KEY &&
            (
                eventData.virtualKeyCode == VK_RETURN ||
                eventData.virtualKeyCode == VK_ESCAPE
                )
            )
        {
            break;
        }

        if (eventData.type != CONSOLE_EVENT_MOUSE)
        {
            continue;
        }

        if (
            eventData.leftPressed == 1 &&
            previousLeftPressed == 0
            )
        {
            if (
                isPointInsideButton(
                    eventData.mouseX,
                    eventData.mouseY,
                    startModeButton
                ) == 1
                )
            {
                editMode = LOCATION_MODE_START;
                needsRedraw = 1;
            }
            else if (
                isPointInsideButton(
                    eventData.mouseX,
                    eventData.mouseY,
                    destinationModeButton
                ) == 1
                )
            {
                editMode = LOCATION_MODE_DESTINATION;
                needsRedraw = 1;
            }
            else if (
                isPointInsideButton(
                    eventData.mouseX,
                    eventData.mouseY,
                    clearButton
                ) == 1
                )
            {
                if (
                    start->row >= 0 ||
                    *destinationCount > 0
                    )
                {
                    start->row = -1;
                    start->col = -1;
                    *destinationCount = 0;

                    invalidateRoute(
                        map,
                        rows,
                        cols,
                        routeValid
                    );

                    changed = 1;
                    editMode = LOCATION_MODE_START;
                }

                needsRedraw = 1;
            }
            else if (
                isPointInsideButton(
                    eventData.mouseX,
                    eventData.mouseY,
                    backButton
                ) == 1
                )
            {
                break;
            }
            else if (
                convertMouseToMapPosition(
                    eventData.mouseX,
                    eventData.mouseY,
                    mapStartY,
                    rows,
                    cols,
                    &selectedPosition
                ) == 1
                )
            {
                if (
                    isWalkable(
                        map,
                        selectedPosition.row,
                        selectedPosition.col
                    ) == 0
                    )
                {
                    messageCode = 1;
                    needsRedraw = 1;
                }
                else if (editMode == LOCATION_MODE_START)
                {
                    existingIndex =
                        findPositionIndex(
                            destinations,
                            *destinationCount,
                            selectedPosition
                        );

                    if (existingIndex >= 0)
                    {
                        for (
                            i = existingIndex;
                            i < *destinationCount - 1;
                            i++
                            )
                        {
                            destinations[i] =
                                destinations[i + 1];
                        }

                        (*destinationCount)--;
                    }

                    if (
                        start->row != selectedPosition.row ||
                        start->col != selectedPosition.col
                        )
                    {
                        *start = selectedPosition;

                        invalidateRoute(
                            map,
                            rows,
                            cols,
                            routeValid
                        );

                        changed = 1;
                    }

                    needsRedraw = 1;
                }
                else
                {
                    if (
                        selectedPosition.row == start->row &&
                        selectedPosition.col == start->col
                        )
                    {
                        messageCode = 2;
                        needsRedraw = 1;
                    }
                    else
                    {
                        existingIndex =
                            findPositionIndex(
                                destinations,
                                *destinationCount,
                                selectedPosition
                            );

                        if (existingIndex >= 0)
                        {
                            for (
                                i = existingIndex;
                                i < *destinationCount - 1;
                                i++
                                )
                            {
                                destinations[i] =
                                    destinations[i + 1];
                            }

                            (*destinationCount)--;

                            invalidateRoute(
                                map,
                                rows,
                                cols,
                                routeValid
                            );

                            changed = 1;
                            needsRedraw = 1;
                        }
                        else if (*destinationCount >= MAX_DESTINATIONS)
                        {
                            messageCode = 3;
                            needsRedraw = 1;
                        }
                        else
                        {
                            destinations[*destinationCount] =
                                selectedPosition;

                            (*destinationCount)++;

                            invalidateRoute(
                                map,
                                rows,
                                cols,
                                routeValid
                            );

                            changed = 1;
                            needsRedraw = 1;
                        }
                    }
                }
            }
        }

        previousLeftPressed =
            eventData.leftPressed;
    }

    endMouseInput(
        inputHandle,
        originalMode
    );

    return changed;
}

/*
 * A* 비용 행렬, 완전탐색 TSP, 구간별 A* 경로 연결을 차례대로 실행한다.
 */
int calculateDeliveryRoute(
    Map** map,
    AStarNode** nodeMap,
    int rows,
    int cols,
    Position start,
    Position* destinations,
    int destinationCount,
    int* visitOrder,
    int* segmentCosts,
    long long* optimalCost,
    long long* totalCost,
    Position** totalPath,
    size_t* totalPathLength
)
{
    int costMatrix[MAX_POINTS][MAX_POINTS];
    int matrixResult;
    int routeResult;

    size_t cellCount;
    size_t totalPathCapacity;
    size_t pathIndex;

    *totalPath = NULL;
    *totalPathLength = 0;
    *optimalCost = 0;
    *totalCost = 0;

    clearPathMarks(
        map,
        rows,
        cols
    );

    if (start.row < 0 || start.col < 0)
    {
        return ROUTE_MISSING_START;
    }

    if (destinationCount <= 0)
    {
        return ROUTE_MISSING_DESTINATION;
    }

    matrixResult = buildCostMatrix(
        map,
        nodeMap,
        rows,
        cols,
        start,
        destinations,
        destinationCount,
        costMatrix
    );

    if (matrixResult == PATH_ERROR)
    {
        return ROUTE_MEMORY_ERROR;
    }

    if (matrixResult == PATH_NOT_FOUND)
    {
        return ROUTE_UNREACHABLE;
    }

    if (
        determineOptimalVisitOrder(
            costMatrix,
            destinationCount,
            visitOrder,
            optimalCost
        ) == 0
        )
    {
        return ROUTE_ORDER_ERROR;
    }

    cellCount =
        (size_t)rows *
        (size_t)cols;

    if (
        cellCount >
        SIZE_MAX / (size_t)destinationCount
        )
    {
        return ROUTE_MEMORY_ERROR;
    }

    totalPathCapacity =
        cellCount *
        (size_t)destinationCount;

    if (
        totalPathCapacity >
        SIZE_MAX / sizeof(Position)
        )
    {
        return ROUTE_MEMORY_ERROR;
    }

    *totalPath = (Position*)malloc(
        totalPathCapacity *
        sizeof(Position)
    );

    if (*totalPath == NULL)
    {
        return ROUTE_MEMORY_ERROR;
    }

    routeResult = buildCompleteRoute(
        map,
        nodeMap,
        rows,
        cols,
        start,
        destinations,
        destinationCount,
        visitOrder,
        *totalPath,
        totalPathCapacity,
        totalPathLength,
        segmentCosts,
        totalCost
    );

    if (routeResult != PATH_FOUND)
    {
        free(*totalPath);
        *totalPath = NULL;
        *totalPathLength = 0;

        if (routeResult == PATH_ERROR)
        {
            return ROUTE_MEMORY_ERROR;
        }

        return ROUTE_PATH_ERROR;
    }

    for (
        pathIndex = 0;
        pathIndex < *totalPathLength;
        pathIndex++
        )
    {
        map
            [(*totalPath)[pathIndex].row]
            [(*totalPath)[pathIndex].col]
            .isPath = 1;
    }

    return ROUTE_SUCCESS;
}

/*
 * 경로 계산 결과를 보여 주고, 사용자가 돌아가기 버튼을 누를 때까지 기다린다.
 */
void showResultScreen(
    int routeResult,
    Map** map,
    int rows,
    int cols,
    Position start,
    Position* destinations,
    int destinationCount,
    int* visitOrder,
    int* segmentCosts,
    long long optimalCost,
    long long totalCost,
    Position* totalPath,
    size_t totalPathLength
)
{
    HANDLE inputHandle;
    DWORD originalMode;
    ConsoleInputEvent eventData;
    ConsoleButton backButton;

    int buttonLeft;
    int buttonTop;
    int previousLeftPressed = 0;

    clearConsoleScreen();

    printf("===== 최적 배송 경로 계산 결과 =====\n");

    if (routeResult == ROUTE_SUCCESS)
    {
        printf("완전탐색 TSP 최적 비용: %lld\n", optimalCost);

        printDeliveryResult(
            start,
            destinations,
            destinationCount,
            visitOrder,
            segmentCosts,
            totalCost,
            totalPath,
            totalPathLength
        );

        /*
         * 전체 경로 출력이 끝난 뒤의 위치를 기준으로 버튼을 배치한다.
         * 기존처럼 buttonTop을 2로 고정하면 긴 경로 문장 위에
         * 버튼이 겹쳐 출력되는 문제가 발생한다.
         */
        buttonTop =
            getConsoleCursorY() + 2;

        printMap(
            map,
            rows,
            cols,
            start,
            destinations,
            destinationCount,
            visitOrder
        );
    }
    else if (routeResult == ROUTE_MISSING_START)
    {
        printf("출발점이 설정되지 않았습니다.\n");
    }
    else if (routeResult == ROUTE_MISSING_DESTINATION)
    {
        printf("배송지를 한 곳 이상 설정해야 합니다.\n");
    }
    else if (routeResult == ROUTE_UNREACHABLE)
    {
        printf("장애물 때문에 도달할 수 없는 배송지가 있습니다.\n");
        printf("장애물 배치를 수정한 뒤 다시 계산하세요.\n");
    }
    else if (routeResult == ROUTE_ORDER_ERROR)
    {
        printf("방문 가능한 배송지 순서를 찾지 못했습니다.\n");
    }
    else if (routeResult == ROUTE_MEMORY_ERROR)
    {
        printf("경로 계산 중 메모리 처리 오류가 발생했습니다.\n");
    }
    else
    {
        printf("전체 배송 경로를 만들 수 없습니다.\n");
    }

    if (routeResult == ROUTE_SUCCESS)
    {
        buttonLeft =
            MAP_START_X + cols * MAP_CELL_WIDTH + 6;
    }
    else
    {
        buttonLeft = 5;

        /*
         * 오류 결과에서도 기존 문장을 덮어쓰지 않도록
         * 마지막 출력 줄 아래에 버튼을 배치한다.
         */
        buttonTop =
            getConsoleCursorY() + 2;
    }

    backButton.left = buttonLeft;
    backButton.top = buttonTop;
    backButton.right = buttonLeft + 33;
    backButton.bottom = buttonTop + 2;
    backButton.text = "메뉴로 돌아가기";

    drawButton(backButton);

    setConsoleCursorPosition(
        buttonLeft,
        buttonTop + 4
    );

    printf("Enter 또는 Esc를 눌러도 메뉴로 돌아갑니다.\n");

    if (
        beginMouseInput(
            &inputHandle,
            &originalMode
        ) == 0
        )
    {
        return;
    }

    while (1)
    {
        if (
            readConsoleInputEvent(
                inputHandle,
                &eventData
            ) == 0
            )
        {
            break;
        }

        if (
            eventData.type == CONSOLE_EVENT_KEY &&
            (
                eventData.virtualKeyCode == VK_RETURN ||
                eventData.virtualKeyCode == VK_ESCAPE
                )
            )
        {
            break;
        }

        if (eventData.type != CONSOLE_EVENT_MOUSE)
        {
            continue;
        }

        if (
            eventData.leftPressed == 1 &&
            previousLeftPressed == 0 &&
            isPointInsideButton(
                eventData.mouseX,
                eventData.mouseY,
                backButton
            ) == 1
            )
        {
            break;
        }

        previousLeftPressed =
            eventData.leftPressed;
    }

    endMouseInput(
        inputHandle,
        originalMode
    );
}

/*
 * 실제 맵 정보를 저장할 2차원 동적 배열을 만든다.
 */
Map** createMap(int rows, int cols)
{
    Map** map;
    int i;

    map = (Map**)malloc(
        rows * sizeof(Map*)
    );

    if (map == NULL)
    {
        return NULL;
    }

    for (i = 0; i < rows; i++)
    {
        map[i] = (Map*)malloc(
            cols * sizeof(Map)
        );

        if (map[i] == NULL)
        {
            int j;

            for (j = 0; j < i; j++)
            {
                free(map[j]);
            }

            free(map);

            return NULL;
        }
    }

    return map;
}

/*
 * A* 탐색 정보를 저장할 2차원 동적 배열을 만든다.
 */
AStarNode** createNodeMap(int rows, int cols)
{
    AStarNode** nodeMap;
    int i;

    nodeMap = (AStarNode**)malloc(
        rows * sizeof(AStarNode*)
    );

    if (nodeMap == NULL)
    {
        return NULL;
    }

    for (i = 0; i < rows; i++)
    {
        nodeMap[i] = (AStarNode*)malloc(
            cols * sizeof(AStarNode)
        );

        if (nodeMap[i] == NULL)
        {
            int j;

            for (j = 0; j < i; j++)
            {
                free(nodeMap[j]);
            }

            free(nodeMap);

            return NULL;
        }
    }

    return nodeMap;
}

/*
 * 맵의 모든 칸을 기본 상태로 초기화한다.
 */
void initializeMap(
    Map** map,
    int rows,
    int cols
)
{
    int row;
    int col;

    for (row = 0; row < rows; row++)
    {
        for (col = 0; col < cols; col++)
        {
            map[row][col].isObstacle = 0;
            map[row][col].height = 0;
            map[row][col].isPath = 0;
        }
    }

    initializeHeights(
        map,
        rows,
        cols
    );
}

/*
 * 모든 지형 높이를 0으로 초기화한다.
 */
void initializeHeights(
    Map** map,
    int rows,
    int cols
)
{
    int row;
    int col;

    for (row = 0; row < rows; row++)
    {
        for (col = 0; col < cols; col++)
        {
            map[row][col].height = 0;
        }
    }
}

/*
 * 콘솔 화면 전체를 지우고 커서를 왼쪽 위로 이동한다.
 */
void clearConsoleScreen(void)
{
    HANDLE outputHandle;
    CONSOLE_SCREEN_BUFFER_INFO screenInfo;
    COORD homePosition;
    DWORD cellCount;
    DWORD writtenCount;

    outputHandle = GetStdHandle(STD_OUTPUT_HANDLE);

    if (outputHandle == INVALID_HANDLE_VALUE)
    {
        return;
    }

    if (GetConsoleScreenBufferInfo(outputHandle, &screenInfo) == 0)
    {
        return;
    }

    cellCount =
        (DWORD)screenInfo.dwSize.X *
        (DWORD)screenInfo.dwSize.Y;

    homePosition.X = 0;
    homePosition.Y = 0;

    FillConsoleOutputCharacterA(
        outputHandle,
        ' ',
        cellCount,
        homePosition,
        &writtenCount
    );

    FillConsoleOutputAttribute(
        outputHandle,
        screenInfo.wAttributes,
        cellCount,
        homePosition,
        &writtenCount
    );

    SetConsoleCursorPosition(
        outputHandle,
        homePosition
    );
}

/*
 * Windows 콘솔에서 마우스 클릭을 받을 수 있도록 입력 모드를 설정한다.
 */
int beginMouseInput(
    HANDLE* inputHandle,
    DWORD* originalMode
)
{
    DWORD mouseMode;

    *inputHandle =
        GetStdHandle(STD_INPUT_HANDLE);

    if (*inputHandle == INVALID_HANDLE_VALUE)
    {
        return 0;
    }

    if (
        GetConsoleMode(
            *inputHandle,
            originalMode
        ) == 0
        )
    {
        return 0;
    }

    mouseMode = *originalMode;

    mouseMode |= ENABLE_EXTENDED_FLAGS;
    mouseMode |= ENABLE_MOUSE_INPUT;
    mouseMode |= ENABLE_WINDOW_INPUT;
    mouseMode &= ~ENABLE_QUICK_EDIT_MODE;

    if (
        SetConsoleMode(
            *inputHandle,
            mouseMode
        ) == 0
        )
    {
        return 0;
    }

    FlushConsoleInputBuffer(*inputHandle);

    return 1;
}

/*
 * 마우스 설정이 끝난 뒤 원래 콘솔 입력 모드로 복구한다.
 */
void endMouseInput(
    HANDLE inputHandle,
    DWORD originalMode
)
{
    SetConsoleMode(
        inputHandle,
        originalMode
    );

    FlushConsoleInputBuffer(inputHandle);
}

/*
 * 좌표 배열 안에 target과 같은 좌표가 있는지 찾는다.
 */
int findPositionIndex(
    Position* positions,
    int count,
    Position target
)
{
    int i;

    for (i = 0; i < count; i++)
    {
        if (
            positions[i].row == target.row &&
            positions[i].col == target.col
            )
        {
            return i;
        }
    }

    return -1;
}

/*
 * 현재 좌표가 배송지라면 최종 방문 순서 번호를 반환한다.
 * 방문 순서가 아직 계산되지 않았다면 0을 반환해 D로 표시한다.
 * 배송지가 아니라면 -1을 반환한다.
 */
int getDestinationVisitNumber(
    int row,
    int col,
    Position* destinations,
    int destinationCount,
    int* visitOrder
)
{
    int destinationIndex;
    int orderIndex;

    if (destinations == NULL)
    {
        return -1;
    }

    for (
        destinationIndex = 0;
        destinationIndex < destinationCount;
        destinationIndex++
        )
    {
        if (
            destinations[destinationIndex].row == row &&
            destinations[destinationIndex].col == col
            )
        {
            if (visitOrder == NULL)
            {
                return 0;
            }

            for (
                orderIndex = 0;
                orderIndex < destinationCount;
                orderIndex++
                )
            {
                if (
                    visitOrder[orderIndex] ==
                    destinationIndex
                    )
                {
                    return orderIndex + 1;
                }
            }
        }
    }

    return -1;
}

/*
 * 맵의 현재 상태를 네모 격자 형태로 출력한다.
 *
 * 빈 칸은 공백으로 두고, 장애물은 블록 문자로 가득 채운다.
 * 따라서 기존의 숫자 0과 #보다 지도 형태를 알아보기 쉽다.
 */
int printInteractiveMap(
    Map** map,
    int rows,
    int cols,
    Position start,
    Position* destinations,
    int destinationCount,
    int* visitOrder,
    int mapTopY
)
{
    int row;
    int col;
    int destinationNumber;
    int firstCellY;

    setConsoleCursorPosition(
        0,
        mapTopY
    );

    printf("    ");

    for (col = 0; col < cols; col++)
    {
        printf("%4d", col);
    }

    printf("\n    ┌");

    for (col = 0; col < cols; col++)
    {
        printf("───");

        if (col == cols - 1)
        {
            printf("┐");
        }
        else
        {
            printf("┬");
        }
    }

    printf("\n");

    firstCellY =
        getConsoleCursorY();

    for (row = 0; row < rows; row++)
    {
        printf("%3d │", row);

        for (col = 0; col < cols; col++)
        {
            destinationNumber =
                getDestinationVisitNumber(
                    row,
                    col,
                    destinations,
                    destinationCount,
                    visitOrder
                );

            if (map[row][col].isObstacle == 1)
            {
                printf("███");
            }
            else if (
                row == start.row &&
                col == start.col
                )
            {
                printf(" S ");
            }
            else if (destinationNumber == 0)
            {
                printf(" D ");
            }
            else if (destinationNumber > 0)
            {
                printf("%3d", destinationNumber);
            }
            else if (map[row][col].isPath == 1)
            {
                printf(" * ");
            }
            else
            {
                printf("   ");
            }

            printf("│");
        }

        printf("\n    ");

        if (row == rows - 1)
        {
            printf("└");
        }
        else
        {
            printf("├");
        }

        for (col = 0; col < cols; col++)
        {
            printf("───");

            if (col == cols - 1)
            {
                if (row == rows - 1)
                {
                    printf("┘");
                }
                else
                {
                    printf("┤");
                }
            }
            else
            {
                if (row == rows - 1)
                {
                    printf("┴");
                }
                else
                {
                    printf("┼");
                }
            }
        }

        /*
         * 마지막 줄에서는 개행하지 않는다.
         * 10×10 지도가 25줄 콘솔에서 마지막 개행 때문에
         * 한 줄 위로 스크롤되는 현상을 방지한다.
         */
        if (row < rows - 1)
        {
            printf("\n");
        }
    }

    return firstCellY;
}

void printMap(
    Map** map,
    int rows,
    int cols,
    Position start,
    Position* destinations,
    int destinationCount,
    int* visitOrder
)
{
    int row;
    int col;
    int destinationNumber;

    /* 열 번호를 각 네모 칸의 가운데에 맞춰 출력한다. */
    printf("\n    ");

    for (col = 0; col < cols; col++)
    {
        printf("%4d", col);
    }

    printf("\n    ┌");

    for (col = 0; col < cols; col++)
    {
        printf("───");

        if (col == cols - 1)
        {
            printf("┐");
        }
        else
        {
            printf("┬");
        }
    }

    printf("\n");

    for (row = 0; row < rows; row++)
    {
        printf("%3d │", row);

        for (col = 0; col < cols; col++)
        {
            destinationNumber =
                getDestinationVisitNumber(
                    row,
                    col,
                    destinations,
                    destinationCount,
                    visitOrder
                );

            if (map[row][col].isObstacle == 1)
            {
                printf("███");
            }
            else if (
                row == start.row &&
                col == start.col
                )
            {
                printf(" S ");
            }
            else if (destinationNumber == 0)
            {
                printf(" D ");
            }
            else if (destinationNumber > 0)
            {
                printf("%3d", destinationNumber);
            }
            else if (map[row][col].isPath == 1)
            {
                printf(" * ");
            }
            else
            {
                printf("   ");
            }

            printf("│");
        }

        printf("\n    ");

        if (row == rows - 1)
        {
            printf("└");
        }
        else
        {
            printf("├");
        }

        for (col = 0; col < cols; col++)
        {
            printf("───");

            if (col == cols - 1)
            {
                if (row == rows - 1)
                {
                    printf("┘");
                }
                else
                {
                    printf("┤");
                }
            }
            else
            {
                if (row == rows - 1)
                {
                    printf("┴");
                }
                else
                {
                    printf("┼");
                }
            }
        }

        printf("\n");
    }

    if (visitOrder == NULL)
    {
        printf(
            "\nS: 출발지, D: 선택한 배송지, "
            "*: 경로, █: 장애물\n"
        );
    }
    else
    {
        printf(
            "\nS: 출발지, 숫자: 배송 방문 순서, "
            "*: 전체 경로, █: 장애물\n"
        );
    }
}

/*
 * 사용자가 원하는 맵의 행과 열 크기를 입력받는다.
 */
void inputMapSize(
    int* rows,
    int* cols
)
{
    int inputResult;
    int character;

    while (1)
    {
        printf("맵의 행과 열 크기를 입력하세요: ");

        inputResult = scanf(
            "%d %d",
            rows,
            cols
        );

        do
        {
            character = getchar();
        } while (
            character != '\n' &&
            character != EOF
            );

        if (inputResult != 2)
        {
            printf("행과 열을 정수 두 개로 입력해야 합니다.\n");
            continue;
        }

        if (*rows <= 0 || *cols <= 0)
        {
            printf("행과 열의 크기는 1 이상이어야 합니다.\n");
            continue;
        }

        if (*rows == 1 && *cols == 1)
        {
            printf("출발지와 배송지를 위해 칸이 2개 이상 필요합니다.\n");
            continue;
        }

        if (*rows > INT_MAX / *cols)
        {
            printf("입력한 맵의 크기가 너무 큽니다.\n");
            continue;
        }

        return;
    }
}

int isInsideMap(
    int row,
    int col,
    int rows,
    int cols
)
{
    if (
        row >= 0 &&
        row < rows &&
        col >= 0 &&
        col < cols
        )
    {
        return 1;
    }

    return 0;
}

int isWalkable(
    Map** map,
    int row,
    int col
)
{
    if (map[row][col].isObstacle == 0)
    {
        return 1;
    }

    return 0;
}

/*
 * A* 탐색을 시작하기 전에 모든 노드 정보를 초기화한다.
 */
void initializeAStarNodes(
    AStarNode** nodeMap,
    int rows,
    int cols
)
{
    int row;
    int col;

    for (row = 0; row < rows; row++)
    {
        for (col = 0; col < cols; col++)
        {
            nodeMap[row][col].gCost = INF;
            nodeMap[row][col].hCost = 0;
            nodeMap[row][col].fCost = INF;
            nodeMap[row][col].isOpen = 0;
            nodeMap[row][col].isClosed = 0;
            nodeMap[row][col].parent.row = -1;
            nodeMap[row][col].parent.col = -1;
        }
    }
}

/*
 * 현재 위치에서 목적지까지의 맨해튼 거리를 계산한다.
 */
int calculateHeuristic(
    Position current,
    Position destination
)
{
    int rowDistance;
    int colDistance;

    rowDistance =
        current.row - destination.row;

    colDistance =
        current.col - destination.col;

    if (rowDistance < 0)
    {
        rowDistance = -rowDistance;
    }

    if (colDistance < 0)
    {
        colDistance = -colDistance;
    }

    return rowDistance + colDistance;
}

/*
 * 현재 칸에서 다음 칸으로 이동할 실제 비용을 계산한다.
 */
int calculateMoveCost(
    Map** map,
    Position current,
    Position next
)
{
    int heightDifference;

    heightDifference =
        map[current.row][current.col].height -
        map[next.row][next.col].height;

    if (heightDifference < 0)
    {
        heightDifference = -heightDifference;
    }

    return 1 + heightDifference;
}

PriorityQueue* initializePriorityQueue(
    int initialCapacity
)
{
    PriorityQueue* queue;

    queue = (PriorityQueue*)malloc(
        sizeof(PriorityQueue)
    );

    if (queue == NULL)
    {
        return NULL;
    }

    queue->items = (QueueItem*)malloc(
        initialCapacity * sizeof(QueueItem)
    );

    if (queue->items == NULL)
    {
        free(queue);
        return NULL;
    }

    queue->size = 0;
    queue->capacity = initialCapacity;

    return queue;
}

int hasHigherPriority(
    QueueItem first,
    QueueItem second
)
{
    if (first.fCost < second.fCost)
    {
        return 1;
    }

    if (first.fCost > second.fCost)
    {
        return 0;
    }

    if (first.hCost < second.hCost)
    {
        return 1;
    }

    return 0;
}

void swapQueueItem(
    QueueItem* first,
    QueueItem* second
)
{
    QueueItem temp;

    temp = *first;
    *first = *second;
    *second = temp;
}

int pushPriorityQueue(
    PriorityQueue* queue,
    QueueItem item
)
{
    QueueItem* resizedItems;
    int currentIndex;
    int parentIndex;

    if (queue->size >= queue->capacity)
    {
        resizedItems = (QueueItem*)realloc(
            queue->items,
            queue->capacity *
            2 *
            sizeof(QueueItem)
        );

        if (resizedItems == NULL)
        {
            return 0;
        }

        queue->items = resizedItems;
        queue->capacity *= 2;
    }

    currentIndex = queue->size;
    queue->items[currentIndex] = item;
    queue->size++;

    while (currentIndex > 0)
    {
        parentIndex =
            (currentIndex - 1) / 2;

        if (
            hasHigherPriority(
                queue->items[currentIndex],
                queue->items[parentIndex]
            ) == 0
            )
        {
            break;
        }

        swapQueueItem(
            &queue->items[currentIndex],
            &queue->items[parentIndex]
        );

        currentIndex = parentIndex;
    }

    return 1;
}

QueueItem popPriorityQueue(
    PriorityQueue* queue
)
{
    QueueItem rootItem;
    QueueItem emptyItem;

    int currentIndex;
    int leftChildIndex;
    int rightChildIndex;
    int higherPriorityChildIndex;

    emptyItem.position.row = -1;
    emptyItem.position.col = -1;
    emptyItem.fCost = INF;
    emptyItem.hCost = INF;

    if (
        queue == NULL ||
        queue->size == 0
        )
    {
        return emptyItem;
    }

    rootItem = queue->items[0];

    queue->size--;

    if (queue->size > 0)
    {
        queue->items[0] =
            queue->items[queue->size];
    }

    currentIndex = 0;

    while (1)
    {
        leftChildIndex =
            currentIndex * 2 + 1;

        rightChildIndex =
            currentIndex * 2 + 2;

        higherPriorityChildIndex =
            currentIndex;

        if (
            leftChildIndex < queue->size &&
            hasHigherPriority(
                queue->items[leftChildIndex],
                queue->items[higherPriorityChildIndex]
            ) == 1
            )
        {
            higherPriorityChildIndex =
                leftChildIndex;
        }

        if (
            rightChildIndex < queue->size &&
            hasHigherPriority(
                queue->items[rightChildIndex],
                queue->items[higherPriorityChildIndex]
            ) == 1
            )
        {
            higherPriorityChildIndex =
                rightChildIndex;
        }

        if (
            higherPriorityChildIndex ==
            currentIndex
            )
        {
            break;
        }

        swapQueueItem(
            &queue->items[currentIndex],
            &queue->items[higherPriorityChildIndex]
        );

        currentIndex =
            higherPriorityChildIndex;
    }

    return rootItem;
}

/*
 * A* 알고리즘으로 한 시작점부터 한 목적지까지의 최적 경로를 찾는다.
 */
int findPath(
    Map** map,
    AStarNode** nodeMap,
    int rows,
    int cols,
    Position start,
    Position destination,
    Position* path,
    int* pathLength,
    PriorityQueue** createdQueue
)
{
    PriorityQueue* queue;

    QueueItem currentItem;
    QueueItem nextItem;

    Position current;
    Position next;

    int directionRow[4] =
    {
        -1,
        1,
        0,
        0
    };

    int directionCol[4] =
    {
        0,
        0,
        -1,
        1
    };

    int direction;
    int newGCost;

    *pathLength = 0;

    queue = initializePriorityQueue(
        rows * cols
    );

    *createdQueue = queue;

    if (queue == NULL)
    {
        return PATH_ERROR;
    }

    nodeMap[start.row][start.col].gCost = 0;

    nodeMap[start.row][start.col].hCost =
        calculateHeuristic(
            start,
            destination
        );

    nodeMap[start.row][start.col].fCost =
        nodeMap[start.row][start.col].gCost +
        nodeMap[start.row][start.col].hCost;

    nodeMap[start.row][start.col].parent =
        start;

    nodeMap[start.row][start.col].isOpen = 1;

    nextItem.position = start;
    nextItem.fCost =
        nodeMap[start.row][start.col].fCost;
    nextItem.hCost =
        nodeMap[start.row][start.col].hCost;

    if (
        pushPriorityQueue(
            queue,
            nextItem
        ) == 0
        )
    {
        return PATH_ERROR;
    }

    while (queue->size > 0)
    {
        currentItem =
            popPriorityQueue(queue);

        current =
            currentItem.position;

        if (
            nodeMap[current.row][current.col].isClosed
            == 1
            )
        {
            continue;
        }

        if (
            currentItem.fCost !=
            nodeMap[current.row][current.col].fCost
            )
        {
            continue;
        }

        nodeMap[current.row][current.col].isOpen = 0;
        nodeMap[current.row][current.col].isClosed = 1;

        if (
            current.row == destination.row &&
            current.col == destination.col
            )
        {
            reconstructPath(
                nodeMap,
                start,
                destination,
                path,
                pathLength
            );

            return PATH_FOUND;
        }

        for (
            direction = 0;
            direction < 4;
            direction++
            )
        {
            next.row =
                current.row +
                directionRow[direction];

            next.col =
                current.col +
                directionCol[direction];

            if (
                isInsideMap(
                    next.row,
                    next.col,
                    rows,
                    cols
                ) == 0
                )
            {
                continue;
            }

            if (
                isWalkable(
                    map,
                    next.row,
                    next.col
                ) == 0
                )
            {
                continue;
            }

            if (
                nodeMap[next.row][next.col].isClosed
                == 1
                )
            {
                continue;
            }

            newGCost =
                nodeMap[current.row][current.col].gCost +
                calculateMoveCost(
                    map,
                    current,
                    next
                );

            if (
                newGCost <
                nodeMap[next.row][next.col].gCost
                )
            {
                nodeMap[next.row][next.col].gCost =
                    newGCost;

                nodeMap[next.row][next.col].hCost =
                    calculateHeuristic(
                        next,
                        destination
                    );

                nodeMap[next.row][next.col].fCost =
                    nodeMap[next.row][next.col].gCost +
                    nodeMap[next.row][next.col].hCost;

                nodeMap[next.row][next.col].parent =
                    current;

                nodeMap[next.row][next.col].isOpen = 1;

                nextItem.position = next;
                nextItem.fCost =
                    nodeMap[next.row][next.col].fCost;
                nextItem.hCost =
                    nodeMap[next.row][next.col].hCost;

                if (
                    pushPriorityQueue(
                        queue,
                        nextItem
                    ) == 0
                    )
                {
                    return PATH_ERROR;
                }
            }
        }
    }

    return PATH_NOT_FOUND;
}

/*
 * 각 노드의 parent를 따라가며 한 구간의 전체 최단 경로를 만든다.
 */
void reconstructPath(
    AStarNode** nodeMap,
    Position start,
    Position destination,
    Position* path,
    int* pathLength
)
{
    Position current;
    Position temp;

    int left;
    int right;

    current = destination;
    *pathLength = 0;

    while (
        !(
            current.row == start.row &&
            current.col == start.col
            )
        )
    {
        path[*pathLength] = current;
        (*pathLength)++;

        current =
            nodeMap[current.row][current.col].parent;
    }

    path[*pathLength] = start;
    (*pathLength)++;

    left = 0;
    right =
        *pathLength - 1;

    while (left < right)
    {
        temp = path[left];
        path[left] = path[right];
        path[right] = temp;

        left++;
        right--;
    }
}

/*
 * 출발지와 배송지 사이의 모든 실제 A* 최단 비용을 계산한다.
 *
 * point 0: 출발지
 * point 1: destinations[0]
 * point 2: destinations[1]
 * ...
 */
int buildCostMatrix(
    Map** map,
    AStarNode** nodeMap,
    int rows,
    int cols,
    Position start,
    Position* destinations,
    int destinationCount,
    int costMatrix[MAX_POINTS][MAX_POINTS]
)
{
    Position points[MAX_POINTS];
    Position* temporaryPath;
    PriorityQueue* queue;

    int pointCount;
    int pathLength;
    int pathResult;
    int i;
    int j;

    pointCount =
        destinationCount + 1;

    points[0] = start;

    for (i = 0; i < destinationCount; i++)
    {
        points[i + 1] = destinations[i];
    }

    temporaryPath = (Position*)malloc(
        (size_t)rows *
        (size_t)cols *
        sizeof(Position)
    );

    if (temporaryPath == NULL)
    {
        return PATH_ERROR;
    }

    for (i = 0; i < pointCount; i++)
    {
        costMatrix[i][i] = 0;
    }

    for (i = 0; i < pointCount; i++)
    {
        for (j = i + 1; j < pointCount; j++)
        {
            queue = NULL;

            initializeAStarNodes(
                nodeMap,
                rows,
                cols
            );

            pathResult = findPath(
                map,
                nodeMap,
                rows,
                cols,
                points[i],
                points[j],
                temporaryPath,
                &pathLength,
                &queue
            );

            if (pathResult == PATH_ERROR)
            {
                freePriorityQueue(queue);
                free(temporaryPath);

                return PATH_ERROR;
            }

            if (pathResult == PATH_NOT_FOUND)
            {
                costMatrix[i][j] = INF;
                costMatrix[j][i] = INF;
            }
            else
            {
                costMatrix[i][j] =
                    nodeMap[points[j].row][points[j].col].gCost;

                /*
                 * 현재 이동 비용은 방향에 관계없이 같으므로 대칭으로 저장한다.
                 */
                costMatrix[j][i] =
                    costMatrix[i][j];
            }

            freePriorityQueue(queue);
        }
    }

    free(temporaryPath);

    for (i = 1; i < pointCount; i++)
    {
        if (costMatrix[0][i] >= INF)
        {
            return PATH_NOT_FOUND;
        }
    }

    return PATH_FOUND;
}

/*
 * 아직 방문하지 않은 배송지를 하나씩 선택하면서
 * 가능한 모든 배송 순서를 재귀적으로 생성한다.
 *
 * 모든 배송지를 방문했을 때 총비용이 기존 bestCost보다 작다면
 * bestOrder를 현재 순서로 교체한다.
 */
void searchOptimalOrder(
    int costMatrix[MAX_POINTS][MAX_POINTS],
    int destinationCount,
    int depth,
    int currentPointIndex,
    long long currentCost,
    int* used,
    int* currentOrder,
    int* bestOrder,
    long long* bestCost
)
{
    int destinationIndex;
    int nextPointIndex;
    int edgeCost;
    int i;

    /*
     * 이미 현재 비용이 최적 비용 이상이면
     * 뒤에 어떤 배송지를 더 방문해도 더 좋아질 수 없으므로 중단한다.
     */
    if (currentCost >= *bestCost)
    {
        return;
    }

    if (depth == destinationCount)
    {
        *bestCost = currentCost;

        for (i = 0; i < destinationCount; i++)
        {
            bestOrder[i] =
                currentOrder[i];
        }

        return;
    }

    for (
        destinationIndex = 0;
        destinationIndex < destinationCount;
        destinationIndex++
        )
    {
        if (used[destinationIndex] == 1)
        {
            continue;
        }

        /*
         * costMatrix의 0번은 출발지이므로
         * 배송지 배열 인덱스에 1을 더해 행렬 인덱스로 바꾼다.
         */
        nextPointIndex =
            destinationIndex + 1;

        edgeCost =
            costMatrix[currentPointIndex][nextPointIndex];

        if (edgeCost >= INF)
        {
            continue;
        }

        used[destinationIndex] = 1;
        currentOrder[depth] =
            destinationIndex;

        searchOptimalOrder(
            costMatrix,
            destinationCount,
            depth + 1,
            nextPointIndex,
            currentCost + edgeCost,
            used,
            currentOrder,
            bestOrder,
            bestCost
        );

        used[destinationIndex] = 0;
    }
}

/*
 * 완전탐색 TSP를 시작하고 최적 방문 순서와 비용을 반환한다.
 * 출발지로 다시 돌아오는 비용은 더하지 않는다.
 */
int determineOptimalVisitOrder(
    int costMatrix[MAX_POINTS][MAX_POINTS],
    int destinationCount,
    int* bestOrder,
    long long* bestCost
)
{
    int used[MAX_DESTINATIONS] = { 0 };
    int currentOrder[MAX_DESTINATIONS];

    *bestCost = LLONG_MAX;

    searchOptimalOrder(
        costMatrix,
        destinationCount,
        0,
        0,
        0,
        used,
        currentOrder,
        bestOrder,
        bestCost
    );

    if (*bestCost == LLONG_MAX)
    {
        return 0;
    }

    return 1;
}

/*
 * TSP가 정한 방문 순서대로 각 구간을 A*로 다시 탐색한다.
 * 각 구간 경로를 연결해 하나의 전체 배송 경로를 만든다.
 */
int buildCompleteRoute(
    Map** map,
    AStarNode** nodeMap,
    int rows,
    int cols,
    Position start,
    Position* destinations,
    int destinationCount,
    int* visitOrder,
    Position* totalPath,
    size_t totalPathCapacity,
    size_t* totalPathLength,
    int* segmentCosts,
    long long* totalCost
)
{
    Position current;
    Position target;
    Position* segmentPath;
    PriorityQueue* queue;

    int segmentPathLength;
    int pathResult;
    int orderIndex;
    int pathIndex;
    int startPathIndex;

    segmentPath = (Position*)malloc(
        (size_t)rows *
        (size_t)cols *
        sizeof(Position)
    );

    if (segmentPath == NULL)
    {
        return PATH_ERROR;
    }

    current = start;
    *totalPathLength = 0;
    *totalCost = 0;

    for (
        orderIndex = 0;
        orderIndex < destinationCount;
        orderIndex++
        )
    {
        target =
            destinations[visitOrder[orderIndex]];

        initializeAStarNodes(
            nodeMap,
            rows,
            cols
        );

        queue = NULL;

        pathResult = findPath(
            map,
            nodeMap,
            rows,
            cols,
            current,
            target,
            segmentPath,
            &segmentPathLength,
            &queue
        );

        if (pathResult != PATH_FOUND)
        {
            freePriorityQueue(queue);
            free(segmentPath);

            return pathResult;
        }

        segmentCosts[orderIndex] =
            nodeMap[target.row][target.col].gCost;

        *totalCost +=
            segmentCosts[orderIndex];

        /*
         * 두 번째 구간부터는 이전 배송지가 첫 좌표로 다시 들어온다.
         * 연결점이 중복되지 않도록 첫 좌표를 건너뛴다.
         */
        if (orderIndex == 0)
        {
            startPathIndex = 0;
        }
        else
        {
            startPathIndex = 1;
        }

        for (
            pathIndex = startPathIndex;
            pathIndex < segmentPathLength;
            pathIndex++
            )
        {
            if (*totalPathLength >= totalPathCapacity)
            {
                freePriorityQueue(queue);
                free(segmentPath);

                return PATH_ERROR;
            }

            totalPath[*totalPathLength] =
                segmentPath[pathIndex];

            (*totalPathLength)++;
        }

        current = target;

        freePriorityQueue(queue);
    }

    free(segmentPath);

    return PATH_FOUND;
}

/*
 * 최적 배송 방문 순서, 각 구간 비용, 총비용과 전체 좌표 경로를 출력한다.
 */
void printDeliveryResult(
    Position start,
    Position* destinations,
    int destinationCount,
    int* visitOrder,
    int* segmentCosts,
    long long totalCost,
    Position* totalPath,
    size_t totalPathLength
)
{
    int orderIndex;
    int destinationIndex;
    size_t pathIndex;

    printf("\n출발지: (%d, %d)\n", start.row, start.col);
    printf("배송지 개수: %d개\n", destinationCount);

    printf("\n최적 배송 방문 순서:\n");

    for (
        orderIndex = 0;
        orderIndex < destinationCount;
        orderIndex++
        )
    {
        destinationIndex =
            visitOrder[orderIndex];

        printf(
            "%d번째 방문: 배송지 %d (%d, %d), 구간 비용 %d\n",
            orderIndex + 1,
            destinationIndex + 1,
            destinations[destinationIndex].row,
            destinations[destinationIndex].col,
            segmentCosts[orderIndex]
        );
    }

    printf("\n총 이동 비용: %lld\n", totalCost);

    if (totalPathLength > 0)
    {
        printf(
            "전체 경로 좌표 개수: %llu개\n",
            (unsigned long long)totalPathLength
        );

        printf(
            "총 이동 횟수: %llu회\n",
            (unsigned long long)(totalPathLength - 1)
        );
    }

    printf("\n전체 배송 경로:\n");

    for (
        pathIndex = 0;
        pathIndex < totalPathLength;
        pathIndex++
        )
    {
        printf(
            "(%d, %d)",
            totalPath[pathIndex].row,
            totalPath[pathIndex].col
        );

        if (pathIndex + 1 < totalPathLength)
        {
            printf(" -> ");
        }
    }

    printf("\n");
}

void freePriorityQueue(
    PriorityQueue* queue
)
{
    if (queue == NULL)
    {
        return;
    }

    free(queue->items);
    free(queue);
}

void freeMap(
    Map** map,
    int rows
)
{
    int i;

    if (map == NULL)
    {
        return;
    }

    for (i = 0; i < rows; i++)
    {
        free(map[i]);
    }

    free(map);
}

void freeNodeMap(
    AStarNode** nodeMap,
    int rows
)
{
    int i;

    if (nodeMap == NULL)
    {
        return;
    }

    for (i = 0; i < rows; i++)
    {
        free(nodeMap[i]);
    }

    free(nodeMap);
}


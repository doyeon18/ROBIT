# 로봇 데이터 작성
robot_status = [
    {"name": "mobilebot", "battery": 82,
     "position": (1.2, 0.5), "distance": 0.8},

    {"name": "drone", "battery": 18,
     "position": (0.3, 1.5), "distance": 0.4},

    {"name": "manipulator", "battery": 45,
     "position": (2.0, 1.0), "distance": 1.2},
]

# 배터리 상태 판정 함수 작성
def get_battery_status(battery):
    if battery >= 60:
        return "배터리 충분"
    elif battery >= 20:
        return "배터리 주의"
    else:
        return "충전 필요" 
    
# 장애물 감지 함수 작성
def get_movement_status(distance):
    if distance < 0.5:
        return "장애물 감지"
    else:
        return "전진 가능"

# 로봇 한 대를 출력하는 함수
def print_robot_info(robot):
    name = robot["name"]
    battery = robot["battery"]
    position =  robot["position"]
    distance = robot["distance"]

    # 튜플 언패킹
    x, y = position

    battery_status = get_battery_status(battery)
    movement_status = get_movement_status(distance)

    print(f"[{name}]")
    print(f"배터리: {battery_status}")
    print(f"위치: x={x}, y={y}")
    print(f"상태: {movement_status}")

# 전체 로봇 데이터를 반복하여 처리하는 함수
def main():
    for robot in robot_status:
        print_robot_info(robot)
        print()

main()
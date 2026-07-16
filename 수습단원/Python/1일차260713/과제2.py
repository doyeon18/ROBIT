'''
문제 발생 오류 원인 
1. 문자열과 정수를 +로 합쳐서 TypeError가 발생하였다.
2. 기본값으로 작성한 logs=[]가 함수 호출마다 같은 리스트를 사용해서 로그가 누적되었다.
'''

# 로봇 이름과 배터리 값을 로그 문자열로 만드는 함수
def make_log(robot_name, battery):
    ''' 
    f-string을 사용하면 문자열 안에 정수 값을 문자열 형태로 표현할 수 있다.
    따라서 문자열과 정수를 + 연산자로 직접 연결할 때 발생하는 TypeError를 해결할 수 있다.
    '''
    log = f"{robot_name} battery: {battery}"

    return log


# 만들어진 로그를 리스트에 추가하는 함수
def add_log(robot_name, battery, logs=None):
    '''
    logs=[]를 기본값으로 사용하면 같은 리스트가 계속 재사용된다.
    리스트는 mutable 자료형이기 때문에 이전 함수 호출의 값이 남아 있게 된다.
    따라서 기본값을 None으로 설정한 뒤 함수 내부에서 새 리스트를 만든다.
    '''
    if logs is None:
        logs = []

    log = make_log(robot_name, battery)
    logs.append(log)

    return logs


# 프로그램 실행을 담당하는 함수
def main():
    print(add_log("frontbot", 80))
    print(add_log("rearbot", 50))
    print(add_log("armbot", 20))


main()
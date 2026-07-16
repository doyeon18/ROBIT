import numpy as np
# 사용한 NumPy 버전: 2.5.1

def input_scores():
    """사용자에게 점수를 입력받아 리스트로 반환한다."""

    scores = []

    print("학생들의 점수를 입력하세요.")
    print("입력을 끝내려면 q를 입력하세요.")

    while True:
        value = input("점수: ").strip()

        # q를 입력하면 점수 입력 종료
        if value.lower() == "q":
            break

        # 입력값을 정수로 변환
        try:
            score = int(value)

        except ValueError:
            print(
                f"잘못된 입력입니다. "
                f"오류 원인: 점수 '{value}'를 정수로 변환할 수 없습니다."
            )
            continue

        # 점수 범위 검사
        if not 0 <= score <= 100:
            print(
                "잘못된 입력입니다. "
                "오류 원인: 점수는 0부터 100 사이여야 합니다."
            )
            continue

        # 정상 점수만 리스트에 추가
        scores.append(score)

    return scores


def calculate_statistics(scores):
    """NumPy를 사용해 점수 통계를 계산한다."""

    # 파이썬 리스트를 NumPy 배열로 변환
    score_array = np.array(scores)

    statistics = {
        "count": score_array.size,
        "average": round(float(score_array.mean()), 2),
        "highest": int(score_array.max()),
        "lowest": int(score_array.min()),
        "standard_deviation": round(float(score_array.std()), 2)
    }

    return statistics


def print_statistics(statistics):
    """계산된 통계를 화면에 출력한다."""

    print()
    print("===== 성적 통계 =====")
    print(f"인원수: {statistics['count']}명")
    print(f"평균 점수: {statistics['average']}점")
    print(f"최고 점수: {statistics['highest']}점")
    print(f"최저 점수: {statistics['lowest']}점")
    print(f"표준편차: {statistics['standard_deviation']}")


def main():
    # 사용자에게 점수를 입력받기
    scores = input_scores()

    # 입력된 점수가 없는 경우 종료
    if not scores:
        print("입력된 점수가 없습니다.")
        return

    # NumPy로 통계 계산
    statistics = calculate_statistics(scores)

    # 결과 출력
    print_statistics(statistics)

    print()
    print(f"사용한 NumPy 버전: {np.__version__}")


if __name__ == "__main__":
    main()
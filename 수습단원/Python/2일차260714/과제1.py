import csv
import json
from pathlib import Path


# 현재 파이썬 파일이 저장된 폴더
BASE_DIR = Path(__file__).resolve().parent

INPUT_FILE = BASE_DIR / "students.csv"
OUTPUT_CSV = BASE_DIR / "clean_students.csv"
OUTPUT_JSON = BASE_DIR / "summary.json"

# 유효한 학생 정보를 저장할 리스트
valid_students = []

try:
    # 원본 CSV 파일 읽기
    with open(
        INPUT_FILE,
        "r",
        encoding="utf-8",
        newline=""
    ) as file:

        reader = csv.reader(file)

        # 첫 번째 행인 name, score 건너뛰기
        next(reader, None)

        # 학생 데이터를 2행부터 한 줄씩 읽기
        for line_number, row in enumerate(reader, start=2):

            try:
                # 이름과 점수, 총 2개의 열이 있는지 검사
                if len(row) != 2:
                    raise ValueError("열의 개수가 2개가 아닙니다.")

                name = row[0].strip()
                score_text = row[1].strip()

                # 이름이 비어 있는지 검사
                if not name:
                    raise ValueError("이름이 비어 있습니다.")

                # 점수가 비어 있는지 검사
                if not score_text:
                    raise ValueError("점수가 비어 있습니다.")

                # 문자열 점수를 정수로 변환
                try:
                    score = int(score_text)

                except ValueError:
                    raise ValueError(
                        f"점수 '{score_text}'를 정수로 변환할 수 없습니다."
                    )

                # 점수가 0~100 범위인지 검사
                if not 0 <= score <= 100:
                    raise ValueError(
                        f"점수 {score}가 0~100 범위를 벗어났습니다."
                    )

            except ValueError as error:
                # 잘못된 행은 이유를 출력하고 다음 행으로 이동
                print(
                    f"{line_number}행 오류: {row}, "
                    f"오류 원인: {error}"
                )
                continue

            # 유효한 학생만 리스트에 저장
            valid_students.append([name, score])

except FileNotFoundError:
    print(f"{INPUT_FILE} 파일을 찾을 수 없습니다.")

else:
    # 유효한 학생을 새로운 CSV 파일에 저장
    with open(
        OUTPUT_CSV,
        "w",
        encoding="utf-8",
        newline=""
    ) as file:

        writer = csv.writer(file)

        # 첫 번째 행에 열 이름 작성
        writer.writerow(["name", "score"])

        # 유효한 학생 정보 작성
        writer.writerows(valid_students)

    # 학생들의 점수만 가져오기
    scores = [student[1] for student in valid_students]

    # 통계 정보 계산
    if scores:
        summary = {
            "count": len(scores),
            "average": round(sum(scores) / len(scores), 2),
            "highest_score": max(scores)
        }

    else:
        summary = {
            "count": 0,
            "average": 0,
            "highest_score": 0
        }

    # 통계 정보를 JSON 파일로 저장
    with open(
        OUTPUT_JSON,
        "w",
        encoding="utf-8"
    ) as file:

        json.dump(
            summary,
            file,
            ensure_ascii=False,
            indent=2
        )

    print()
    print("처리가 완료되었습니다.")
    print(f"유효한 학생 수: {summary['count']}명")
    print(f"평균 점수: {summary['average']}")
    print(f"최고 점수: {summary['highest_score']}")
    print(f"{OUTPUT_CSV}와 {OUTPUT_JSON}이 생성되었습니다.")
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

        reader = csv.DictReader(file)

        # CSV 파일에 헤더가 있는지 검사
        if reader.fieldnames is None:
            raise ValueError("CSV 파일에 헤더가 없습니다.")

        # 헤더 앞뒤의 불필요한 공백 제거
        reader.fieldnames = [
            field.strip() if field is not None else ""
            for field in reader.fieldnames
        ]

        # name과 score 열이 있는지 검사
        if "name" not in reader.fieldnames:
            raise ValueError("헤더에 name 열이 없습니다.")

        if "score" not in reader.fieldnames:
            raise ValueError("헤더에 score 열이 없습니다.")

        # 학생 데이터를 2행부터 한 줄씩 읽기
        for line_number, row in enumerate(reader, start=2):

            try:
                # 헤더보다 데이터 열이 더 많은 경우
                if None in row:
                    raise ValueError("열의 개수가 헤더보다 많습니다.")

                # 딕셔너리에서 열 이름으로 값 가져오기
                name = (row.get("name") or "").strip()
                score_text = (row.get("score") or "").strip()

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
                    f"{line_number}행 오류: "
                    f"name={row.get('name')}, "
                    f"score={row.get('score')} "
                    f"오류 원인: {error}"
                )
                continue

            # 유효한 학생은 딕셔너리로 저장
            valid_students.append({
                "name": name,
                "score": score
            })

except FileNotFoundError:
    print(f"{INPUT_FILE} 파일을 찾을 수 없습니다.")

except ValueError as error:
    print(f"CSV 형식 오류: {error}")

else:
    # 유효한 학생을 새로운 CSV 파일에 저장
    with open(
        OUTPUT_CSV,
        "w",
        encoding="utf-8",
        newline=""
    ) as file:

        writer = csv.DictWriter(
            file,
            fieldnames=["name", "score"]
        )

        # 첫 번째 행에 name,score 작성
        writer.writeheader()

        # 유효한 학생 정보 작성
        writer.writerows(valid_students)

    # 학생들의 점수만 가져오기
    scores = [
        student["score"]
        for student in valid_students
    ]

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
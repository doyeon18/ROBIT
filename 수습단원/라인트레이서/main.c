/*
 * LineTracer
 *
 * Sequence0 : S자
 * Sequence1 : 8자
 * Sequence2 : 평행사변형
 * Sequence3 : 차단바
 * Sequence4 : 주차
 * Sequence5 : 검정 바탕 / 흰 선
 * Sequence6 : 벽돌 / PSD
 *
 * ATmega128
 *
 * IR
 * IR1 = PF2 / ADC2
 * IR2 = PF3 / ADC3
 * IR3 = PF4 / ADC4
 * IR4 = PF5 / ADC5
 * IR5 = PF6 / ADC6
 * IR6 = PF7 / ADC7
 *
 * PSD = PF1 / ADC1
 *
 * SW1 = PE4
 */

#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdint.h>


/* =========================================================
   기본 설정
   ========================================================= */

#define IR_NUM              6

#define BASE_SPEED          40
#define PID_LIMIT           30

#define PIVOT_SPEED         50


/* =========================================================
   시작 스위치
   ========================================================= */

#define START_SWITCH        PE4


/* =========================================================
   PSD / 차단바
   ========================================================= */

#define PSD_CHANNEL         1

#define BARRIER_OPEN_DIFF   100
#define BARRIER_OPEN_COUNT  5


/* =========================================================
   평행사변형
   ========================================================= */

#define LANE_TURN_SPEED         55
#define LANE_BASE_BIAS          7

#define LANE_HIT_BACK_SPEED     50
#define LANE_HIT_BACK_MS        15

#define LANE_TURN_MS            100

#define LANE_EXIT_TIMER_COUNT   180

#define LANE_EXIT_TURN_MS       350
#define LANE_EXIT_STRAIGHT_MS   200
#define LANE_EXIT_EXTRA_STRAIGHT_MS 100


/* =========================================================
   주차
   ========================================================= */

#define PARKING_LEFT_TRIM           2

#define PARKING_TURN_SPEED          60
#define PARKING_FIRST_TURN_MS       100
#define PARKING_180_IGNORE_MS       1000
#define PARKING_TURN_STABLE_COUNT   3
#define PARKING_180_LEFT_BOOST      10


/* =========================================================
   검정구간 Sequence5

   성공 코드의 기본 구조를 최대한 사용

   일반선:
   Black PID로 다음 이벤트까지 이동

   교차로:
   무조건 직진

   일반 좌/우회전:
   300ms 진입 후
   속도 45로 750ms 피봇

   급우회전 C6:
   성공 코드처럼 420ms 깊게 진입 후
   동일한 우피봇 사용
   ========================================================= */

#define BLACK_EVENT_IR_COUNT          3

#define BLACK_TURN_SPEED              45
#define BLACK_TURN_ENTRY_MS           370
#define BLACK_TURN_90_MS              800

#define BLACK_SHARP_ENTRY_MS          450
#define BLACK_SHARP_TURN_MS          1100
#define BLACK_C1_LOCKOUT_COUNT        100   // 100 x 10ms = 약 1초
#define BLACK_SHARP_LOCKOUT_COUNT     200   // 200 x 10ms = 약 2초
#define BLACK_AFTER_SHARP_LOCKOUT_COUNT 200 // 200 x 10ms = 약 2초
#define BLACK_FINAL_TURN_LOCKOUT_COUNT 200  // 200 x 10ms = 약 2초

#define BLACK_STRAIGHT_LEFT_SPEED     35
#define BLACK_STRAIGHT_RIGHT_SPEED    40

#define BLACK_FINAL_STRAIGHT_COUNT    20
#define BLACK_FINAL_STABLE_COUNT      5


/* =========================================================
   벽돌 구간 Sequence6
   ========================================================= */

#define BRICK_PSD_PEAK_THRESHOLD      500
#define BRICK_PSD_STOP_THRESHOLD      400

#define BRICK_STOP_WAIT_MS            3000

#define BRICK_LEFT_TURN_MS            720
#define BRICK_RIGHT_ENTRY_MS          300


/* =========================================================
   흰 바탕 / 검은 선 Threshold
   ========================================================= */

uint16_t threshold[IR_NUM] =
{
    590,
    585,
    570,
    580,
    570,
    570
};


/* =========================================================
   검정 바탕 / 흰 선 Threshold

   네 실측값 기반
   ========================================================= */

uint16_t blackThreshold[IR_NUM] =
{
    150,
    180,
    180,
    160,
    150,
    110
};


/* =========================================================
   IR 변수
   ========================================================= */

uint16_t irRaw[IR_NUM] =
{
    0, 0, 0, 0, 0, 0
};


uint8_t irState[IR_NUM] =
{
    0, 0, 0, 0, 0, 0
};


uint16_t irNormalized[IR_NUM] =
{
    0, 0, 0, 0, 0, 0
};


uint8_t irCount = 0;


/* =========================================================
   PID
   ========================================================= */

const int IR_WEIGHT[IR_NUM] =
{
    -19,
    -10,
    -3,
     3,
     10,
     19
};


int pidPrevError = 0;


/* =========================================================
   평행사변형 상태
   ========================================================= */

int8_t laneLastWall = 0;

uint8_t laneTurnPending = 0;

volatile uint16_t laneTimerCount = 0;


/* =========================================================
   함수 선언
   ========================================================= */

void initSystem(void);

void jtagDisable(void);

void adcInit(void);

void ledInit(void);

void motorInit(void);

void switchInit(void);

void waitStart(void);


uint16_t adcRead(uint8_t channel);


void updateIR(void);

void updateIRBlack(void);

void updateLED(void);


void setLeftSpeed(int speed);

void setRightSpeed(int speed);


void motorDrive(
    int leftSpeed,
    int rightSpeed
);


void motorStop(void);

void motorBrake(void);


int calculatePID(
    int Kp,
    int Ki,
    int Kd
);


void followLine(
    int Kp,
    int Ki,
    int Kd
);


void followLineParking(
    int Kp,
    int Ki,
    int Kd
);


void followLineBlack(
    int Kp,
    int Ki,
    int Kd
);


void followLineUntilAtLeast(
    int Kp,
    int Ki,
    int Kd,
    uint8_t targetCount,
    uint16_t minTime
);


void followLineUntilAtMost(
    int Kp,
    int Ki,
    int Kd,
    uint8_t targetCount,
    uint16_t minTime
);


void parkingFollowUntilAtLeast(
    int Kp,
    int Ki,
    int Kd,
    uint8_t targetCount,
    uint16_t minTime
);


void parkingFollowUntilAtMost(
    int Kp,
    int Ki,
    int Kd,
    uint8_t targetCount,
    uint16_t minTime
);


void blackMoveToEvent(
    int Kp,
    int Ki,
    int Kd,
    uint8_t targetCount,
    uint16_t minCount
);


void blackPassIntersection(
    uint16_t inCount,
    uint16_t outCount
);


void blackTurnLeft90(
    uint16_t entryMs
);


void blackTurnRight90(
    uint16_t entryMs
);


void blackTurnRightSharp(
    uint16_t entryMs
);


void blackFollowStraightAndStop(
    int Kp,
    int Ki,
    int Kd,
    uint16_t minCount
);


void brickApproachByPSD(void);

void brickTurnLeft90(void);


void delayMs(uint16_t ms);


void pivotLeftUntilLine(int speed);

void pivotLeft180UntilLine(int speed);


void laneTimerStart(void);

void laneTimerStop(void);

uint16_t getLaneTimerCount(void);


char detectLaneWall(void);

char laneCarlineStep(void);


void Sequence0_S(void);

void Sequence1_Eight(void);

void Sequence2_Lane(void);

void Sequence3_Barrier(void);

void Sequence4_Parking(void);

void Sequence5_Black(void);

void Sequence6_Brick(void);


/* =========================================================
   Timer3 ISR
   ========================================================= */

ISR(TIMER3_COMPA_vect)
{
    laneTimerCount++;
}


/* =========================================================
   MAIN
   ========================================================= */

int main(void)
{
    initSystem();


    waitStart();


    Sequence0_S();


    Sequence1_Eight();


    Sequence2_Lane();


    Sequence3_Barrier();


    Sequence4_Parking();


    Sequence5_Black();


    Sequence6_Brick();


    while (1)
    {
        motorStop();

        _delay_ms(10);
    }
}


/* =========================================================
   전체 초기화
   ========================================================= */

void initSystem(void)
{
    jtagDisable();

    adcInit();

    ledInit();

    motorInit();

    switchInit();


    motorStop();
}


/* =========================================================
   시작 스위치
   ========================================================= */

void switchInit(void)
{
    DDRE &=
        ~(1 << START_SWITCH);


    PORTE |=
        (1 << START_SWITCH);
}


/* =========================================================
   시작 대기
   ========================================================= */

void waitStart(void)
{
    uint8_t pressCount = 0;


    motorStop();


    while (1)
    {
        updateIR();


        if (
            (PINE & (1 << START_SWITCH))
            == 0
        )
        {
            pressCount++;


            if (
                pressCount >= 3
            )
            {
                break;
            }
        }

        else
        {
            pressCount = 0;
        }


        motorStop();


        _delay_ms(10);
    }


    while (
        (PINE & (1 << START_SWITCH))
        == 0
    )
    {
        updateIR();

        motorStop();

        _delay_ms(10);
    }


    _delay_ms(20);
}


/* =========================================================
   JTAG 비활성화
   ========================================================= */

void jtagDisable(void)
{
    MCUCSR |=
        (1 << JTD);


    MCUCSR |=
        (1 << JTD);
}


/* =========================================================
   ADC 초기화
   ========================================================= */

void adcInit(void)
{
    DDRF &=
        ~0xFE;


    PORTF &=
        ~0xFE;


    ADMUX =
        (1 << REFS0);


    ADCSRA =
        (1 << ADEN) |
        (1 << ADPS2) |
        (1 << ADPS1) |
        (1 << ADPS0);
}


/* =========================================================
   ADC 읽기
   ========================================================= */

uint16_t adcRead(uint8_t channel)
{
    ADMUX =
        (1 << REFS0) |
        (channel & 0x07);


    ADCSRA |=
        (1 << ADSC);


    while (
        ADCSRA &
        (1 << ADSC)
    )
    {
    }


    return ADC;
}


/* =========================================================
   LED 초기화
   ========================================================= */

void ledInit(void)
{
    DDRA |=
        0x3F;


    PORTA |=
        0x3F;
}


/* =========================================================
   LED 업데이트
   ========================================================= */

void updateLED(void)
{
    for (
        uint8_t i = 0;
        i < IR_NUM;
        i++
    )
    {
        if (
            irState[i] == 0
        )
        {
            PORTA &=
                ~(1 << i);
        }

        else
        {
            PORTA |=
                (1 << i);
        }
    }
}


/* =========================================================
   일반 IR

   흰 바탕 / 검은 선
   ========================================================= */

void updateIR(void)
{
    irRaw[0] = adcRead(2);

    irRaw[1] = adcRead(3);

    irRaw[2] = adcRead(4);

    irRaw[3] = adcRead(5);

    irRaw[4] = adcRead(6);

    irRaw[5] = adcRead(7);


    irCount = 0;


    for (
        uint8_t i = 0;
        i < IR_NUM;
        i++
    )
    {
        if (
            irRaw[i] <
            threshold[i]
        )
        {
            irState[i] = 1;

            irNormalized[i] = 0;

            irCount++;
        }

        else
        {
            irState[i] = 0;

            irNormalized[i] = 1000;
        }
    }


    updateLED();
}


/* =========================================================
   검정 구간 IR

   검정 바탕 / 흰 선

   ADC > threshold
   → 흰 선
   ========================================================= */

void updateIRBlack(void)
{
    irRaw[0] = adcRead(2);

    irRaw[1] = adcRead(3);

    irRaw[2] = adcRead(4);

    irRaw[3] = adcRead(5);

    irRaw[4] = adcRead(6);

    irRaw[5] = adcRead(7);


    irCount = 0;


    for (
        uint8_t i = 0;
        i < IR_NUM;
        i++
    )
    {
        if (
            irRaw[i] >
            blackThreshold[i]
        )
        {
            irState[i] = 1;

            irNormalized[i] = 0;

            irCount++;
        }

        else
        {
            irState[i] = 0;

            irNormalized[i] = 1000;
        }
    }


    updateLED();
}


/* =========================================================
   모터 초기화
   ========================================================= */

void motorInit(void)
{
    DDRB |=
        (1 << PB0) |
        (1 << PB1) |
        (1 << PB2) |
        (1 << PB3) |
        (1 << PB5) |
        (1 << PB6);


    TCCR1A =
        (1 << COM1A1) |
        (1 << COM1B1) |
        (1 << WGM10);


    TCCR1B =
        (1 << WGM12) |
        (1 << CS11) |
        (1 << CS10);


    OCR1A = 0;

    OCR1B = 0;
}


/* =========================================================
   왼쪽 모터 PWM
   ========================================================= */

void setLeftSpeed(int speed)
{
    if (
        speed < 0
    )
    {
        speed =
            -speed;
    }


    if (
        speed > 100
    )
    {
        speed =
            100;
    }


    OCR1A =
        (uint8_t)
        (
            ((long)speed * 255)
            / 100
        );
}


/* =========================================================
   오른쪽 모터 PWM
   ========================================================= */

void setRightSpeed(int speed)
{
    if (
        speed < 0
    )
    {
        speed =
            -speed;
    }


    if (
        speed > 100
    )
    {
        speed =
            100;
    }


    OCR1B =
        (uint8_t)
        (
            ((long)speed * 255)
            / 100
        );
}


/* =========================================================
   모터 통합 제어
   ========================================================= */

void motorDrive(
    int leftSpeed,
    int rightSpeed
)
{
    if (
        leftSpeed > 100
    )
    {
        leftSpeed = 100;
    }


    if (
        leftSpeed < -100
    )
    {
        leftSpeed = -100;
    }


    if (
        rightSpeed > 100
    )
    {
        rightSpeed = 100;
    }


    if (
        rightSpeed < -100
    )
    {
        rightSpeed = -100;
    }


    /* 왼쪽 */

    if (
        leftSpeed > 0
    )
    {
        PORTB |=
            (1 << PB0);


        PORTB &=
            ~(1 << PB1);
    }

    else if (
        leftSpeed < 0
    )
    {
        PORTB &=
            ~(1 << PB0);


        PORTB |=
            (1 << PB1);
    }

    else
    {
        PORTB &=
            ~(1 << PB0);


        PORTB &=
            ~(1 << PB1);
    }


    /* 오른쪽 */

    if (
        rightSpeed > 0
    )
    {
        PORTB &=
            ~(1 << PB2);


        PORTB |=
            (1 << PB3);
    }

    else if (
        rightSpeed < 0
    )
    {
        PORTB |=
            (1 << PB2);


        PORTB &=
            ~(1 << PB3);
    }

    else
    {
        PORTB &=
            ~(1 << PB2);


        PORTB &=
            ~(1 << PB3);
    }


    setLeftSpeed(
        leftSpeed
    );


    setRightSpeed(
        rightSpeed
    );
}


/* =========================================================
   모터 정지
   ========================================================= */

void motorStop(void)
{
    OCR1A = 0;

    OCR1B = 0;


    PORTB &=
        ~(
            (1 << PB0) |
            (1 << PB1) |
            (1 << PB2) |
            (1 << PB3)
        );
}


/* =========================================================
   모터 브레이크
   ========================================================= */

void motorBrake(void)
{
    PORTB &=
        ~(
            (1 << PB0) |
            (1 << PB1) |
            (1 << PB2) |
            (1 << PB3)
        );


    setLeftSpeed(
        100
    );


    setRightSpeed(
        100
    );
}


/* =========================================================
   PID
   ========================================================= */

int calculatePID(
    int Kp,
    int Ki,
    int Kd
)
{
    long weightedSum = 0;

    long total = 0;

    int error;

    int derivative;

    int output;

    static long integral = 0;


    for (
        uint8_t i = 0;
        i < IR_NUM;
        i++
    )
    {
        int lineValue;


        lineValue =
            1000 -
            irNormalized[i];


        weightedSum +=
            (long)lineValue *
            IR_WEIGHT[i];


        total +=
            lineValue;
    }


    if (
        total > 0
    )
    {
        error =
            (int)
            (
                weightedSum /
                total
            );
    }

    else
    {
        error =
            pidPrevError;
    }


    if (
        Ki != 0
    )
    {
        integral +=
            error;


        if (
            integral > 1000
        )
        {
            integral = 1000;
        }


        if (
            integral < -1000
        )
        {
            integral = -1000;
        }
    }

    else
    {
        integral = 0;
    }


    derivative =
        error -
        pidPrevError;


    output =
        (int)
        (
            (
                (long)Kp * error
                +
                (long)Ki * integral
                +
                (long)Kd * derivative
            )
            / 100
        );


    pidPrevError =
        error;


    if (
        output > PID_LIMIT
    )
    {
        output =
            PID_LIMIT;
    }


    if (
        output < -PID_LIMIT
    )
    {
        output =
            -PID_LIMIT;
    }


    return output;
}


/* =========================================================
   기본 PID
   ========================================================= */

void followLine(
    int Kp,
    int Ki,
    int Kd
)
{
    int correction;


    updateIR();


    correction =
        calculatePID(
            Kp,
            Ki,
            Kd
        );


    motorDrive(
        BASE_SPEED + correction,
        BASE_SPEED - correction
    );
}


/* =========================================================
   주차 PID
   ========================================================= */

void followLineParking(
    int Kp,
    int Ki,
    int Kd
)
{
    int correction;


    updateIR();


    correction =
        calculatePID(
            Kp,
            Ki,
            Kd
        );


    motorDrive(
        BASE_SPEED
        - PARKING_LEFT_TRIM
        + correction,

        BASE_SPEED
        - correction
    );
}


/* =========================================================
   Black PID
   ========================================================= */

void followLineBlack(
    int Kp,
    int Ki,
    int Kd
)
{
    int correction;


    updateIRBlack();


    correction =
        calculatePID(
            Kp,
            Ki,
            Kd
        );


    motorDrive(
        BASE_SPEED + correction,
        BASE_SPEED - correction
    );
}


/* =========================================================
   기본 PID
   IR >= target
   ========================================================= */

void followLineUntilAtLeast(
    int Kp,
    int Ki,
    int Kd,
    uint8_t targetCount,
    uint16_t minTime
)
{
    uint16_t elapsedTime = 0;


    while (1)
    {
        followLine(
            Kp,
            Ki,
            Kd
        );


        _delay_ms(10);


        elapsedTime +=
            10;


        if (
            elapsedTime >= minTime
            &&
            irCount >= targetCount
        )
        {
            break;
        }
    }
}


/* =========================================================
   기본 PID
   IR <= target
   ========================================================= */

void followLineUntilAtMost(
    int Kp,
    int Ki,
    int Kd,
    uint8_t targetCount,
    uint16_t minTime
)
{
    uint16_t elapsedTime = 0;


    while (1)
    {
        followLine(
            Kp,
            Ki,
            Kd
        );


        _delay_ms(10);


        elapsedTime +=
            10;


        if (
            elapsedTime >= minTime
            &&
            irCount <= targetCount
        )
        {
            break;
        }
    }
}


/* =========================================================
   주차 PID
   IR >= target
   ========================================================= */

void parkingFollowUntilAtLeast(
    int Kp,
    int Ki,
    int Kd,
    uint8_t targetCount,
    uint16_t minTime
)
{
    uint16_t elapsedTime = 0;


    while (1)
    {
        followLineParking(
            Kp,
            Ki,
            Kd
        );


        _delay_ms(10);


        elapsedTime +=
            10;


        if (
            elapsedTime >= minTime
            &&
            irCount >= targetCount
        )
        {
            break;
        }
    }
}


/* =========================================================
   주차 PID
   IR <= target
   ========================================================= */

void parkingFollowUntilAtMost(
    int Kp,
    int Ki,
    int Kd,
    uint8_t targetCount,
    uint16_t minTime
)
{
    uint16_t elapsedTime = 0;


    while (1)
    {
        followLineParking(
            Kp,
            Ki,
            Kd
        );


        _delay_ms(10);


        elapsedTime +=
            10;


        if (
            elapsedTime >= minTime
            &&
            irCount <= targetCount
        )
        {
            break;
        }
    }
}


/* =========================================================
   검정구간 일반선

   성공 코드 Mov_N_PID_B 대응

   Black PID로 주행하다가
   최소 minCount * 10ms가 지난 뒤
   IR 개수가 targetCount 이상이면
   다음 이벤트에 도착한 것으로 판단

   왼쪽/오른쪽 방향은 여기서 판단하지 않음
   다음 행동은 Sequence5의 호출 순서가 결정
   ========================================================= */

void blackMoveToEvent(
    int Kp,
    int Ki,
    int Kd,
    uint8_t targetCount,
    uint16_t minCount
)
{
    uint16_t count = 0;


    while (1)
    {
        followLineBlack(
            Kp,
            Ki,
            Kd
        );


        _delay_ms(10);


        count++;


        if (
            count > minCount
            &&
            irCount >= targetCount
        )
        {
            break;
        }
    }
}


/* =========================================================
   검정구간 교차로 직진

   성공 코드 Mov_EG_PID_B의
   진입 / 진출 2단계 틀을 사용

   차이점:
   교차로에서는 PID가 가지를 선택하지 않도록
   L = 35 / R = 40으로 직진을 강제

   inCount:
   교차로 안으로 최소 진행할 카운트

   outCount:
   진출 단계에서 최소 진행할 카운트
   ========================================================= */

void blackPassIntersection(
    uint16_t inCount,
    uint16_t outCount
)
{
    uint16_t count = 0;

    uint8_t intersectionSeen = 0;


    /*
     * blackMoveToEvent()가 끝난 직후라면
     * 이미 넓은 선 위에 있을 수 있음
     */
    if (
        irCount >=
        BLACK_EVENT_IR_COUNT
    )
    {
        intersectionSeen = 1;
    }


    /* =====================================================
       교차로 진입
       ===================================================== */

    while (1)
    {
        updateIRBlack();


        motorDrive(
            BLACK_STRAIGHT_LEFT_SPEED,
            BLACK_STRAIGHT_RIGHT_SPEED
        );


        _delay_ms(10);


        count++;


        if (
            irCount >=
            BLACK_EVENT_IR_COUNT
        )
        {
            intersectionSeen = 1;
        }


        if (
            intersectionSeen
            &&
            count > inCount
        )
        {
            break;
        }
    }


    /* =====================================================
       교차로 진출

       성공 코드처럼 IR 2개 이하가 되고
       outCount * 10ms 이상 진행했을 때 종료

       중앙선 재획득을 강제하지 않음
       교차로가 코너와 가까운 경우의 과도한 대기 방지
       ===================================================== */

    count = 0;


    while (1)
    {
        updateIRBlack();


        motorDrive(
            BLACK_STRAIGHT_LEFT_SPEED,
            BLACK_STRAIGHT_RIGHT_SPEED
        );


        _delay_ms(10);


        count++;


        if (
            count > outCount
            &&
            irCount <= 2
        )
        {
            break;
        }
    }


    pidPrevError = 0;
}


/* =========================================================
   검정구간 좌 90도

   성공 코드 Mov_EG_M_Plus_B 방식

   1. entryMs 동안 교차점/코너 안쪽으로 직진
   2. 정지
   3. L -45 / R +45로 750ms 피봇
   4. 센서 재획득 조건 없이 시간으로 회전 종료

   회전 중 바로 다음 교차로가 IR에 잡혀도
   회전 종료조건에 영향을 주지 않음
   ========================================================= */

void blackTurnLeft90(
    uint16_t entryMs
)
{
    motorDrive(
        BLACK_STRAIGHT_LEFT_SPEED,
        BLACK_STRAIGHT_RIGHT_SPEED
    );


    delayMs(
        entryMs
    );


    motorStop();


    delayMs(
        5
    );


    motorDrive(
        -BLACK_TURN_SPEED,
         BLACK_TURN_SPEED
    );


    delayMs(
        BLACK_TURN_90_MS
    );


    motorStop();


    delayMs(
        20
    );


    pidPrevError = 0;
}


/* =========================================================
   검정구간 우 90도

   좌90과 동일하고 회전 방향만 반대
   ========================================================= */

void blackTurnRight90(
    uint16_t entryMs
)
{
    motorDrive(
        BLACK_STRAIGHT_LEFT_SPEED,
        BLACK_STRAIGHT_RIGHT_SPEED
    );


    delayMs(
        entryMs
    );


    motorStop();


    delayMs(
        5
    );


    motorDrive(
         BLACK_TURN_SPEED,
        -BLACK_TURN_SPEED
    );


    delayMs(
        BLACK_TURN_90_MS
    );


    motorStop();


    delayMs(
        20
    );


    pidPrevError = 0;
}


/* =========================================================
   검정구간 급우회전

   성공 코드 C6 대응

   일반 우회전과 피봇 시간은 동일하게 두고
   코너 안쪽 진입시간을 300ms보다 길게 사용

   성공 코드:
   internul_delay * 1.4
   300ms * 1.4 = 420ms
   ========================================================= */

void blackTurnRightSharp(
    uint16_t entryMs
)
{
    motorDrive(
        BLACK_STRAIGHT_LEFT_SPEED,
        BLACK_STRAIGHT_RIGHT_SPEED
    );


    delayMs(
        entryMs
    );


    motorStop();


    delayMs(
        5
    );


    motorDrive(
         BLACK_TURN_SPEED,
        -BLACK_TURN_SPEED
    );


    delayMs(
        BLACK_SHARP_TURN_MS
    );


    motorStop();


    delayMs(
        20
    );


    pidPrevError = 0;
}


/* =========================================================
   마지막 좌회전 이후 직선 정렬 후 정지

   일정 시간 이상 Black PID로 진행한 뒤
   중앙 IR3 / IR4가 라인을 잡고
   전체 감지 개수가 2개 이하인 상태를
   연속 확인하면 일자선에 올라온 것으로 판단
   ========================================================= */

void blackFollowStraightAndStop(
    int Kp,
    int Ki,
    int Kd,
    uint16_t minCount
)
{
    uint16_t count = 0;


    /*
     * Sequence5의 모든 교차로 / 회전이 끝난 뒤
     * Black PID로 계속 진행.
     *
     * 최소 진행시간이 지난 뒤
     * 가로 흰선이 IR 4개 이상 잡히면
     * 벽탐지 구간 시작선으로 판단하고 정지.
     */

    while (1)
    {
        followLineBlack(
            Kp,
            Ki,
            Kd
        );


        _delay_ms(10);


        count++;


        if (
            count > minCount
            &&
            irCount >= 4
        )
        {
            break;
        }
    }


    motorStop();


    pidPrevError = 0;
}


/* =========================================================
   Sequence6 PSD 벽돌 접근

   1. ADC 500 이상을 한 번 확인
   2. 그 이후 ADC 400 이하로 내려오면 정지

   성공 코드처럼 벽돌 접근 중에는
   PID가 아니라 등속 직진 사용
   ========================================================= */

void brickApproachByPSD(void)
{
    uint16_t psdValue;

    uint8_t peakSeen = 0;


    /* ADC 채널 전환 후 버림 읽기 */

    adcRead(
        PSD_CHANNEL
    );


    _delay_ms(2);


    adcRead(
        PSD_CHANNEL
    );


    _delay_ms(2);


    motorDrive(
        BLACK_STRAIGHT_LEFT_SPEED,
        BLACK_STRAIGHT_RIGHT_SPEED
    );


    while (1)
    {
        psdValue =
            adcRead(
                PSD_CHANNEL
            );


        if (
            peakSeen == 0
        )
        {
            if (
                psdValue >=
                BRICK_PSD_PEAK_THRESHOLD
            )
            {
                peakSeen = 1;
            }
        }

        else
        {
            if (
                psdValue <=
                BRICK_PSD_STOP_THRESHOLD
            )
            {
                break;
            }
        }


        _delay_ms(10);
    }


    motorStop();


    delayMs(
        BRICK_STOP_WAIT_MS
    );


    pidPrevError = 0;
}


/* =========================================================
   Sequence6 벽돌 직후 좌회전

   성공 코드처럼 일반 회전시간의 약 0.9배
   현재 800ms 기준 720ms
   ========================================================= */

void brickTurnLeft90(void)
{
    motorDrive(
        -BLACK_TURN_SPEED,
         BLACK_TURN_SPEED
    );


    delayMs(
        BRICK_LEFT_TURN_MS
    );


    motorStop();


    delayMs(
        20
    );


    pidPrevError = 0;
}


/* =========================================================
   Delay
   ========================================================= */

void delayMs(uint16_t ms)
{
    while (
        ms--
    )
    {
        _delay_ms(1);
    }
}


/* =========================================================
   일반 구간 왼쪽 피봇

   흰 바탕 / 검은 선
   ========================================================= */

void pivotLeftUntilLine(int speed)
{
    uint8_t leftOldLine = 0;


    motorDrive(
        -speed,
         speed
    );


    while (1)
    {
        updateIR();


        /*
         * 기존 선에서 먼저 이탈
         */

        if (
            leftOldLine == 0
        )
        {
            if (
                irState[2] == 0
                &&
                irState[3] == 0
            )
            {
                leftOldLine = 1;
            }
        }


        /*
         * 새로운 선을 중앙에서 잡음
         */

        else
        {
            if (
                irState[2] == 1
                ||
                irState[3] == 1
            )
            {
                break;
            }
        }


        _delay_ms(10);
    }


    motorStop();
}


/* =========================================================
   주차 180도 센서 기반 좌피봇

   Sequence4에서 이미
   1. 100ms 좌피봇
   2. 1초 정지
   를 수행한 뒤 이 함수로 들어옴

   여기서는 기존 1700ms 고정회전 대신
   1. 1000ms 동안 센서 판정을 무시하며 좌피봇
   2. 중간 T / 90도 후보를 지나간 뒤
   3. 중앙 센서가 한 번 라인에서 완전히 빠진 것을 확인
   4. 이후 중앙 목표 라인을 3회 연속 잡으면 종료

   1000ms는 회전 종료 시간이 아니라
   중간 T를 판정 대상에서 제외하기 위한 최소 회전 시간
   ========================================================= */

void pivotLeft180UntilLine(int speed)
{
    uint8_t lineLeft = 0;

    uint8_t stableCount = 0;


    motorDrive(
        -(speed + PARKING_180_LEFT_BOOST),
          speed
    );


    /* =====================================================
       중간 T / 90도 라인 구간은 판정하지 않음
       ===================================================== */

    delayMs(
        PARKING_180_IGNORE_MS
    );


    while (1)
    {
        updateIR();


        /* =================================================
           1000ms 회전 후
           중앙 센서가 한 번 완전히 라인에서 빠져야
           목표 라인 탐색을 시작
           ================================================= */

        if (
            lineLeft == 0
        )
        {
            if (
                irState[2] == 0
                &&
                irState[3] == 0
            )
            {
                lineLeft = 1;

                stableCount = 0;
            }
        }


        /* =================================================
           이후 처음 중앙으로 들어오는 일반 라인을
           180도 목표 라인으로 판단
           ================================================= */

        else
        {
            if (
                irCount <= 2
                &&
                (
                    irState[2] == 1
                    ||
                    irState[3] == 1
                )
            )
            {
                stableCount++;


                if (
                    stableCount >=
                    PARKING_TURN_STABLE_COUNT
                )
                {
                    break;
                }
            }

            else
            {
                stableCount = 0;
            }
        }


        _delay_ms(10);
    }


    motorStop();


    delayMs(
        50
    );


    pidPrevError = 0;
}


/* =========================================================
   Sequence 0
   S자
   ========================================================= */

void Sequence0_S(void)
{
    const int Kp = 500;

    const int Ki = 0;

    const int Kd = 2000;


    followLineUntilAtMost(
        Kp,
        Ki,
        Kd,
        4,
        50
    );


    followLineUntilAtLeast(
        Kp,
        Ki,
        Kd,
        5,
        1000
    );
}


/* =========================================================
   Sequence 1
   8자
   ========================================================= */

void Sequence1_Eight(void)
{
    const int Kp = 500;

    const int Ki = 0;

    const int Kd = 2000;


    uint8_t intersectionCount = 0;

    uint8_t readyToCount = 0;


    while (1)
    {
        followLine(
            Kp,
            Ki,
            Kd
        );


        if (
            irCount <= 2
        )
        {
            readyToCount = 1;
        }


        if (
            readyToCount == 1
            &&
            irCount >= 4
        )
        {
            intersectionCount++;


            readyToCount = 0;


            if (
                intersectionCount == 2
            )
            {
                motorDrive(
                    BASE_SPEED + 10,
                    BASE_SPEED - 10
                );


                delayMs(
                    120
                );


                motorDrive(
                    BASE_SPEED,
                    BASE_SPEED
                );


                delayMs(
                    200
                );
            }


            if (
                intersectionCount >= 3
            )
            {
                break;
            }
        }


        _delay_ms(10);
    }


    motorDrive(
        BASE_SPEED,
        BASE_SPEED
    );


    delayMs(
        200
    );


    pivotLeftUntilLine(
        PIVOT_SPEED
    );


    followLineUntilAtMost(
        Kp,
        Ki,
        Kd,
        2,
        50
    );


    followLineUntilAtLeast(
        Kp,
        Ki,
        Kd,
        4,
        100
    );
}


/* =========================================================
   Timer3 시작
   ========================================================= */

void laneTimerStart(void)
{
    TCCR3A =
        0x00;


    TCCR3B =
        0x00;


    TCNT3 =
        0;


    laneTimerCount =
        0;


    OCR3A =
        3124;


    ETIFR =
        (1 << OCF3A);


    ETIMSK |=
        (1 << OCIE3A);


    TCCR3B =
        (1 << WGM32) |
        (1 << CS32);


    sei();
}


/* =========================================================
   Timer3 정지
   ========================================================= */

void laneTimerStop(void)
{
    TCCR3B &=
        ~(
            (1 << CS32) |
            (1 << CS31) |
            (1 << CS30)
        );


    ETIMSK &=
        ~(1 << OCIE3A);
}


/* =========================================================
   Timer3 읽기
   ========================================================= */

uint16_t getLaneTimerCount(void)
{
    uint16_t value;

    uint8_t savedSREG;


    savedSREG =
        SREG;


    cli();


    value =
        laneTimerCount;


    SREG =
        savedSREG;


    return value;
}


/* =========================================================
   평행사변형 벽 판정
   ========================================================= */

char detectLaneWall(void)
{
    uint8_t leftOuter;

    uint8_t rightOuter;

    int position = 0;


    leftOuter =
        irState[0] +
        irState[1];


    rightOuter =
        irState[4] +
        irState[5];


    if (
        leftOuter >
        rightOuter
    )
    {
        return 'L';
    }


    if (
        rightOuter >
        leftOuter
    )
    {
        return 'R';
    }


    for (
        uint8_t i = 0;
        i < IR_NUM;
        i++
    )
    {
        if (
            irState[i] == 1
        )
        {
            position +=
                IR_WEIGHT[i];
        }
    }


    if (
        position < 0
    )
    {
        return 'L';
    }


    if (
        position > 0
    )
    {
        return 'R';
    }


    if (
        laneLastWall < 0
    )
    {
        return 'L';
    }


    if (
        laneLastWall > 0
    )
    {
        return 'R';
    }


    return 'L';
}


/* =========================================================
   평행사변형 벽 회피
   ========================================================= */

char laneCarlineStep(void)
{
    char hitWall;


    updateIR();


    if (
        laneTurnPending == 1
    )
    {
        motorBrake();


        delayMs(
            10
        );


        motorStop();


        delayMs(
            10
        );


        if (
            laneLastWall == -1
        )
        {
            motorDrive(
                 LANE_TURN_SPEED,
                -LANE_TURN_SPEED
            );
        }

        else if (
            laneLastWall == 1
        )
        {
            motorDrive(
                -LANE_TURN_SPEED,
                 LANE_TURN_SPEED
            );
        }


        delayMs(
            LANE_TURN_MS
        );


        motorBrake();


        delayMs(
            10
        );


        motorStop();


        updateIR();


        laneTurnPending = 0;


        return 'n';
    }


    if (
        irCount > 0
    )
    {
        hitWall =
            detectLaneWall();


        if (
            hitWall == 'L'
        )
        {
            laneLastWall = -1;
        }

        else
        {
            laneLastWall = 1;
        }


        laneTurnPending = 1;


        motorBrake();


        delayMs(
            10
        );


        motorDrive(
            -LANE_HIT_BACK_SPEED,
            -LANE_HIT_BACK_SPEED
        );


        delayMs(
            LANE_HIT_BACK_MS
        );


        motorBrake();


        delayMs(
            5
        );


        motorStop();


        delayMs(
            10
        );


        return hitWall;
    }


    if (
        laneLastWall == 0
    )
    {
        motorDrive(
            BASE_SPEED,
            BASE_SPEED
        );
    }

    else if (
        laneLastWall == -1
    )
    {
        motorDrive(
            BASE_SPEED + LANE_BASE_BIAS,
            BASE_SPEED - LANE_BASE_BIAS
        );
    }

    else
    {
        motorDrive(
            BASE_SPEED - LANE_BASE_BIAS,
            BASE_SPEED + LANE_BASE_BIAS
        );
    }


    delayMs(
        25
    );


    return 'n';
}


/* =========================================================
   Sequence 2
   평행사변형
   ========================================================= */

void Sequence2_Lane(void)
{
    const int Kp = 500;

    const int Ki = 0;

    const int Kd = 2000;


    uint16_t timerValue;


    followLineUntilAtMost(
        Kp,
        Ki,
        Kd,
        2,
        60
    );


    laneLastWall = 0;

    laneTurnPending = 0;


    laneTimerStart();


    while (1)
    {
        timerValue =
            getLaneTimerCount();


        if (
            timerValue >=
            LANE_EXIT_TIMER_COUNT
        )
        {
            break;
        }


        laneCarlineStep();


        _delay_ms(10);
    }


    laneTimerStop();


    motorDrive(
        -LANE_TURN_SPEED,
         LANE_TURN_SPEED
    );


    delayMs(
        LANE_EXIT_TURN_MS
    );


    motorDrive(
        BASE_SPEED,
        BASE_SPEED
    );


    delayMs(
        LANE_EXIT_STRAIGHT_MS
    );


    /* =====================================================
       평행사변형 종료 후 추가 직진

       기존 200ms 직진은 그대로 유지하고
       100ms를 추가로 직진
       ===================================================== */

    delayMs(
        LANE_EXIT_EXTRA_STRAIGHT_MS
    );
}


/* =========================================================
   Sequence 3
   차단바
   ========================================================= */

void Sequence3_Barrier(void)
{
    const int Kp = 500;

    const int Ki = 0;

    const int Kd = 2000;


    uint16_t psdValue;

    uint16_t stopPSD;

    uint16_t difference;

    uint8_t openCount = 0;

    uint32_t psdSum = 0;


    /*
     * 차단바 위치까지 PID
     */

    for (
        uint16_t i = 0;
        i < 280;
        i++
    )
    {
        followLine(
            Kp,
            Ki,
            Kd
        );


        _delay_ms(10);
    }


    motorStop();


    delayMs(
        200
    );


    /*
     * ADC 채널 PF7 → PF1 전환
     */

    adcRead(
        PSD_CHANNEL
    );


    _delay_ms(2);


    adcRead(
        PSD_CHANNEL
    );


    _delay_ms(2);


    /*
     * PSD 기준값
     */

    psdSum = 0;


    for (
        uint8_t i = 0;
        i < 5;
        i++
    )
    {
        psdSum +=
            adcRead(
                PSD_CHANNEL
            );


        _delay_ms(2);
    }


    stopPSD =
        (uint16_t)
        (
            psdSum / 5
        );


    openCount = 0;


    while (1)
    {
        psdValue =
            adcRead(
                PSD_CHANNEL
            );


        if (
            psdValue >=
            stopPSD
        )
        {
            difference =
                psdValue -
                stopPSD;
        }

        else
        {
            difference =
                stopPSD -
                psdValue;
        }


        if (
            difference >=
            BARRIER_OPEN_DIFF
        )
        {
            openCount++;


            if (
                openCount >=
                BARRIER_OPEN_COUNT
            )
            {
                break;
            }
        }

        else
        {
            openCount = 0;
        }


        motorStop();


        _delay_ms(10);
    }
}


/* =========================================================
   Sequence 4
   주차
   ========================================================= */

void Sequence4_Parking(void)
{
    const int Kp = 500;

    const int Ki = 0;

    const int Kd = 2000;


    /* 차단바 이후 */

    parkingFollowUntilAtMost(
        Kp,
        Ki,
        Kd,
        2,
        100
    );


    /* 주차 입구 T */

    parkingFollowUntilAtLeast(
        Kp,
        Ki,
        Kd,
        4,
        200
    );


    motorStop();


    delayMs(
        100
    );


    /* 입구 안으로 */

    motorDrive(
        BASE_SPEED - PARKING_LEFT_TRIM,
        BASE_SPEED
    );


    delayMs(
        500
    );


    motorStop();


    delayMs(
        20
    );


    /* 좌회전 */

    pivotLeftUntilLine(
        60
    );


    /* 라인 확인 */

    parkingFollowUntilAtMost(
        Kp,
        Ki,
        Kd,
        2,
        100
    );


    /* 내부 첫 T */

    parkingFollowUntilAtLeast(
        Kp,
        Ki,
        Kd,
        4,
        400
    );


    /*
     * 첫 T 직진 통과
     * PID OFF
     */

    motorDrive(
        BASE_SPEED - PARKING_LEFT_TRIM,
        BASE_SPEED
    );


    while (1)
    {
        updateIR();


        if (
            irCount <= 2
        )
        {
            break;
        }


        _delay_ms(10);
    }


    pidPrevError = 0;


    /* 안쪽 정지 라인 */

    parkingFollowUntilAtLeast(
        Kp,
        Ki,
        Kd,
        4,
        50
    );


    motorStop();


    delayMs(
        50
    );


    /* =====================================================
       주차 내부 180도 회전

       기존 동작 중
       1. 100ms 좌피봇 유지
       2. 1초 정지 유지
       3. 기존 1700ms 고정 회전만 삭제
       4. 이후 센서로 목표 라인을 잡을 때까지 회전
       ===================================================== */

    /* 기존 100ms 좌피봇 유지 */

    motorDrive(
        -PARKING_TURN_SPEED,
         PARKING_TURN_SPEED
    );


    delayMs(
        PARKING_FIRST_TURN_MS
    );


    motorStop();


    /* 기존 1초 정지 유지 */

    delayMs(
        1000
    );


    /* 기존 1700ms 대신 센서 기반 회전 */

    pivotLeft180UntilLine(
        PARKING_TURN_SPEED
    );


    /* =====================================================
       180도 회전 후 주차 출구

       센서 조건 / Parking PID 사용하지 않음.

       1. 무조건 700ms 직진
       2. 좌회전 속도 60 -> 40
       3. 좌회전 완료 후 기본 PID 라인트레이싱
       ===================================================== */

    motorDrive(
        BASE_SPEED - PARKING_LEFT_TRIM,
        BASE_SPEED
    );


    delayMs(
        700
    );


    motorStop();


    delayMs(
        20
    );


    /* 출구 좌회전 */

    pivotLeftUntilLine(
        50
    );


    pidPrevError = 0;


    /* 좌회전 완료 후 기본 PID 라인트레이싱 */

    followLineUntilAtMost(
        Kp,
        Ki,
        Kd,
        2,
        100
    );

}void Sequence5_Black(void)
{
    const int Kp = 500;

    const int Ki = 0;

    const int Kd = 2000;


    /* =====================================================
       검정 구간 직전 검은 가로선

       아직 흰 바탕 / 검은 선 모드
       ===================================================== */

    motorDrive(
        BASE_SPEED,
        BASE_SPEED
    );


    while (1)
    {
        updateIR();


        if (
            irCount >= 5
        )
        {
            break;
        }


        _delay_ms(10);
    }


    pidPrevError = 0;


    /* =====================================================
       검정 영역에서 첫 흰 라인 탐색
       ===================================================== */

    motorDrive(
        BASE_SPEED,
        BASE_SPEED
    );


    while (1)
    {
        updateIRBlack();


        if (
            irCount >= 1
        )
        {
            break;
        }


        _delay_ms(10);
    }


    pidPrevError = 0;


    /* =====================================================
       L0 -> 교차로 1

       성공 코드
       L0 = 20
       EG0 = 50 / 40
       ===================================================== */

    blackMoveToEvent(
        Kp,
        Ki,
        Kd,
        3,
        20
    );


    blackPassIntersection(
        50,
        40
    );


    /* =====================================================
       L1 -> 첫 번째 좌90

       성공 코드 L1 = 10
       ===================================================== */

    blackMoveToEvent(
        Kp,
        Ki,
        Kd,
        3,
        10
    );


    blackTurnLeft90(
        BLACK_TURN_ENTRY_MS
    );


    /* =====================================================
       L2 -> 두 번째 좌90

       성공 코드 Sort_line_delay = 7
       ===================================================== */

    blackMoveToEvent(
        Kp,
        Ki,
        Kd,
        3,
        7
    );


    blackTurnLeft90(
        BLACK_TURN_ENTRY_MS
    );


    /* =====================================================
       C1 직후 1초 잠금

       두 번째 좌90 뒤에 붙어 있는 EG1은
       별도 탐색 / 별도 직진 처리하지 않음.

       약 2초 동안은 Black PID만 수행하고
       모든 이벤트 판정을 무시함.

       잠금이 끝난 뒤부터
       첫 번째 우90(C2) 이벤트 탐색 시작.
       ===================================================== */

    for (
        uint16_t i = 0;
        i < BLACK_C1_LOCKOUT_COUNT;
        i++
    )
    {
        followLineBlack(
            Kp,
            Ki,
            Kd
        );

        _delay_ms(10);
    }


    /* =====================================================
       L4 -> 첫 번째 우90
       ===================================================== */

    blackMoveToEvent(
        Kp,
        Ki,
        Kd,
        3,
        7
    );


    blackTurnRight90(
        BLACK_TURN_ENTRY_MS
    );


    /* =====================================================
       L5 -> 두 번째 우90

       성공 코드 L5 = 30
       ===================================================== */

    blackMoveToEvent(
        Kp,
        Ki,
        Kd,
        3,
        30
    );


    blackTurnRight90(
        BLACK_TURN_ENTRY_MS
    );


    /* =====================================================
       L6 -> 교차로 3

       성공 코드
       EG2 = 50 / 40
       ===================================================== */

    blackMoveToEvent(
        Kp,
        Ki,
        Kd,
        3,
        7
    );


    blackPassIntersection(
        50,
        40
    );


    /* =====================================================
       L7 -> 세 번째 우90
       ===================================================== */

    blackMoveToEvent(
        Kp,
        Ki,
        Kd,
        3,
        7
    );


    blackTurnRight90(
        BLACK_TURN_ENTRY_MS
    );


    /* =====================================================
       L8 -> 네 번째 우90
       ===================================================== */

    blackMoveToEvent(
        Kp,
        Ki,
        Kd,
        3,
        7
    );


    blackTurnRight90(
        BLACK_TURN_ENTRY_MS
    );


    /* =====================================================
       L9 -> 급우회전 C6

       직전 우90 회전이 완전히 끝나기 전에
       급우회전 이벤트를 잘못 잡는 것을 방지

       우90 종료 후 약 3초 동안은
       Black PID만 수행하고 이벤트 판정을 금지

       3초 이후부터 IR 3개 이상을
       급우회전 이벤트로 인정
       ===================================================== */

    blackMoveToEvent(
        Kp,
        Ki,
        Kd,
        3,
        BLACK_SHARP_LOCKOUT_COUNT
    );


    blackTurnRightSharp(
        BLACK_SHARP_ENTRY_MS
    );


    /* =====================================================
       급우회전 C6 직후 2초 잠금

       급우회전 직후 바로 붙어 있는 교차로4(EG10)는
       별도 탐색 / 별도 직진 처리하지 않음.

       약 1초 동안은 Black PID만 수행하고
       모든 이벤트 판정을 무시함.

       잠금이 끝난 뒤부터
       다음 교차로5(EG11) 이벤트 탐색 시작.
       ===================================================== */

    for (
        uint16_t i = 0;
        i < BLACK_AFTER_SHARP_LOCKOUT_COUNT;
        i++
    )
    {
        followLineBlack(
            Kp,
            Ki,
            Kd
        );

        _delay_ms(10);
    }


    /* =====================================================
       L11 -> 교차로 5
       ===================================================== */

    blackMoveToEvent(
        Kp,
        Ki,
        Kd,
        3,
        7
    );


    blackPassIntersection(
        30,
        20
    );


    /* =====================================================
       교차로 5 이후 완만한 우회전

       약 2초 동안은 Black PID만 수행하고
       IR 이벤트 판정을 전부 무시함.

       완만한 우회전을 충분히 따라간 뒤부터
       IR 4개 이상을 마지막 좌회전 이벤트로 인정.
       ===================================================== */

    for (
        uint16_t i = 0;
        i < BLACK_FINAL_TURN_LOCKOUT_COUNT;
        i++
    )
    {
        followLineBlack(
            Kp,
            Ki,
            Kd
        );

        _delay_ms(10);
    }


    blackMoveToEvent(
        Kp,
        Ki,
        Kd,
        4,
        7
    );


    /* =====================================================
       마지막 좌90
       ===================================================== */

    blackTurnLeft90(
        BLACK_TURN_ENTRY_MS
    );


    /* =====================================================
       마지막 일자선

       중앙 라인이 안정적으로 잡히면 정지
       ===================================================== */

    blackFollowStraightAndStop(
        Kp,
        Ki,
        Kd,
        BLACK_FINAL_STRAIGHT_COUNT
    );


    /*
     * 벽탐지 시작 가로 흰선에서 정지한 뒤
     * Sequence6_Brick()으로 넘어감.
     */

    return;
}


/* =========================================================
   Sequence 6
   벽돌 / PSD 구간

   현재 Sequence5 종료 위치:
   벽탐지 시작 첫 가로 흰선에서 정지

   성공 코드 구조를 현재 시작점에 맞게 적용
   ========================================================= */

void Sequence6_Brick(void)
{
    const int Kp = 500;

    const int Ki = 0;

    const int Kd = 1500;


    /* =====================================================
       현재 가로 흰선 탈출
       ===================================================== */

    while (1)
    {
        followLineBlack(
            Kp,
            Ki,
            Kd
        );


        _delay_ms(10);


        if (
            irCount <= 2
        )
        {
            break;
        }
    }


    pidPrevError = 0;


    /* =====================================================
       성공 코드 END-2에 해당하는 다음 가로선

       첫 가로선은 Sequence5에서 이미 잡았으므로
       다음 한 개만 추가 탐색
       ===================================================== */

    blackMoveToEvent(
        Kp,
        Ki,
        Kd,
        3,
        2
    );


    /* =====================================================
       PSD 벽돌 접근

       500 이상을 먼저 확인한 뒤
       400 이하로 내려오면 정지
       ===================================================== */

    brickApproachByPSD();


    /* =====================================================
       벽돌 직후 좌회전
       ===================================================== */

    brickTurnLeft90();


    /* =====================================================
       첫 번째 가로선
       ===================================================== */

    blackMoveToEvent(
        Kp,
        Ki,
        Kd,
        3,
        20
    );


    /* =====================================================
       두 번째 가로선
       ===================================================== */

    blackMoveToEvent(
        Kp,
        Ki,
        Kd,
        3,
        20
    );


    /* =====================================================
       우회전 90도
       성공 코드 internul_delay = 300ms
       ===================================================== */

    blackTurnRight90(
        BRICK_RIGHT_ENTRY_MS
    );


    /* =====================================================
       다음 코스 끝선
       성공 코드처럼 IR 5개 이상
       ===================================================== */

    blackMoveToEvent(
        Kp,
        Ki,
        Kd,
        5,
        20
    );


    motorStop();


    pidPrevError = 0;
}
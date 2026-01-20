import cv2
import mediapipe as mp
import serial
import time


cap = cv2.VideoCapture(0, cv2.CAP_AVFOUNDATION)

if not cap.isOpened():
    print("Camera failed to open")
    exit()

print("Camera started")


SERIAL_PORT = "/dev/tty.usbmodem..."
BAUD_RATE = 115200

ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
time.sleep(2)

print("Serial connected")


mp_hands = mp.solutions.hands
mp_draw = mp.solutions.drawing_utils

hands = mp_hands.Hands(
    static_image_mode=False,
    max_num_hands=1,
    min_detection_confidence=0.7,
    min_tracking_confidence=0.7
)


def detect_gesture(hand_landmarks):
    tips = [4, 8, 12, 16, 20]
    fingers = []


    fingers.append(
        hand_landmarks.landmark[4].x <
        hand_landmarks.landmark[3].x
    )

    for tip in tips[1:]:
        fingers.append(
            hand_landmarks.landmark[tip].y <
            hand_landmarks.landmark[tip - 2].y
        )

    if fingers == [False, False, False, False, False]:
        return "R"

    if fingers == [True, True, True, True, True]:
        return "P"

    if fingers == [False, True, True, False, False]:
        return "S"

    return None


SEND_INTERVAL = 0.25   
last_send_time = 0.0

print("Show ROCK / PAPER / SCISSORS")

while True:
    ret, frame = cap.read()
    if not ret:
        continue

    frame = cv2.flip(frame, 1)

    rgb = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
    results = hands.process(rgb)

    gesture = None

    if results.multi_hand_landmarks:
        for hand_landmarks in results.multi_hand_landmarks:
            mp_draw.draw_landmarks(
                frame,
                hand_landmarks,
                mp_hands.HAND_CONNECTIONS
            )

            gesture = detect_gesture(hand_landmarks)

            if gesture:
                cv2.putText(
                    frame,
                    f"Gesture: {gesture}",
                    (20, 50),
                    cv2.FONT_HERSHEY_SIMPLEX,
                    1.2,
                    (0, 255, 0),
                    3
                )

    now = time.time()
    if gesture and (now - last_send_time) >= SEND_INTERVAL:
        ser.write(gesture.encode())
        ser.write(gesture.encode())  
        print(">>> SENT BYTE:", gesture)
        last_send_time = now

    cv2.imshow("Rock Paper Scissors", frame)

    if cv2.waitKey(1) & 0xFF == 27:
        break

cap.release()
ser.close()
cv2.destroyAllWindows()

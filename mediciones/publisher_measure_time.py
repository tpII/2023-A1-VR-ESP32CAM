import paho.mqtt.client as mqtt
import curses
import time

def on_connect(client, userdata, flags, rc):
    # client.subscribe("ack_topic")
    print("Connected with result code "+str(rc))

def on_message(client, userdata, msg):
    if msg.topic == "ack_topic":
        received_time = time.time()
        sent_time = float(msg.payload.decode().split(',')[1])
        rtt = received_time - sent_time
        print(f"RTT: {rtt}")
        # Add this RTT to a list and calculate average

def main(stdscr):
    # Properly configure the curses environment
    curses.curs_set(0)
    stdscr.nodelay(1)
    stdscr.timeout(100)

    client = mqtt.Client()
    client.on_connect = on_connect

    client.on_message = on_message



    client.connect("192.168.100.132", 1883, 60)

    last_key_pressed = "X"

    while True:
        # client.loop()
        c = stdscr.getch()
        if c != -1:
            if c == curses.KEY_UP:
                last_key_pressed = "U"
                # client.publish("test", "U")
                timestamp = time.time()
                client.publish("test", f"U,{timestamp}")
            elif c == curses.KEY_DOWN:
                last_key_pressed = "D"
                timestamp = time.time()
                client.publish("test", f"D,{timestamp}")
            elif c == curses.KEY_RIGHT:
                last_key_pressed = "R"
                timestamp = time.time()
                client.publish("test", f"R,{timestamp}")
            elif c == curses.KEY_LEFT:
                last_key_pressed = "L"
                timestamp = time.time()
                client.publish("test", f"L,{timestamp}")
            elif c == ord('q'):  # Press 'q' to quit
                break
        else:  # No key was pressed
            if (last_key_pressed != "S"):
                timestamp = time.time()
                client.publish("test", f"S,{timestamp}")
            last_key_pressed = "S"

        #  delay 100 ms ver si modificaba el problema de que al principio no manda chorro de mensajes
        # curses.napms(10)

    curses.endwin()  # Restore the terminal to its original state

if __name__ == "__main__":
    curses.wrapper(main)

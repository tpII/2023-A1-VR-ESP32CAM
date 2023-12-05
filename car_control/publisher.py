import paho.mqtt.client as mqtt
import curses

def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))



def main(stdscr):
    # Properly configure the curses environment
    curses.curs_set(0)
    stdscr.nodelay(1)
    stdscr.timeout(100)

    client = mqtt.Client()
    client.on_connect = on_connect

    client.connect("192.168.137.48", 1883, 60)

    last_key_pressed = "X"

    while True:
        # client.loop()
        c = stdscr.getch()
        if c != -1:
            if c == curses.KEY_UP:
                last_key_pressed = "U"
                client.publish("test", "U")
            elif c == curses.KEY_DOWN:
                last_key_pressed = "D"
                client.publish("test", "D")
            elif c == curses.KEY_RIGHT:
                last_key_pressed = "R"
                client.publish("test", "R")
            elif c == curses.KEY_LEFT:
                last_key_pressed = "L"
                client.publish("test", "L")
            elif c == ord('q'):  # Press 'q' to quit
                break
        else:  # No key was pressed
            if (last_key_pressed != "S"):
                client.publish("test", "S")
            last_key_pressed = "S"

        #  delay 100 ms
        curses.napms(10)

    curses.endwin()  # Restore the terminal to its original state

if __name__ == "__main__":
    curses.wrapper(main)

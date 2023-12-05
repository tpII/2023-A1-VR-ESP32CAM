import paho.mqtt.client as mqtt
import time

def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))
    # client.subscribe("test")
    client.subscribe("ack_topic")

rtt_sum = 0
rtt_amount = 0

# Inside the on_message function

def on_message(client, userdata, msg):
    global rtt_sum
    global rtt_amount

    if msg.topic == "ack_topic":
        received_time = time.time()
        sent_time = float(msg.payload.decode().split(',')[1])
        # rtt = round((received_time - sent_time) * 1000, 3)

        rtt_sum += received_time - sent_time
        rtt_amount += 1
        # print(f"RTT: {rtt}ms")
        print(f"Amount: {rtt_amount}")
        # Add this RTT to a list and calculate average
    else:
        print(f'Topic: {msg.topic} - Message: {msg.payload.decode()}')

def main():
    client = mqtt.Client()
    client.on_connect = on_connect
    client.on_message = on_message

    client.connect("192.168.100.132", 1883, 60)

    while (rtt_amount < 100):
        client.loop()

    print(f'Average RTT: {round((rtt_sum / rtt_amount)*1000, 3)}ms')


if __name__ == "__main__":
    main()

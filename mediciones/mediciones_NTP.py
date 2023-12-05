import requests
import ntplib
from datetime import datetime
import time
from datetime import timedelta

NTP_SERVER = '192.168.137.48'

def parse_esp_timestamp(timestamp_str):
    # Convert the timestamp to float (seconds since epoch)
    epoch_seconds = float(timestamp_str)
    # Convert to datetime
    return datetime.fromtimestamp(epoch_seconds)

def get_ntp_time():
    client = ntplib.NTPClient()
    response = client.request(NTP_SERVER, version=3)
    return datetime.fromtimestamp(response.tx_time)

def stream_video_and_compare_timestamp(url):
    total_size = 0  # Initialize total size counter
    try:
        with requests.get(url, stream=True) as r:
            # print(len(r))
            for line in r.iter_lines():
                # Check for X-Timestamp in the header
                # print(line)
                if line.startswith(b'X-NTPTimestamp:'):

                    total_size +=len(line)
                    # timestamp = line
                    timestamp = line.decode("utf-8").split(': ', 1)[1]

                    # print(datetime.fromtimestamp(timestamp))
                    esp_time = datetime.strptime(timestamp, '%Y-%m-%d %H:%M:%S.%f') - timedelta(hours=4)
                    # print(timestamp_dt)

                    ntp_time = get_ntp_time()
                    # print((ntp_time))
                    # print(ntp_time)
                    # print(ntp_time)

                    diff = (ntp_time-esp_time).total_seconds() * 1000
                    if (diff > 0):
                        print(f"Time difference: {diff} milliseconds")


                        # print(esp_time, ntp_time)
                    # else:
                    #     print("menores")
                    #     print(esp_time, ntp_time)
                    #     print()
                elif line:
                    total_size +=len(line)
    except:
        print(f"Total size of the streamed response: {total_size} bytes")


# Replace with your actual stream URL
stream_url = 'http://192.168.137.238:81/stream'
stream_video_and_compare_timestamp(stream_url)

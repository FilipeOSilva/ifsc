import paho.mqtt.client as mqtt
import json
import time
import sys 
# public broker
broker = "test.mosquitto.org"
port = 1883
topic = "aula3-teste-filipe-jorge"

def send_msg(id, date, volt, nivel):
    message = {
        "id": id,
        "data": date,
        "tensao": volt,
        "nivel": nivel
    }
    message_json = json.dumps(message)
    client.publish(topic, message_json)
    print(f"Message sent: {message_json}")

def on_connect(client, userdata, flags, rc):
    if rc == 0:
        print("Successfully established connection to the broker")
    else:
        print(f"Connection failed. Return code: {rc}")

if __name__ =="__main__":

    id = sys.argv[1]
    date = sys.argv[2]
    volt = sys.argv[3]
    nivel = sys.argv[4]

    try:
        client = mqtt.Client()
        client.on_connect = on_connect
        client.connect(broker, port, 60)

        time.sleep(1)

        send_msg(id, date, volt, nivel)

        client.disconnect()
    except:
        print("Exit to except")
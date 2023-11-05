import paho.mqtt.client as mqtt
import json
import dbDAO

class MQTTSubscriber:
    def __init__(self, broker, topic):
        self.broker = broker
        self.topic = topic
        self.client = mqtt.Client()

        self.client.on_connect = self.on_connect
        self.client.on_message = self.on_message

    def on_connect(self, client, userdata, flags, rc):
        print(f"Connected with result code {rc}")
        self.client.subscribe(self.topic)

    def on_message(self, client, userdata, msg):
        try:
            data = json.loads(msg.payload)
            print(f"Received message: {data}")
            db_file = 'dados.db'
            dao = dbDAO.datasetDAO(db_file)
            
            dao.insert_data(data['data'], data['tensao'], data['nivel'], data['id'], data['status_bomba'], data['interruptor'])
            
        except json.JSONDecodeError as e:
            print(f"Error decoding JSON: {e}")
        except Exception as e:
            print(f"Error processing message: {e}")

    def start(self):
        self.client.connect(self.broker)
        self.client.loop_forever()

if __name__ == "__main__":
    broker = "test.mosquitto.org"
    topic = "trabalho-filipe-jorge"

    subscriber = MQTTSubscriber(broker, topic)
    subscriber.start()

#!/usr/bin/python3
import paho.mqtt.client as mqtt
from salesforce_api import Salesforce
import json

sf = Salesforce(username='matias@kforce.com', password='6tgAwEYL6Vj6UDS', security_token='qy5IOYAOupD2lmO3jXCJs52N7')

def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))
    client.subscribe("htO9wfUxA50uzDS/input")

def on_message(client, userdata, msg):
    print(msg.topic+" "+str(msg.payload))
    data = msg.payload
    data = data.decode('utf8').replace("'", '"')
    print(data)

def mqtt_publish(client, message):
    client.publish("htO9wfUxA50uzDS/input", message)

def connect_mqtt():
    client = mqtt.Client()
    client.on_connect = on_connect
    client.on_message = on_message
    client.username_pw_set(username="hGR2rL1latTuCnB", password="ASJ5c61zVvtuib7")
    client.connect("ioticos.org", 1883, 60)
    return client

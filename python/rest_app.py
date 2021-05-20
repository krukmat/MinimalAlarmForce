#!/usr/bin/python3
import sys, os
sys.path.append('/rest /var/www/html/AlarmForce/python/')
from flask import Flask, request
import mqtt_ioticos
app = Flask(__name__)

@app.route('/armed')
def index():
    value = request.args.get('armed', '')
    channel = request.args.get('channel_id')
    message = value + ";" + value + ";MQTT"
    if value and channel:
        client = mqtt_ioticos.connect_mqtt()
        mqtt_ioticos.mqtt_publish(client, message)
    return 'OK'
    
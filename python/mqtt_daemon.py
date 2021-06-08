#!/usr/bin/python3
import paho.mqtt.client as mqtt
from salesforce_api import Salesforce
from sf_creds import SF_USER, SF_PASSWORD, SF_ACCESS_TOKEN
import json

sf = Salesforce(username=SF_USER, password=SF_PASSWORD, security_token=SF_ACCESS_TOKEN)

def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))
    client.subscribe("channels/1299306/subscribe/json/18YFIXSCYYK95BTO/")

def on_message(client, userdata, msg):
    print(msg.topic+" "+str(msg.payload))
    data = msg.payload
    my_json = data.decode('utf8').replace("'", '"')
    data = json.loads(my_json)
    #try:
    result = sf.sobjects.Sensor__c.insert({'Date__c': data['created_at'], 'External_id__c': data['entry_id'], 'value__c':  data['field1'],  'channel__c': data['channel_id']})
    #catch:
    #    pass
    print(result)        
 
client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message
client.username_pw_set(username="Sensor_movimiento", password="FLDP47GIGCD7GHEP")
client.connect("mqtt.thingspeak.com", 1883, 60)
client.loop_forever()
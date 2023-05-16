import paho.mqtt.client as mqtt
import csv
from datetime import datetime
import re
import threading
import time
##################################################
#broker ip (ip4)
brokerip = '192.168.0.102'
#perioda senzoru (s)
perioda = 60
##################################################
def on_connect(client, userdata, flags, rc):
    print(f"Connected with result code {rc}")

    pass

def bum():
    print("funkce funguje")
    pass

def on_message(client, userdata, msg):
    vysledky = []
    print("snazim se decodovat" + msg.topic)
    #dekoduje zpravu venkovni jednotky
    if msg.topic == "venku/hod":
        now = datetime.now()
        cas = str(now.strftime("%d.%m %H:%M:%S"))
        f = open('dataVenku.csv','a', encoding='UTF8')
        print(f"Message received [{msg.topic}]: {msg.payload}")
        payload = str(msg.payload)
        vysledky = re.findall("\d+\.\d+", payload)
        print(vysledky)
        tisk = cas + ", " + str(vysledky[0]) + ", " + str(vysledky[1]) +", " + str(vysledky[2]) +", " + str(vysledky[3])
        f.write("\n" + tisk)
        f.close()
        pass
    #decoduje zpravu domaci jednotky
    if msg.topic == "doma/hod":
        now = datetime.now()
        cas = str(now.strftime("%d.%m %H:%M:%S"))
        f = open('dataDoma.csv','a', encoding='UTF8')
        print(f"Message received [{msg.topic}]: {msg.payload}")
        payload = str(msg.payload)
        vysledky = re.findall("\d+\.\d+", payload)
        tisk = cas + ", " + str(vysledky[0]) + ", " + str(vysledky[1]) +", " + str(vysledky[2])
        f.write("\n" + tisk)
        f.close()
        pass
    pass

def send_mesage(spanek):
    while True:
        print("Posilam zpravy")
        time.sleep(spanek)
        client.publish("venku/ov", "zprava")
        time.sleep(1)
        client.publish("doma/ov", "zprava")
        pass
    pass

client = mqtt.Client("piii")
client.connect(brokerip, 1883)
client.on_connect = on_connect



th = threading.Thread(target=send_mesage, args=(perioda,))
th.start()
print("Zapl jsem druhe jadro!")

client.loop_start()
client.subscribe("venku/hod")
client.subscribe("doma/hod")
client.on_message=on_message
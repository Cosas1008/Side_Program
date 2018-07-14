#!/usr/bin/env python
# -*- coding: utf-8 -*-
# refers from https://github.com/dpkp/kafka-python/blob/master/example.py

import threading
import logging
import time
import multiprocessing
import csv
from kafka import KafkaClient
from kafka import SimpleProducer
from kafka import KafkaProducer
import sys

class Producer(threading.Thread):
    def __init__(self):
        threading.Thread.__init__(self)
        self.stop_event = threading.Event()
        
    def stop(self):
        self.stop_event.set()

    def run(self):
        # producer = KafkaProducer(bootstrap_servers='localhost:9092')
        kafka = KafkaClient('127.0.0.1:9092')
        producer = SimpleProducer(kafka, async=True)

        topic_name = "test"
        print "sending messages to topic: [%s]" % (topic_name)

        for i in range(10):
            msg = 'async message-%d' % (i)
            print "sending message: [%s]" % msg
            producer.send_messages(topic_name, msg)
            # with data in open("test.csv","r"):
            #     producer.send(data)
            #     time.sleep(1)
            # break
        producer.close()

def main():
    # single thread
    productor = Producer()
    productor.start()

    time.sleep(1000)
    
    productor.stop()
    
        
        
if __name__ == "__main__":
    logging.basicConfig(
        format='%(asctime)s.%(msecs)s:%(name)s:%(thread)d:%(levelname)s:%(process)d:%(message)s',
        level=logging.INFO
        )
    main()
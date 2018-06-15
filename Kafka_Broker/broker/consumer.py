
from util import pyhiveexesql
from kafka import SimpleConsumer, SimpleClient
from kafka import KafkaConsumer
from kafka import KafkaClient


def initialize():
    try:
#    pyhiveexesql('SELECT * FROM my_awesome_data LIMIT 10')
        pyhiveexesql('DROP TABLE IF EXIST displayAds')
        pyhiveexesql('DROP TABLE IF EXIST searchAds')
        pyhiveexesql('DROP TABLE IF EXIST videoAds')
        pyhiveexesql('DROP TABLE IF EXIST socialAds')
        pyhiveexesql('CREATE TABLE displayAds(ad_type STRING, ad_id INT,impressions INT, clicks INT, reach INT, onDate DATE)')
        pyhiveexesql('CREATE TABLE searchAds(ad_type STRING, ad_id INT, clicks INT, reach INT, conversions INT, onDate DATE)')
        pyhiveexesql('CREATE TABLE videoAds(ad_type STRING, ad_id INT, views INT, onDate DATE, null0 TINYINT, null1 TINYINT)')
        pyhiveexesql('CREATE TABLE socialAds(ad_type STRING, ad_id INT, clicks INT, interactions INT, onDate DATE, null0 TINYINT)')
        pyhiveexesql("SHOW TABLES")
        print "[init] initial DB finished"
    except Exception, e:
        print '%s' % (e.message)
        print "[init] exception on initializing HIVE on derbyDb error"
        return False
    
    try:
        #open socket to kafka broker and listen to specific port as single node
        print "[init] start Kafka broker connection"
        kafka.connect("localhost:9021") # kafka broker host 0
        kafka.topic("test") 
        print "[init] finish Kafka broker connection"
        group_name = ""
        topic_name = "test"
        print "[init] start Kafka client connection with group_name: %s and topic_name: %s" % group_name, topic_name
        kafka = KafkaClient('127.0.0.1:9092')
        consumer = SimpleConsumer(kafka, group_name, topic_name)
    except Exception, e:
        print '%s' % (e.message)
        print "[init] exception on initializing HIVE on derbyDb error"
        return False
    return True

def consuming():
    print "[consum] start consuming"
    
    while True:
        print "[consum] Waiting for messages..."
        for msg in consumer:
            print "[consum] msg: %s", msg
            token = msg.split(kafka.msg,' ,')
            id = token[0]
            catagory = token[1]
            if(catagory.find('display')):
                # table displayAds add row
                print "[consum] display msg
            elif(catagory.find('search')):
                # table searchAds add row
                print "[consum] search msg
            elif(catagory.find('video')):
                # table videoAds add row
                print "[consum] video msg
            elif(catagory.find('socialAds')):
                # table socialAds add row
                print "[consum] social msg
            else:
                # show result
                pyhiveexesql("[consum] show report")
                showReport()


    # pyhiveexesql('DROP TABLE naiwe')
    # pyhiveexesql('SHOW TABLES')
    # pyhiveexesql("DROP TABLE there")
#!/usr/bin/env python
# -*- coding: utf-8 -*-

# package name
from broker import initialize, consuming
import threading
import functools

def run():
    events = []
    for request in requests:
        event = threading.Event()
        callback_with_event = functools.partial(callback, event)
        client.send_request(request, callback_with_event)
        events.append(event)
    print "Finish all events"
    return events

def callback(event error, response):
    # handle response
    event.set()

def wait_for_events(events):
    for event in events:
        event.wait()

def main():
    events = run()
    wait_for_events(events)


def main():
    request = []
    requests.appends(initialize())
    event = run()
    wait_for_events(event)
    print "[main] initialized DB finished"
    consuming()
    
    
if __name__ == '__main__':
    print "[main]running..."
    main()
    
    print "[main]finish running"

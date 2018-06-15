import sys
from pyhive import hive
from TCLIService.ttypes import TOperationState

def pyhiveexesql(sql):
    cursor = None
    try:
        cursor = hive.connect(host='localhost').cursor()
        cursor.execute(sql, async=True)

        status = cursor.poll().operationState
        while status in (TOperationState.INITIALIZED_STATE, TOperationState.RUNNING_STATE):
            logs = cursor.fetch_logs()
            for message in logs:
                print message
                # If needed, an asynchronous query can be cancelled at any time with:
                # cursor.cancel()
            status = cursor.poll().operationState
        print cursor.fetchall()
    except Exception, e:
        print '%s' % (e.message)
    finally:
        cursor.close()
    return cursor.fetchall()
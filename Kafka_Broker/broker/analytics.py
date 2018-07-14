'''
example report:

    table - displayAds                //table name
    count : 10000                     //total rows
    impressions : 803.022 / 1944.272  //(average / stdev)
    clicks : 71.411 / 461.585
    reach : 783.590 / 1898.662

TABLES:
    NAME        COL1       COL2    COL3        COL4    COL5    COL6
    --------------------------------------------------------------------
    displayAds  ad_type    ad_id   impressions clicks  reach   onDate
    searchAds   ad_type    ad_id    clicks      reach   conversions onDate
    videoAds    ad_type     ad_id   views       onDate  null0   null1
    socialAds   ad_type     ad_id   clicks      interactions    onDate  null0
'''

from util import pyhiveexesql

tables = ['displayAds','searchAds','videoAds','socialAds']

def showReport():
    total = 0
    discribe = ''
    # open output file as analytics result
    with open('analytics.out','w') as fanaly:
        for table in tables:
            total = pyhiveexesql('FROM displayAds COUNT ROWs')
            # get the column name
            sql = 'SHOW COLUMNS IN' + table
            describe = pyhiveexesql(sql)
            rows = []
            # check the column name, if the colume is null or date, the column can be skipped in the report
            for col in describe:
                if "nul" not in col or "Date" not in col:
                    # svalue = pyhiveexesql('SELECT SUM(OrderPrice) AS OrderTotal FROM displayAds')
                    avalue = pyhiveexesql('SELECT AVG(OrderPrice) AS OrderTotal FROM displayAds')
                    stdval = pyhiveexesql('SELECT STDEVP(OrderPrice) AS OrderTotal FROM displayAds')
                    rows.append((col,avalue,stdval))
                else:
                    pass
            # result
            print "The %s table has %d properties as:" % str(table), rows.count
            
            fanaly.write("table - %10s", table)
            fanaly.write("count : %10s", total)
            for row in rows:
                fanaly.write("%10s : %s//%s", row[0], row[1], row[2])
    print "Finish Report"
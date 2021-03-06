#Transit Analysis
#Brian Sisco
#Started July 28, 2012

import sqlite3
names = dict()

def main():
    """Performs various sorts of transit data analysis.
    Options:
    help    This info.
    q       Quit.
    init    Drop/add tables, populate with data from existing files.
    init f  Above, but for fare data instead of turnstile data.
    d       Download turnstile data files from MTA.info
    d f     Download fare data files from MTA.info
    %       Show stations with x% more entries than exits.
    /       Show stations with the highest ratio of entrances/exits.
    +       Show stations with the greatest difference between entrances and exits.
    crowded Show stations with the greatest entries per turnstile.
    annual  Calculate annual totals for 2011 (only full year with data).
    common  Show stations in common between lists of stations with most entries and most entries/turnstile
    
    Any other input is interpreted as an SQL command with results displayed.
    
    """
    
    #Establish connection to the db.
    conn = sqlite3.connect('turnstile.db')
    c = conn.cursor()
    while True:
        x = raw_input("Input: ")
        if x == "help" or x == "h":
            print main.__doc__
        elif x == "quit" or x == "q":
            return "Goodbye"
        elif x == "pass":
            continue
        elif x == "init":
            init_db(conn)
        elif x == "init f":
            init_fare(conn)
        elif x == "download" or x == "d":
            input = raw_input("Download all files or new files? (all/new) ")
            if input == "all" or input == "a":
                download_turnstile_files()
            else:
                download_new_turnstile_files()
        elif x == "download fare" or x == "d fare" or x == "d f":
            #Download a separate dataset from the MTA.
            download_fare_files()
        
        elif x == "%":
            #Show stations with x% more entries than exits or between x% and y% more entries than exits.
            x = raw_input("Enter min (or min and max) % increase of entries over exits: ")
            date = "'07-14-12'"
            if is_number(x):
                query = "select stop_name, entries, exits from station_totals where entries > (exits * " + str(1.0 + float(x)/100) + ") and date = " + date
                print_all_rows(c, query)
            else:
                if ',' in x:
                    split = x.split(',')
                    x = split[0].strip()
                    y = split[1].strip()
                    if len(split) > 1 and is_number(x) and is_number(y):
                        query = "select stop_name, entries, exits from station_totals where entries > (exits * " + str(1.0 + float(x)/100) + ") and entries < (exits * " + str(1.0 + float(y)/100) + ") and date = " + date
                        print_all_rows(c, query)
                    else:
                        print "Error: Invalid input. Enter two comma separated numbers to perform this action."
                else:
                    print "Error: Enter one number or two comma separated numbers."
            
        elif x == "/":
            #Show stations with the highest ratio of entrances/exits.
            query = "select stop_name, date, entries, exits, entries/exits as ratio from station_totals order by ratio desc limit 10"
            print_all_rows(c, query)
        elif x == "-/":
            #Show stations with the highest ratio of exits/entrances.
            query = "select stop_name, date, entries, exits, exits/entries as ratio from station_totals order by ratio desc limit 10"
            print_all_rows(c, query)
            
        elif x == "+":
            #Show stations with the greatest difference between entrances and exits.
            print "Stations with the greatest difference between entrances and exits."
            print "Stop name, Date, Entries, Exits"
            query = "select stop_name, date, entries, exits from station_totals order by entries - exits desc limit 10"
            print_all_rows(c, query)    
            
        elif x == "crowded":
            #Show stations with the greatest entries per turnstile.
            print "Stations with the greatest entries per turnstile."
            print "Stop name, Date, Entries, # turnstiles, Ratio"
            query = "select stop_name, date, entries, turnstile_count, ROUND((CAST(entries AS REAL)/turnstile_count), 2) as ratio from station_totals order by ratio DESC limit 10"
            print_all_rows(c, query)
            
        elif x == "annual":
            construct_year_totals(conn)
            
        elif x == "common":
            #Find stations in common between lists of stations with most entries and most entries/turnstile
            print "The busiest stations that are also the busiest per turnstile."
            result_count = 0
            num = 25
            goal = 10
            while result_count < goal:
                x = '''SELECT stop_name
                        FROM (select stop_name, entries from annual_totals_by_name order by entries desc limit {0})
                        WHERE stop_name IN
                             (select stop_name from annual_totals_by_name order by ROUND((CAST(entries AS REAL)/turnstile_count), 2) DESC limit {0})
                        ORDER BY entries
                    '''.format(num)
                c.execute(x)
                rows = c.fetchall()
                result_count = len(rows)
                num += 1
            else:
                for row in rows:
                    print [str(i) for i in row if not is_number(i)] + [i for i in row if is_number(i)]
                print 'Needed to compare the Top {0} Lists to find {1} stations in common.'.format(num, goal)
        elif x == "year sum":
            #System wide totals for 2011.
            print "System wide totals for 2011."
            x = '''SELECT sum(entries), sum(exits), sum(entries) - sum(exits)
                    FROM annual_totals_by_name'''
            print "Entries, exits"
            print_all_rows(c, x)
            
            x = '''SELECT sum(entries), sum(exits), sum(entries) - sum(exits)
                    FROM annual_totals'''
            print "Entries, exits"
            print_all_rows(c, x)
        elif x == "b":
            #b for builtin, execute whatever line is put here
            
            #x = "select exits from raw_data where entries = 3721212"
            #x = "select entries from raw_data where CA = 'A002'"
            #x = "select * from turnstile_totals where stop_name = 'FULTON ST'"
            #x = "select * from turnstile_totals where UNIT = 'R36'"
            #x = "select * from turnstile_totals where UNIT = 'R085' and date = '07-14-12'"
            #x = "select * from station_totals where stop_name = 'ST. GEORGE' order by ENTRIES desc"
            
            x = "select stop_name, turnstile_count, entries, ROUND((CAST(entries AS REAL)/turnstile_count), 2) as ratio from annual_totals_by_name order by ratio DESC limit 10"
            print_all_rows(c, x)
            x = "select * from annual_totals_by_name order by entries desc limit 10"
            print_all_rows(c, x)
        else:
            
            #This is absolutely not secure.
            try:
                print_all_rows(c, x)   
            except sqlite3.OperationalError:
                print "Invalid SQL"
            conn.commit()
    return "success"

def print_all_rows(c, query):
    #Print all results of the given query. Must be passed a cursor.
    try:
        c.execute(query)
        rows = c.fetchall()
        for row in rows:
            #print [str(i) for i in row if not unicode(i).isnumeric()] + [i for i in row if unicode(i).isnumeric()]
            print [str(i) for i in row if not is_number(i)] + [i for i in row if is_number(i)]
        print "Results: " + str(len(rows))
    except sqlite3.OperationalError:
        print "Invalid SQL"
        
def init_db(conn):
    #Create the tables and initialize the database.
    #Reference: http://docs.python.org/library/sqlite3.html
    
    #First we need to create a Cursor object, which we use to execute statements
    c = conn.cursor()

    # Create table
    # Explanation of fields for turnstile data files are from: http://www.mta.info/developers/resources/nyct/turnstile/ts_Field%20Description.txt
    """ C/A = Control Area (A002)
        UNIT = Remote Unit for a station (R051)
        SCP = Subunit Channel Position represents an specific address for a device (02-00-00)
        DATEn = Represents the date (MM-DD-YY)
        TIMEn = Represents the time (hh:mm:ss) for a scheduled audit event
        DEScn = Represent the "REGULAR" scheduled audit event (occurs every 4 hours)
        ENTRIESn = The comulative entry register value for a device
        EXISTn = The cumulative exit register value for a device
    """
    #First drop the table
    try:
        c.execute('''DROP TABLE raw_data''')
    except sqlite3.OperationalError:
        print "raw_data could not be dropped"
    c.execute('''CREATE TABLE raw_data
                 (CA text, UNIT text, SCP text, DATE text, TIME text, ENTRIES integer, EXITS integer, stop_name text)''')
    
    #Let's build a table of stop names
    #These will be associated with the turnstile data
    f = open("data/stations.csv", 'r')
    #The first line is a key
    f.next()
    for line in f:
        pieces = line.split(',')
        names[pieces[0]] = pieces[2]
    
    #Iterate over each datafile, adding the info to the db.
    import glob
    for file in glob.glob("data/turnstile_*.txt"):
        parse_turnstile_file(file, c)
        conn.commit()
        
    try:            
        c.execute('''DROP TABLE turnstile_totals''')
    except sqlite3.OperationalError:
        print "turnstile_totals could not be dropped"
    c.execute('''CREATE TABLE turnstile_totals
                (CA text, UNIT text, SCP text, DATE text, ENTRIES integer, EXITS integer, stop_name text)''')    
    #Records that don't make sense will go here instead. 
    try:
        c.execute('''DROP TABLE turnstile_totals_errors''')
    except sqlite3.OperationalError:
        print "turnstile_totals_errors could not be dropped"
    c.execute('''CREATE TABLE turnstile_totals_errors
                (CA text, UNIT text, SCP text, DATE text, ENTRIES integer, EXITS integer, stop_name text)''')
    #To get daily totals, we compare the starting and ending values.
    c.execute('''select * from raw_data where TIME = "00:00:00"''')
    starts = c.fetchall()
    for i in xrange(len(starts) - 1):
        #If the first three elements match, then it is the same turnstile on two consecutive days.
        #May want to also check that the date on the second one is the day after the first one, but the data is already sorted, so it shouldn't be a big deal.
        if starts[i][0] == starts[i+1][0] and starts[i][1] == starts[i+1][1] and starts[i][2] == starts[i+1][2]:
            t = (starts[i][0], starts[i][1], starts[i][2], starts[i][3], starts[i+1][5] - starts[i][5], starts[i+1][6] - starts[i][6], starts[i][7])
            #We do have to check though that the entry/exit data is increasing. If they aren't, put them instead in the error table for future reference.
            if starts[i+1][5] - starts[i][5] < 0 or starts[i+1][6] - starts[i][6] < 0:
                c.execute('INSERT INTO turnstile_totals_errors VALUES (?,?,?,?,?,?,?)', t)
                #As of 8/17/12, this table has a size of 391 rows compared to 1,255,641 in the main turnstile_totals table.
                #This amounts to 0.031139473% of the data being ignored, but it could still be important so I'll try to look into it.
            else:
                c.execute('INSERT INTO turnstile_totals VALUES (?,?,?,?,?,?,?)', t)
            
    #Now lets do station totals.
    try:
        c.execute('''DROP TABLE station_totals''')
    except sqlite3.OperationalError:
        print "station_totals could not be dropped"
    c.execute('''CREATE TABLE station_totals
                (stop_id text, turnstile_count integer, DATE text, ENTRIES integer, EXITS integer, stop_name text)''')
    #Sort the daily totals by date and station
    c.execute('''select UNIT, count(UNIT), DATE, SUM(ENTRIES), SUM(EXITS), stop_name
                from turnstile_totals
                group by DATE, UNIT''')
    results = c.fetchall() 
    for row in results:
        t = (row[0], row[1], row[2], row[3], row[4], row[5])
        c.execute('INSERT INTO station_totals VALUES (?,?,?,?,?,?)', row)
        
    # Save (commit) the changes
    conn.commit()

    # We can also close the cursor if we are done with it
    c.close()

def init_fare(conn):
    #Create the tables that are based on fare data.
    
    #Cursors made from the connection are used to execute commands.
    c = conn.cursor()
    
    try:
        c.execute('''DROP TABLE raw_fare_data''')
    except sqlite3.OperationalError:
        print "raw_fare_data could not be dropped"
    c.execute('''CREATE TABLE raw_fare_data
                 (REMOTE text, STATION text, FF integer,SEN/DIS integer, 7-D AFAS UNL integer,30-D AFAS/RMF UNL integer,JOINT RR TKT integer,7-D UNL integer,30-D UNL integer,14-D RFM UNL integer,1-D UNL integer,14-D UNL integer,7D-XBUS PASS integer,TCMC integer,LIB SPEC SEN integer,RR UNL NO TRADE integer,TCMC ANNUAL MC integer,MR EZPAY EXP integer,MR EZPAY UNL integer,PATH 2-T integer,AIRTRAIN FF integer,AIRTRAIN 30-D integer,AIRTRAIN 10-T integer,AIRTRAIN MTHLY integer, STUDENTS integer)''')
    
    #Iterate over each datafile, adding the info to the db.
    import glob
    for file in glob.glob("fares/fares_*.csv"):
        parse_fare_file(file, c)
        conn.commit()
    
def is_number(s):
    try:
        float(s)
        return True
    except ValueError:
        return False

def parse_turnstile_file(file, c):
    #Insert into db the data from a turnstile data file.
    #Args: file:name of file to be parsed, c:cursor created from the connection to the db
    f = open(file, 'r')
    for line in f:
        pieces = line.split(',')
        #The lines have a lot of spaces between the last item and the newline character.
        pieces[-1] = pieces[-1].strip()
        #Each line has multiple readings for the same turnstile, so the first few pieces get reused
        CA = pieces[0]
        UNIT = pieces[1]
        SCP = pieces[2]
        #This loop has us iterate over each date/time/entries/exits set.
        for i in xrange(3, len(pieces) - 5, 5):
            if UNIT in names: 
                t = (CA, UNIT, SCP, pieces[i], pieces[i+1], pieces[i+3], pieces[i+4], names[UNIT])
            else:
                t = (CA, UNIT, SCP, pieces[i], pieces[i+1], pieces[i+3], pieces[i+4], UNIT)
            c.execute('INSERT INTO raw_data VALUES (?,?,?,?,?,?,?,?)', t)
    print file + " parsed"
    
def download_turnstile_files():
    #Download all data files listed on http://www.mta.info/developers/turnstile.html
    #If the webpage's address changes, this will have to be updated.
    from lxml import etree
    parser = etree.HTMLParser()
    tree = etree.parse("http://www.mta.info/developers/turnstile.html", parser)
    
    #The list the urls will go in.
    urls = []
    #Uses an XPATH expression to find the div that contains the list of data files.
    r = tree.find(".//div[@class='span-14 last']")
    #for each element in the div, if it is an <a>, we put the url in the list
    for child in r:
        if child.tag == 'a':
            urls.append(child.get('href'))
    
    #The following actually downloads the files. 
    #This would take a long time, so for now let's have it be done elsewhere, using wget.
    #import urllib2        
    #for url in urls:
    #    f = urllib2.urlopen("http://www.mta.info/developers/" + url)
    #    with open("data/" + url.split('/')[-1], "wb") as code:
    #        code.write(f.read())
   
    f = open("data/links.txt", "a")
    for url in urls:
        f.write("http://www.mta.info/developers/" + url + '\n')
    #In CMD, navigate to /data, then execute wget -i links.txt

def download_new_turnstile_files():
    #Check for and download data files that haven't yet been downloaded.
    from lxml import etree
    parser = etree.HTMLParser()
    tree = etree.parse("http://www.mta.info/developers/turnstile.html", parser)
    
    urls = []
    #Open the links.txt file. We will be checking new urls against the already listed ones.
    with open("data/links.txt", "r+") as f:
        for line in f:
            urls.append(line.rstrip())
    
    #Uses an XPATH expression to find the div that contains the list of data files.
    r = tree.find(".//div[@class='span-14 last']")
    #for each element in the div, if it is an <a>, we keep it if it isn't in the list of existing urls
    newurls = []
    for child in r:
        if child.tag == 'a':
            #The URLs on the page are relative. If the website's structure were to change, this will have to be updated.
            curr = "http://www.mta.info/developers/" + child.get('href')
            if curr not in urls:
                newurls.append("\n" + curr)
    
    print str(len(newurls)) + " files found to download."
    if len(newurls) > 0:
        import urllib2
        #We are writing the newly downloaded files' names to this file for future reference.
        with open("data/links.txt", "a") as links:
            print "Downloading..."
            #for each file not yet downloaded
            for url in newurls:
                #open the url
                f = urllib2.urlopen(url)
                #Python's with (as) statement can automate some things, in this case it closes the file for us
                #open a local file
                with open("data/" + url.split('/')[-1], "wb") as code:
                    #write the url's contents to the local file
                    code.write(f.read())
                    #write the url to the links file so we don't download it next time.
                    links.write(url)
                    print "Downloaded:" + url
                
def download_fare_files():
    #Download the fare data distributed by the MTA at http://www.mta.info/developers/fare.html
    #This downloads ~115 files, each 170 kb, so don't call it more than once.
    
    from lxml import etree
    parser = etree.HTMLParser()
    tree = etree.parse("http://www.mta.info/developers/fare.html", parser)
    
    #The list the urls will go in.
    urls = []
    #Uses an XPATH expression to find the div that contains the list of data files.
    print "Parsing http://www.mta.info/developers/fare.html"
    r = tree.find(".//div[@class='span-19 last']")
    #for each element in the div, if it is an <a>, we put the url in the list
    for child in r:
        if child.tag == 'a':
            urls.append(child.get('href'))
    print "Page parsed. {0} links found.".format(len(urls))
    
    #The following actually downloads the files. 
    import urllib2        
    for url in urls:
        f = urllib2.urlopen("http://www.mta.info/developers/" + url)
        with open("fares/" + url.split('/')[-1], "wb") as code:
            code.write(f.read())
        print "Downloaded {0}".format(url)
    with open("fares/links.txt", "a") as f:
        for url in urls:
            f.write("http://www.mta.info/developers/" + url + '\n')
    print "Download complete."


    
def construct_year_totals(conn):
    #Given the database connections, create a table of annual totals for 2011, the only full year with data.
    c = conn.cursor()
    #These totals are by turnstile
    try:            
        c.execute('''DROP TABLE annual_totals''')
        print "annual_totals dropped"
    except sqlite3.OperationalError:
        print "annual_totals could not be dropped"
    c.execute('''CREATE TABLE annual_totals
                (CA text, UNIT text, SCP text, ENTRIES integer, EXITS integer, stop_name text)''')
    print "annual_totals created"
    c.execute('''SELECT CA, UNIT, SCP, sum(ENTRIES), sum(EXITS), stop_name
                FROM turnstile_totals
                WHERE DATE LIKE "__-__-11"
                GROUP BY CA, UNIT, SCP''')
    
    totals = c.fetchall()
    for line in totals:
        c.execute('INSERT INTO annual_totals VALUES (?,?,?,?,?,?)', line)
    print "annual_totals populated"
    conn.commit()
    
    #Now lets compile those into totals by station.
    try:            
        c.execute('''DROP TABLE annual_totals_by_name''')
        print "annual_totals_by_name dropped"
    except sqlite3.OperationalError:
        print "annual_totals_by_name could not be dropped"
    c.execute('''CREATE TABLE annual_totals_by_name
                (stop_name text, turnstile_count integer, ENTRIES integer, EXITS integer)''')
    print "annual_totals_by_name created"
    c.execute('''SELECT stop_name, COUNT(*), sum(ENTRIES), sum(EXITS)
                FROM annual_totals
                GROUP BY stop_name''')
    
    totals = c.fetchall()
    for line in totals:
        c.execute('INSERT INTO annual_totals_by_name VALUES (?,?,?,?)', line)
    print "annual_totals_by_name populated"
    conn.commit()
    
if __name__ == "__main__":
    print main()
    
  
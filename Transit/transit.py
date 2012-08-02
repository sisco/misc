#Transit Analysis
#Brian Sisco
#Started July 28, 2012

import sqlite3
names = dict()

def main():
    #Establish connection to the db.
    conn = sqlite3.connect('turnstile.db')
    c = conn.cursor()
    while True:
        x = raw_input("Input: ")
        if x == "init":
            init_db(conn)
        elif x == "quit" or x == "q":
            return "Goodbye"
        elif x == "pass":
            continue
        elif x == "%":
            #Show stations with x% more entries than exits.
            x = raw_input("Enter % increase of entries over exits: ")
            query = "select stop_name, entries, exits from station_totals where entries > (exits * " + str(1.0 + float(x)/100) + ") and date = '07-14-12'"
            print_all_rows(c, query)
        elif x == "b":
            #b for builtin, execute whatever line is put here
            
            #x = "select exits from raw_data where entries = 3721212"
            #x = "select entries from raw_data where CA = 'A002'"
            #x = "select * from turnstile_totals where stop_name = 'FULTON ST'"
            #x = "select * from turnstile_totals where UNIT = 'R36'"
            x = "select * from turnstile_totals where UNIT = 'R085' and date = '07-14-12'"
            
            print_all_rows(c, x)
        else:
            
            #This is absolutely not secure.
            try:
                c.execute(x)
                print c.fetchone()
                
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
            print row
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
                 (CA text, UNIT text, SCP text, DATE text, TIME text, ENTRIES integer, EXITS integer, stop_name)''')
    
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
        print file
        parse_turnstile_file(file, c)
    
    try:
        c.execute('''DROP TABLE turnstile_totals''')
    except sqlite3.OperationalError:
        print "turnstile_totals could not be dropped"
    c.execute('''CREATE TABLE turnstile_totals
                (CA text, UNIT text, SCP text, DATE integer, ENTRIES integer, EXITS integer, stop_name)''')
    #To get daily totals, we compare the starting and ending values.
    c.execute('''select * from raw_data where TIME = "00:00:00"''')
    starts = c.fetchall()
    for i in xrange(len(starts) - 1):
        #If the first three elements match, then it is the same turnstile on two consecutive days.
        #May want to also check that the date on the second one is the day after the first one, but they data is already sorted, so it shouldn't be a big deal.
        if starts[i][0] == starts[i+1][0] and starts[i][1] == starts[i+1][1] and starts[i][2] == starts[i+1][2]:
            t = (starts[i][0], starts[i][1], starts[i][2], starts[i][3], starts[i+1][5] - starts[i][5], starts[i+1][6] - starts[i][6], starts[i][7])
            c.execute('INSERT INTO turnstile_totals VALUES (?,?,?,?,?,?,?)', t)
            
    #Now lets do station totals.
    try:
        c.execute('''DROP TABLE station_totals''')
    except sqlite3.OperationalError:
        print "station_totals could not be dropped"
    c.execute('''CREATE TABLE station_totals
                (stop_id text, turnstile_count integer, DATE integer, ENTRIES integer, EXITS integer, stop_name text)''')
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
            t = (CA, UNIT, SCP, pieces[i], pieces[i+1], pieces[i+3], pieces[i+4], names[UNIT])
            c.execute('INSERT INTO raw_data VALUES (?,?,?,?,?,?,?,?)', t)
    
if __name__ == "__main__":
    print main()
    
  
#Brian Sisco
#MMG Bet Archive Parser

def main():
    print 'Please go to MMG, then click "Look at your Bet Archive" and optionally click "Show Full Two Weeks". Right click, select "View frame source". Right click, select "Save as..."' 
    print "More directly: go to /betarchive.php?showall=1 in KoL and download the source."
    print "Make sure that file and this file, mmg.py, are in the same folder."
    file_name = raw_input("Enter the name of the file you downloaded (including the extension): ")
    #First we'll parse the file.
    from lxml import etree
    parser = etree.HTMLParser()
    tree = etree.parse(file_name, parser)
    
    #Find all the table rows because each bet is on its own row.
    trs = tree.findall(".//tr")
    #Recursively search the table rows for text.
    rows = []
    for tr in trs:
        item = []
        append_recurse(item, tr)
        rows.append(item)
    #Trim this to just get the relevent items.
    bets = []
    for i in xrange(len(rows)):
        #Get the ones of the right length, but not the headers.
        if len(rows[i]) == 5 and rows[i][3] != "Who Won":
            bets.append(rows[i])
    #Initialize the dictionaries that will hold the data.
    wins = dict()
    loses = dict()
    totals = dict()
    #Each bet is a list of length 5. The name and amount are at indices 2 and 4.
    for bet in bets:
        name = bet[2]
        #Following line converts, for example, '-64,000 Meat' to '-64000'
        delta = (bet[4].split(" "))[0].replace(",", "")
        #Initialize a users entries in the dicts the first time they are seen.
        if name not in totals:
            totals[name] = 0
            wins[name] = 0
            loses[name] = 0
        try:
            #print "D: " + delta + " ID " + unicode(int(delta))
            delta = int(delta)
            totals[name] = totals[name] + delta
            if delta > 0:
                wins[name] += 1
            else:
                loses[name] += 1
        except ValueError:
            print "ValueError on int'ing bet:"
            print "{} {}".format(name, delta)

    #Keeping track of some totals to display later
    total_delta = 0
    total_wins = 0
    total_loses = 0
            
    #Print the header and then the info
    print "Opponent               Your Gain   Wins  Loses  Ratio"
    #Here they are sorted by the amount they gave you and displayed in a (hopefully) nice table.
    for x in sorted(totals, key=totals.get, reverse=True):
        #Add the thousand separators to the total.
        total = "{:,}".format(totals[x])
        ratio = 1
        if loses[x] != 0:
            ratio = round(float(wins[x])/loses[x], 3)
        print x.ljust(20), total.rjust(11), str(wins[x]).rjust(6), str(loses[x]).rjust(6), " ", ratio
        total_delta += totals[x]
        total_wins += wins[x]
        total_loses += loses[x]
    #Now let's print a row of totals.
    ratio = 1
    if total_loses != 0:
        ratio = round(float(total_wins)/total_loses, 3)
    print
    print "Totals:".ljust(20), "{:,}".format(total_delta).rjust(11), str(total_wins).rjust(6), str(total_loses).rjust(6), " ", ratio
    
def append_recurse(list, parent):
    if parent.text != None:
        list.append(parent.text.replace(u"\u2013", '-'))
    for child in parent:
        append_recurse(list, child)
    return list         
    
if __name__ == "__main__":
    main()
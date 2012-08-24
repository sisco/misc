#Brian Sisco
#MMG Bet Archive Parser

raw_data = []

def main():
    print "Please go to /betarchive.php?showall=1 in KoL and download the source."
    file_name = raw_input("Enter the name of the file you downloaded: ")
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
        counts[name] = counts[name] + 1
        try:
            #print "D: " + delta + " ID " + unicode(int(delta))
            delta = int(delta)
            totals[name] = totals[name] + delta
            if delta > 0:
                wins[name] += 1
            else
                loses[name] += 1
        except ValueError:
            print "ValueError on int'ing bet:"
            #x = "{} {} {} {} {}".format(raw_data[x], raw_data[x + 1], raw_data[x + 2], raw_data[x + 3], raw_data[x + 4]).replace(u'\xa0', ' ').encode('utf-8')
            print "{} {}".format(name, delta)

    #Print the header and then the info
    print "Opponent            Your Gain          Wins   Loses    "
    #Here they are sorted by the amount they gave you and displayed in a (hopefully) nice table.
    for x in sorted(totals, key=totals.get, reverse=True):
        #Add the thousand separators to the total.
        total = "{:,}".format(totals[x])
        print x.ljust(20), total.rjust(11), 
    
def append_recurse(list, parent):
    if parent.text != None:
        list.append(parent.text.replace(u"\u2013", '-'))
    for child in parent:
        append_recurse(list, child)
    return list
        
def print_recurse(parent):
    print_contents(parent)
    for child in parent:
        print_recurse(child)
                    
def print_contents(element):
    if element.text != None:
        x = element.text
        print x.replace(u"\u2013", '-')
    if element.tail != None:
        x = element.tail
        print x.replace(u"\u2013", '-')
                        
    
if __name__ == "__main__":
    main()
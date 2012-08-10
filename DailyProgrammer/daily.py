#Brian Sisco
#Solutions for problems posed at http://www.reddit.com/r/dailyprogrammer

def main():
    #User 
    func = input("Which problem shall we solve? ")
    try:
        (func)()
    except NameError:
        print "Sorry, that is not the name of a function."
        
#Naming convention: a, b, c for easy, intermediate, difficult, followed by the number.

def b86():
    """Calculate the day of the week on any date in history"""
    #Reference: http://en.wikipedia.org/wiki/Doomsday_rule
    print b86.__doc__
    print "Please enter the following as integers."
    day = int(input("What day of the month? "))
    month = int(raw_input("What month? "))
    year = int(raw_input("What year? "))
    
    #Indexing into this list will give the text form of the answer when we are done.
    names = ["Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"]
    
    #The "memorable date" of each month.
    memorable = [3, 0, 0, 4, 9, 6, 11, 8, 5, 10, 7, 12]
    #January and February's memorable dates are different in leap years.
    if is_leap_year(year):
        memorable[0] = 4
        memorable[1] = 1
    
    #In this formula the divisions are to be floored, but integer division essentially does that for us.
    doomsday = (2 + year + year / 4 - year / 100 + year / 400) % 7
    print "Doomsday: " + names[doomsday]

    #Subtract one because the list is zero indexed, but colloquially January is 1.
    distance = day - memorable[month - 1]
    answer = (distance + doomsday) % 7
    
    print "Answer: " + names[answer]

def is_leap_year(year):
    #Return True if the given year is a leap year, False if it isn't.
    year = int(year)
    if year % 400 is 0:
        return True
    elif year % 100 is 0:
        return False
    elif year % 4 is 0:
       return True
    else:
       return False
       
       
def c():
    print "yay"
        
	
if __name__ == "__main__":
    main()

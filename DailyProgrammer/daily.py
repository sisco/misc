#Brian Sisco
#Solutions for problems posed at http://www.reddit.com/r/dailyprogrammer

def main():
    #User 
    func = input("Which problem shall we solve? ")
    try:
        (func)()
    except NameError:
        print "Sorry, that is not the name of a function. (Or it is and it caused a NameError)"
        
#Naming convention: a, b, c for easy, intermediate, difficult, followed by the number.

class Rect():
    #Rectangle Class
    #(x1, y1) is the top-left corner, (x2, y2) is the bottom-right corner.
    x1 = 0
    y1 = 0
    x2 = 0
    y2 = 0
    
    def __init__(self):
        x1 = 0
        y1 = 0
        x2 = 0
        y2 = 0
        
    def __init__(self, a, b, c, d):
        self.x1 = a
        self.y1 = b
        self.x2 = c
        self.y2 = d
        
    def __repr__(self):
        return "({}, {})~({}, {})".format(self.x1, self.y1, self.x2, self.y2)

def a87():
    """Write a function that calculates the intersection of two rectangles, returning either a new rectangle or some kind of null value.
    """
    x = Rect(3, 10, 10, 3)
    y = Rect(6, 12, 12, 6)
    print rectIntersection(x, y)
    
def rectIntersection(a, b):
    #Returns a Rect (or None) representing the intersection of the two Rects it is given.
    #This assumes a Cartesian coordinate system, not a screen based one with (0,0) in the upper left.
    #Check cases of zero intersection
    #second is above first
    if b.y1 > a.y1 and b.y2 > a.y1:
        return None
    #second is below first
    if b.y1 < a.y2 and b.y2 < a.y2:
        return None
    #second is to the right of first
    if b.x1 > a.x2 and b.x2 > a.x2:
        return None
    #second is to the left of first
    if b.x1 < a.x1 and b.x2 < a.x1:
        return None
        
    #Assuming each Rect is well defined cuts down on the options/complexity.
    #By well defined I mean x1<x2, y1>y2 due to the Rect being defined by top-left and bottom-right points.
    
    #Initializing the return values.
    ax, ay, bx, by = 0, 0, 0, 0
    #Find the top-left corner of the intersection
    ax = max(a.x1, b.x1)
    ay = min(a.y1, b.y1)
    #Find the bottom-right corner of the intersection
    bx = min(a.x2, b.x2)
    by = max(a.y2, b.y2)
    
    return Rect(ax, ay, bx, by)

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

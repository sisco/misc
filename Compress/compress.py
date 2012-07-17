#Brian Sisco
#String compression algorithm
#7/6/12

import sys

def main():
    """Compress/decompress a string based on command line arguments
    
    To compress: python compress.py -c string_to_modify
    To decompress: python compress.py -d string_to_modify
    
    """
    
    if len(sys.argv) >= 3:
        if sys.argv[1] == "-c":
            print compress(sys.argv[2])
        elif sys.argv[1] == "-d":
            print decompress(sys.argv[2])
    else:
        print main.__doc__

def compress(input):
    """Compress strings by partially replacing runs of the same character with a digit.
    
To produce the compressed output, any run of two or more of the same
character should be converted to two of that character plus a number
indicating how many repeated runs were compressed
    
    """
    
    #"A valid input consists of one or more upper case english letters A-Z"
    #Comment out the next two lines to accept invalid inputs. Manual testing showed they are handled smoothly, ex: @@@@ -> @@2
    if not input.isalpha() or not input.isupper():
        return "Invalid input. A valid input consists of one or more upper case english letters A-Z"
    #The first character of the input is always included in the compression
    output = input[0:1]
    count = 1
    #This flag is set when we have a run of 11 of the same character in a row
    too_long = False
    for i in range(1, len(input)):
        if input[i] != input[i-1] or too_long:
            #This is the start of a new run, either because the characters didn't match or because the run was too long
            #Append a digit, if needed, for the length of the previous run
            if count > 1:
                output += str(count - 2)
                too_long = False
            #Add the character that begins the next run.
            output += input[i]
            count = 1
        else:
            #The characters match, so we increase the length of this run
            count += 1
            #We display the second instance of a character
            if count == 2:
                output += input[i]
            #We can only compress so much, so if the length of a run is too high we break it up
            elif count > 10:
                #set the flag for it be broken up on the next pass through the loop
                too_long = True
        #After the last character, we may need a digit for replaced characters
        if i == len(input) - 1:
            if count > 1:
                output += str(count - 2)
    return output

def decompress(input):
    """Decompress strings assuming they were compressed using the above algorithm."""
    output = input[0:1]
    for i in range(1, len(input)):
        if input[i].isalpha():
            output += input[i]
        elif input[i].isdigit():
            output += input[i-1] * int(input[i])
        #If the input is invalid, we'll pass along the odd bits to avoid a loss of data.
        elif not input[i].isalnum():
            output += input[i]
    return output  
    
if __name__ == "__main__":
    main()
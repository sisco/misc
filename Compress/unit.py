#Brian Sisco
#Unit testing for the String compression algorithm, compress.py
#7/6/12

from compress import compress, decompress

def main():
    #Test compression
    assert compress("A") == "A"
    assert compress("ABAC") == "ABAC"
    assert compress("AA") == "AA0"
    assert compress("AAB") == "AA0B"
    assert compress("AABB") == "AA0BB0"
    assert compress("AAACBBC") == "AA1CBB0C"
    
    #Test strings with long runs of the same character
    assert compress("AAA") == "AA1"
    assert compress("AAAB") == "AA1B"
    assert compress("AAABB") == "AA1BB0"
    assert compress("AAABBB") == "AA1BB1"
    assert compress("AAABBBB") == "AA1BB2"
    assert compress("AAABBBBB") == "AA1BB3"
    assert compress("AAABBBBBB") == "AA1BB4"
    assert compress("AAABBBBBBB") == "AA1BB5"
    assert compress("AAABBBBBBBB") == "AA1BB6"
    assert compress("AAABBBBBBBBB") == "AA1BB7"
    assert compress("AAABBBBBBBBBB") == "AA1BB8"
    assert compress("AAABBBBBBBBBBB") == "AA1BB9"
    assert compress("AAABBBBBBBBBBBB") == "AA1BB9B"
    assert compress("AAABBBBBBBBBBBBB") == "AA1BB9BB0"
    assert compress("AAABBBBBBBBBBBBBB") == "AA1BB9BB1"
    assert compress("AAABBBBBBBBBBBBBBB") == "AA1BB9BB2"
    assert compress("AAABBBBBBBBBBBBBBBB") == "AA1BB9BB3"
    
    #Test decompression
    assert decompress("A") == "A"
    assert decompress("ABAC") == "ABAC"
    assert decompress("AA0") == "AA"
    assert decompress("AA0B") == "AAB"
    assert decompress("AA0BB0") == "AABB"
    assert decompress("AA1CBB0C") == "AAACBBC"
    
    #Test that decompress acts as an inverse of compress.
    assert decompress(compress("A")) == "A"
    assert decompress(compress("ABAC")) == "ABAC"
    assert decompress(compress("AA")) == "AA"
    assert decompress(compress("AAB")) == "AAB"
    assert decompress(compress("AABB")) == "AABB"
    assert decompress(compress("AAACBBC")) == "AAACBBC"
    
    print "Compression/decompression tests successful."
    
if __name__ == "__main__":
    main()
def parse(s, parent):
    i = digitstart = 1 #the first character in s will always be '['. Don't need it, so skip it.
    while i < len(s):
        if s[i] == ',' and s[i-1] != ']': #if the character before a comma is ], that means there's no digit to parse. otherwise, there is, so parse the digit.
            parent.append(int(s[digitstart:i]))
            digitstart = i+1
        elif (s[i] == '['): #begin a sublist, and parse that sublist. parse returns the number of characters eaten, so the i += on line nine jumps to the end of the sublist.
            l = []
            i += parse(s[i:], l)
            parent.append(l)
            digitstart = i + 2
        elif (s[i] == ']'): #we reached the end of a [sub]list. if the previous character is a [ (ie: the sublist is []) or ] (ie the fourth char in: [[]]), there's no digit to parse, so just return the number of characters eaten. otherwise, parse the remaining digit and return the number of characters eaten
            if (s[i - 1] == '[' or s[i-1] == ']'):
                return i
            parent.append(int(s[digitstart:i]))
            return i
        i += 1
        
with open("S:/Projections/Dan/Coding Group/input.txt") as f:
    inputs = filter(lambda x: x != '\n', f.readlines())
    parsed_inputs = []
    for _ in inputs:
        parent = []
        parse(_, parent)
        parsed_inputs.append(parent)

    for _ in parsed_inputs:
        print(_)

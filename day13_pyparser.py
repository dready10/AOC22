def parse(s, parent):
    i = digitstart = 1
    while i < len(s):
        if s[i] == ',' and s[i-1] != ']':
            parent.append(int(s[digitstart:i]))
            digitstart = i+1
        elif (s[i] == '['):
            l = []
            i += parse(s[i:], l)
            parent.append(l)
            digitstart = i + 2
        elif (s[i] == ']'): 
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

def parse(s):
    parent = []
    i = digitstart = 1
    while i < len(s)-1:
        if s[i] == ',' and s[i-1] != ']':
            parent.append(int(s[digitstart:i]))
            digitstart = i+1
        elif (s[i] == '['):
            parent.append(parse(s[i:]))
            lbracketcount = 1
            i += 1
            while lbracketcount != 0:
                if s[i] == '[':
                    lbracketcount += 1
                if s[i] == ']':
                    lbracketcount -= 1
                    # we want s[i] == ']' after we exit the loop so that line 25
                    # increments us to the point after ]. if we didn't have this,
                    # we'd leave this elif with s[i] being the character after ']',
                    # and then line 25 would increment us on further character.
                    i -= 1
                i += 1
            # + 2 because after ']' is always a comma or ']' which can't be a digit
            # start. 2 after ] can only be a digit OR a [. but if it's a [, the subsequent
            # recursion will update digit start to be 2 after the ] ending the sublist.
            # so we only end up in a place where, when we check if s[i] == ',', digit start
            # is only pointing at the start of a digit.
            digitstart = i + 2
        elif (s[i] == ']'): #we reached the end of a [sub]list. if the previous character is a [ (ie: the sublist is []) or ] (ie the fourth char in: [[]]), there's no digit to parse, so just return the number of characters eaten. otherwise, parse the remaining digit and return the number of characters eaten
            if (s[i - 1] == '[' or s[i-1] == ']'):
                return parent
            parent.append(int(s[digitstart:i]))
            return parent
        i += 1
    return parent
        
with open("input/day13.txt") as f:
    inputs = filter(lambda x: x != '\n', f.readlines())
    parsed_inputs = []
    for _ in inputs:
        parsed_inputs.append(parse(_))

    for _ in parsed_inputs:
        print(_)
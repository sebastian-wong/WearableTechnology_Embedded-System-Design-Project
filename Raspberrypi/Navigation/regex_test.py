import re
from Speechrecogniser import SpeechRecognition

##print(re.split(r'\s*', 'here are some words'))
##print(re.split(r'(s*)', 'here are some words'))
##print(re.split(r'[a-f]', 'finqwenlaskdjriewasd'))
##print(re.split(r'[a-f]', 'finqwenlaskdjriewasd'), re.I|re.M) #re.I = ignorecase #re.M = multiline
##print(re.split(r'[a-df-z0-9]', 'e8rvp98rvPADOIVJW48O;BJSIDJFawe'), re.I|re.M)
##print(re.split(r'[a-df-z0-9]', 'e8rvp98rvPADOIVJW48O;BJSIDJFawe'), re.I|re.M)
##print(re.findall(r'\d', 'finqwen324 main st.fawbhevibwhve'), re.I|re.M)
#\d = digits
#\D = non-digits
#\S = non-space
#print(re.findall(r'\d', 'finqwen324 main st.fawbhevibwhve'), re.I|re.M)
#* = 0 or more
#+ = 1 or more
#? 0 or 1 of...
#{5} = exactly 5 of...
#{1,60} = range from 1-60
#\w = alphanumeric
#\s = space
#print(re.findall(r'\d{1,5}\s\w+\s\w+', 'finSFSen324 main st.fawbFEWhve'), re.I|re.M)
#\. = regular full stop
#. = any character but newline(\n)

def text2int(textnum, numwords={}):
    if not numwords:
      units = [
        "zero", "one", "two", "three", "four", "five", "six", "seven", "eight",
        "nine", "ten", "eleven", "twelve", "thirteen", "fourteen", "fifteen",
        "sixteen", "seventeen", "eighteen", "nineteen",
      ]

      tens = ["", "", "twenty", "thirty", "forty", "fifty", "sixty", "seventy", "eighty", "ninety"]

      scales = ["hundred", "thousand", "million", "billion", "trillion"]

      numwords["and"] = (1, 0)
      for idx, word in enumerate(units):    numwords[word] = (1, idx)
      for idx, word in enumerate(tens):     numwords[word] = (1, idx * 10)
      for idx, word in enumerate(scales):   numwords[word] = (10 ** (idx * 3 or 2), 0)

    current = result = 0
    for word in textnum.split():
        if word not in numwords:
          raise Exception("Illegal word: " + word)

        scale, increment = numwords[word]
        current = current * scale + increment
        if scale > 100:
            result += current
            current = 0

    return result + current


#sample_array = ['TO L T FIFTEEN', 'SEMINAR ROOM TO', 'N U S HACKER\'S ROOM', 'COM ONE', 'TO COM TO HYPHEN TO', 'TWO', 'TO']
def stringParser(sample_this)
    sample = re.split(r'\s*', sample_this)
    sample_string = ''
    for j in range(len(sample)):
        if ((j > 0 or len(sample) == 1) and sample[j] == 'TO'):
            sample[j] = 'TWO'
        try:
            number = text2int(sample[j].lower())
            sample[j] = number
        except:
            pass
        #print sample[j]
    for k in range(len(sample)):
        if sample[k] == 'COM':
            sample_string = sample_string + sample[k]
        elif ( sample[k] == 'HYPHEN'):
            sample_string = sample_string + '-'
        elif ( type(sample[k]) is type(str()) ) and ( len(sample[k]) == 1 ) and ( k < len(sample) - 1 ) \
           and ( ( ( type(sample[k+1]) is type(str()) ) and ( len(sample[k+1]) == 1 ) ) or ( type(sample[k+1]) is int ) ):
                sample_string = sample_string + sample[k]
        elif ( k < len(sample) - 1 ) and ( sample[k+1] == 'HYPHEN' ):
            sample_string = sample_string + str(sample[k])
        else:
            sample_string = sample_string + str(sample[k]) + ' '
    print sample_string


speechInput = SpeechRecognition()
building = speechInput.speechRecognise()
level = speechInput.speechRecognise()

building = stringParser(building)
level = stringParser(level)

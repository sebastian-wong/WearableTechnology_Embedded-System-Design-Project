import os
import time

from espeak_cls import AudioFeedback

output = AudioFeedback()
string = "Hello Chew Yi Xiu "
output.threadedFeedback(string)
time.sleep(2)


while True:		
	string = "Yanqing likes someone"
	output.threadedFeedback(string)
	time.sleep(2)


	string = "Angela has tin tin"
	output.threadedFeedback(string)
	time.sleep(2)


	string = "It's bob the builder"
	output.threadedFeedback(string)
	time.sleep(2)


	string = "Can we fix it?"
	output.threadedFeedback(string)
	time.sleep(2)

	string = "It's bob the builder"
	output.threadedFeedback(string)
	time.sleep(2)

	string = "YES WE CAN!"
	output.threadedFeedback(string)
	time.sleep(2)

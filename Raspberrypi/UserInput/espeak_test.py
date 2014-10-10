import os
import time

from espeak_cls import AudioFeedback

output = AudioFeedback()
string = "Hello Chew Yi Xiu "
output.Feedback(string)
time.sleep(2)


while True:		
	string = "Yanqing likes someone"
	output.Feedback(string)
	time.sleep(1)


	string = "Angela has tin tin"
	output.Feedback(string)
	time.sleep(1)


	string = "It's bob the builder"
	output.Feedback(string)
	time.sleep(1)


	string = "Can we fix it?"
	output.Feedback(string)
	time.sleep(1)

	string = "It's bob the builder"
	output.Feedback(string)
	time.sleep(1)

	string = "YES WE CAN!"
	output.Feedback(string)
	time.sleep(1)

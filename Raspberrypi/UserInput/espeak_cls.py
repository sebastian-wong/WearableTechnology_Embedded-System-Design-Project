#!/usr/bin/env python
import subprocess

class AudioFeedback(object):
	def __init__ (self):
		self._process = None

	def Feedback(self,text):
		#Speaking with amplitude=200, pitch=50, speed=150, capital=20, language= english-rp, voice=female2
		#self._process = subprocess.Popen(['espeak -a200 -p50 -s150 -k20 -ven-rp+f2', text])
		self._process = subprocess.Popen(['espeak', text])
		#self._process.communicate()		
		self._process.terminate()
		self._process.poll();
		#normal return statement should suffice
		#return	self._process.returncode

#to be executed when called by main program
if __name__ == '__main__':
	text = "hello world"
	output = AudioFeedback()
	output.Feedback(text)

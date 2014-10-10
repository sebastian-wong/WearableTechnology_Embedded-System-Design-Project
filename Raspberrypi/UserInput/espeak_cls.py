#!/usr/bin/env python
import subprocess
import threading

class AudioFeedback(object):
	def __init__ (self):
		self._process = None

	def Feedback(self,text,timeout=None):

		#Speaking with amplitude=200, pitch=50, speed=150, capital=20, language= english-rp, voice=female2
		self._process = subprocess.Popen(['espeak', '-a200', '-p50', '-s150', '-k20','-ven-rp+f2', text])
		#wait for subprocess to terminate
		self._process.wait()
		
		

#testing AudioFeedback class with python
if __name__ == "__main__":
	text = "hello world"
	output = AudioFeedback()
	output.Feedback(text)

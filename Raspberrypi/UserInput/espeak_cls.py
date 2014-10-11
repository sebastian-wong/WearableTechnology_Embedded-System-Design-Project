#!/usr/bin/env python
import subprocess
import threading

class AudioFeedback(object):
	def __init__ (self):
		self._process = None


	#threaded feedback so that audio feedback that is not spoken in time will expire in stipulated time	
	def threadedFeedback(self,text, timeout=None):
		def Feedback():
			#Speaking with amplitude=200, pitch=50, speed=150, capital=20, language= english-rp, voice=female2
			self._process = subprocess.Popen(['espeak', '-a200', '-p50', '-s150', '-k20', '-ven-rp+f2', text])
			self._process.poll()
		

		feedbackThreads = threading.Thread(group=None, target = Feedback)
		feedbackThreads.start()

		#Wait until the thread terminates
		feedbackThreads.join(timeout)
		#check if join has expired
		if feedbackThreads.is_alive():
			#kill the process	
			self._process.kill()
		return self._process.returncode	




#to be executed when called by main program
if __name__ == '__main__':
	text = "hello world"
	output = AudioFeedback()
	output.threadedFeedback(text)

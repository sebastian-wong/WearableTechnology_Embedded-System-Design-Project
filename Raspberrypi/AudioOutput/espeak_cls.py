#!/usr/bin/env python
import subprocess
import threading

class AudioFeedback(object):
	def __init__ (self):
		self._process = None
		#self.lock = threading.Lock()


	#threaded feedback to increase efficiency	
	def threadedFeedback(self,text, timeout=None):
		def Feedback():
			#Speaking with amplitude=200, pitch=50, speed=150, capital=20(after speed, -k120, language= english-rp, voice=female2
			self._process = subprocess.Popen(['espeak', '-a200', '-p50', '-s120', '-ven-rp+f2', text])
			#poll does not wait for espeak to finish the current sentence and starts on a second sentence, undesirable
			#use communicate for now
			#self._process.poll()
			self._process.communicate()


			
		
		feedbackThreads = threading.Thread(group=None, target = Feedback)
		#feedbackThreads.lock.acquire()
		feedbackThreads.start()

		#Wait until the thread terminates
		feedbackThreads.join(timeout)
		#feedbackThreads.lock.release()
		#check if join has expired
		if feedbackThreads.is_alive():
			#kill the process	
			self._process.kill()
			feedBackThreads.join()
		return self._process.returncode	




#to be executed when called by main program
if __name__ == '__main__':
	text = "hello world"
	output = AudioFeedback()
	output.threadedFeedback(text)

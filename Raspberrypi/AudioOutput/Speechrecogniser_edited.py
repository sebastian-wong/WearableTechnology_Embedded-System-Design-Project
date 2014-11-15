# ====================================================================
# Copyright (c) 2013 Carnegie Mellon University.  All rights
# reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer. 
#
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in
#    the documentation and/or other materials provided with the
#    distribution.
#
# This work was supported in part by funding from the Defense Advanced 
# Research Projects Agency and the National Science Foundation of the 
# United States of America, and the CMU Sphinx Speech Consortium.
#
# THIS SOFTWARE IS PROVIDED BY CARNEGIE MELLON UNIVERSITY ``AS IS'' AND 
# ANY EXPRESSED OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
# THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
# PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL CARNEGIE MELLON UNIVERSITY
# NOR ITS EMPLOYEES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY 
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
# ====================================================================

import os
from os import environ, path
import pyaudio
import wave
#from itertools import izip
from pocketsphinx import *
from sphinxbase import *
from espeak_cls import AudioFeedback

class SpeechRecognition(object):


    # Create a decoder with certain model
	def __init__(self):
		MODELDIR = "cmusphinx-code/pocketsphinx/model"
		config = Decoder.default_config()
		config.set_string('-hmm', path.join(MODELDIR, 'hmm/en_US/hub4wsj_sc_8k_adapt'))
		config.set_string('-lm', path.join(MODELDIR, 'lm/en_US/num_adapt.dmp'))
		config.set_string('-dict', path.join(MODELDIR, 'lm/en_US/num_adapt.dic'))
		self.decoder = Decoder(config)
		self.speechCue = AudioFeedback()

	def Decode(self,wavfile):
		wavFile = file(wavfile, 'rb')
		wavFile.seek(44)
		self.decoder.decode_raw(wavFile)
		try:
			result = self.decoder.hyp().hypstr
			os.remove(wavfile)
			return result
		except AttributeError:
			print("No speech detected, please try again")
			os.remove(wavfile)	


	def speechRecognise(self, recog_sample):
		Chunk = 1024
		Format = pyaudio.paInt16
		Channels = 1
		Rate = 16000
		Duration = 4
		cue = 'Start'
		for x in range(1):
			fn = "o"+str(x)+".wav"
			p = pyaudio.PyAudio()
			self.speechCue.threadedFeedback(cue)
			stream = p.open(format=Format, channels=Channels, rate=Rate, input=True, frames_per_buffer=Chunk)
			print("* recording")
			frames = []

			for i in range(0, int(Rate / Chunk * Duration)):
		  		data = stream.read(Chunk)
		  		frames.append(data)
			print("* done recording")	

			stream.stop_stream()
			stream.close()
			p.terminate()

			wf = wave.open(fn, 'wb')
			wf.setnchannels(Channels)
			wf.setsampwidth(p.get_sample_size(Format))
			wf.setframerate(Rate)
			wf.writeframes(b''.join(frames))
			wf.close()
			wavfile = fn
			Recognised = self.Decode(wavfile)
			print Recognised
			#return Recognised
			print 'putting the input in queue'
                        recog_sample.put(Recognised)
		print 'end of speech recogniser'
	

if __name__ == '__main__':
	Input = SpeechRecognition()
	queue = []
	while True:
		result = Input.speechRecognise(queue)










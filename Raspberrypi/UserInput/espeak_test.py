import os

from espeak_cls import AudioFeedback

output = AudioFeedback()
string = "hello world"
output.Feedback(string)

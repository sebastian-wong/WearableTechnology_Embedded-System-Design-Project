import os
import sys
import math
import json
import urllib
import Queue as queue
import re

import signal
import threading
import time
import shelve

from multiprocessing import Process, Queue
from data_parser_dis import DataParser
from espeak_cls_edited import AudioFeedback
from Speechrecogniser_edited import SpeechRecognition

def signal_handler(signal, frame):
        #sys.modules[__name__].__dict__.clear()
        sys.exit(0)

class Map:
        AdjList = []
        distance = []
        prev = []
        path = []
        directions = []
        pq = queue.PriorityQueue()
        mapinfo = json.loads('{}')
        northAt = 0
        pos_x = 0
        pos_y = 0
	prev_step_compass_reading = None
	Activation = None

        def __init__(self, mapinfo):
                northAt = int(mapinfo['info']['northAt'])
                nodeConnectList = []
                nodeConnectVector = []
                for i in range(len(mapinfo['map'])):
                        nodeId = int( mapinfo['map'][i]['nodeId'] )
                        #nodeId = mapinfo['map'][i]['nodeId']
			linkTo = []
                        str = mapinfo['map'][i]['linkTo']
                        linkTo = [int(s) for s in str.split(",")]
                        nodeConnectVector.append(nodeId)
                        nodeConnectVector.append(linkTo)
                        nodeConnectList.append(nodeConnectVector)
                        nodeConnectVector = []
		nodeConnectList.sort()

                coordinatesNodes = []
                for i in range(len(mapinfo['map'])):
                        integerPair = [mapinfo['map'][i]['x'], mapinfo['map'][i]['y']]
                        coordinatesNodes.append(integerPair)

                Dist_AdjList = []
                Dist_List = []
                for i in range(len(nodeConnectList)):
                        for j in range(len(nodeConnectList[i][1])):
                                node = nodeConnectList[i][1][j]
                                node_a_x = int(coordinatesNodes[i][0])
                                node_a_y = int(coordinatesNodes[i][1])
                                node_b_x = int(coordinatesNodes[int(node)-1][0])
                                node_b_y = int(coordinatesNodes[int(node)-1][1])
                                dist = math.sqrt(int(int(node_a_x - node_b_x)**2 + int(node_a_y - node_b_y)**2))
                                Dist_List.append(dist)
                        Dist_AdjList.append(Dist_List)
                        Dist_List = []

                for i in range(len(nodeConnectList)):
                        AdjVector = [nodeConnectList[i][0]]
                        for j in range(len(nodeConnectList[i][1])):
                                integerPair = [nodeConnectList[i][1][j], Dist_AdjList[i][j]]
                                AdjVector.append(integerPair)
                        Map.AdjList.append(AdjVector)

                Map.mapinfo = mapinfo
                Map.northAt = northAt

        def searchNodeId(self, nodeName):
                searchByName = True
                searchNodeId = []
                sample = re.split(r'\s*', nodeName)
                if sample[0].lower() == 'node':
			print "searching by node number"
			print "nodeName", nodeName
			
                        searchByName = False
                        for i in range(len(sample)-1):
                                if sample[i+1].isdigit():
                                        searchNodeId.append(sample[i+1])
                                        print "appending", sample[i+1]
			print "searchNodeId is ", searchNodeId
                if searchByName:
                        for i in mapinfo['map']:
                                if i['nodeName'].lower() == nodeName.lower():
                                        print "found"
                                        return i['nodeId']
                else:
                        nodeId = int(''.join(map(str,searchNodeId)))
                        print "searchNodeId is ", nodeId
			for i in mapinfo['map']:
                                if int(i['nodeId']) == nodeId:
                                	print "found"
                                	return i['nodeId']
                raise Exception("invalid location!")

        def relax(self, u, v, w):
                u = int(u)
                v = int(v)
                if(Map.distance[v-1] > Map.distance[u-1] + w):
                        Map.distance[v-1] = Map.distance[u-1] + w
                        Map.prev[v-1] = u
                        integerpair = [Map.distance[v-1], v]
                        Map.pq.put(integerpair)

        def SSSP(self, start, end):
                for i in range(len(mapinfo['map'])):
                        Map.distance.append(sys.maxint)
                        Map.prev.append(None)

                Map.distance[start-1] = 0

                integerPair = [Map.distance[start-1], start]
                Map.pq.put(integerPair)

                while not Map.pq.empty():
                        front = Map.pq.get()
                        if(front[0] == Map.distance[front[1]-1]):
                                for j in range(len(Map.AdjList[front[1]-1])-1):
                                        self.relax(front[1], Map.AdjList[front[1]-1][j+1][0], Map.AdjList[front[1]-1][j+1][1])

                shortestTime = sys.maxint

                if Map.distance[end-1] < shortestTime:
                        shortestTime = Map.distance[end-1]

                backtrack = end-1
                path = []
                path.append(end)
                while Map.prev[backtrack] != None:
                        path.append(Map.prev[backtrack])
                        backtrack = Map.prev[backtrack] - 1

                return path
        
        def calcDisplacement(self, step, compass_read, i, pos_x, pos_y, nextCheckPoint, currentCheckPoint):
		print 'in calcDisplacement\n\n\n'
		reachCheckPoint = False
		detourCheckPoint = False
		direction = '%s %lf degrees, %lf'
		speak_direction = '%s %d degrees, %d point %d meters\n'
		
		per_step_dist = 60
                distance = per_step_dist * step
		print 'distance', distance
		
		#if step = 0, i = 5
		print 'value of i', i
		print 'compass_read in calcDisplacement', compass_read
		heading = compass_read[i*2] * (2**8) + compass_read[i*2+1]
                print 'heading', heading

		print 'before check prev_step_compass_reading'	
		#negate step taken if turning angle is too large to be reasonably taken while moving forward
##		if ( Map.prev_step_compass_reading is not None ) and ( abs(Map.prev_step_compass_reading - heading) > 60 ):
##			distance = 0
##		Map.prev_step_compass_reading = heading
		print 'after check prev_step_compass_reading'
		
		print 'is distance the problem'
		print 'distance is', distance	
                if distance < 0:
                        raise Exception("negative distance invalid")
                print "distance is not the problem"
		
		#compensating for magnetometer direction on bag
		heading = int(heading) - 90
		heading %= 360
		heading = abs(heading)

		#compensating for map northAt
		heading = int(heading) - (360 - Map.northAt)
		heading %= 360
		print '***DEBUGGING BEGINS***\n\n\n'
                print 'heading after compensation', heading
                #calculating displacement
                pos_x_delta = distance * math.sin(math.radians(heading))
                pos_y_delta = distance * math.cos(math.radians(heading))

                #calculating new position
                pos_x = float(pos_x) + pos_x_delta
                pos_y = float(pos_y) + pos_y_delta
                print "current coordinates (", pos_x, ", ", pos_y, " )"
                #getting coordinates of next checkpoint
                checkPoint_x = int(mapinfo['map'][nextCheckPoint - 1]['x'])
                checkPoint_y = int(mapinfo['map'][nextCheckPoint - 1]['y'])
                dist = math.sqrt(int(int(pos_x - checkPoint_x)**2 + int(pos_y - checkPoint_y)**2))
                checkpoint_direction = [checkPoint_x - pos_x, checkPoint_y - pos_y]
		
		print 'checkpoing_direction =', checkpoint_direction

                #getting coordinates of other adjacent checkpoints
                otherCheckPoint = []
                str = mapinfo['map'][currentCheckPoint - 1]['linkTo']
                print "linkTo", str
                otherCheckPoint = [int(s) for s in str.split(",")]
                otherCheckPoint.remove(nextCheckPoint)
                if len(otherCheckPoint) > 1:
                        print "number of adjacent checkpoints, ", len(otherCheckPoint)
                        otherCheckPoint_x = []
                        otherCheckPoint_y = []
                        #print "still ok"
                        for j in range(len(otherCheckPoint)):
                                otherCheckPoint_x.append(int(mapinfo['map'][otherCheckPoint[j] - 1]['x']))
                                otherCheckPoint_y.append(int(mapinfo['map'][otherCheckPoint[j] - 1]['y']))
                        print "check borders"
                        print otherCheckPoint_x
                        print otherCheckPoint_y
                        left_x = min(otherCheckPoint_x) if min(otherCheckPoint_x) < checkPoint_x else - sys.maxint - 1
                        right_x = max(otherCheckPoint_x) if max(otherCheckPoint_x) > checkPoint_x else sys.maxint
                        bottom_y = min(otherCheckPoint_y) if min(otherCheckPoint_y) < checkPoint_y else - sys.maxint - 1
                        top_y = max(otherCheckPoint_y) if max(otherCheckPoint_y) > checkPoint_y else sys.maxint

                        if (pos_x <= left_x or pos_x >= right_x) or (pos_y <= bottom_y or pos_y >= top_y):
                                detourCheckPoint = True
				speak_direction = 'detour'
                                return reachCheckPoint, pos_x, pos_y, detourCheckPoint, speak_direction

                try:
                        #tan_direction = checkpoint_direction[1] / checkpoint_direction[0]
			tan_direction = checkpoint_direction[0] / checkpoint_direction[1]
                except ZeroDivisionError:
                        if checkpoint_direction[1] >= 0:
                                tan_direction = sys.maxint
                        else:
                                tan_direction = - sys.maxint - 1

                heading_direction = math.degrees(math.atan(tan_direction))
                print 'heading_direction raw', heading_direction
		
		if checkpoint_direction[0] >= 0 and checkpoint_direction[1] >= 0:
                        heading_direction = heading_direction
                elif checkpoint_direction[0] < 0 and checkpoint_direction[1] >= 0:
                        heading_direction = 360 + heading_direction
                else:
                        heading_direction = 180 +  heading_direction
		
		print 'heading_direction bearing', heading_direction
                change_direction = heading_direction - heading
		print 'change_direction raw', change_direction
                if change_direction > 180:
                        change_direction = -1 * (360 - change_direction)
		elif change_direction < -180:
			change_direction =  360 + change_direction
		print 'change_direction direction', change_direction
		print 'in calcDisplacement, distance to next checkPoint is ', dist
                if dist >= 100:
                        if change_direction >= 25:
                                turn_instruction = 'clockwise'
                                direction = direction %(turn_instruction, change_direction, dist)
                                speak_direction = speak_direction %(turn_instruction, int(change_direction), int(dist/100), int((dist%100)/10))
                        elif change_direction <= -25:
                                turn_instruction = 'anticlockwise'
                                direction = direction %(turn_instruction, abs(change_direction), dist)
                                speak_direction = speak_direction %(turn_instruction, int(abs(change_direction)), int(dist/100), int((dist%100)/10))
                        else:
                                turn_instruction = 'go straight'
				direction = '%s, %d point %d meters'
                                speak_direction = direction %(turn_instruction, int(dist/100), int((dist%100)/10))
                                                
                else:
                        speak_direction = 'checkpoint reached!\n'
                        reachCheckPoint = True

                print 'end of calcDisplacement\n\n\n'        
                return reachCheckPoint, pos_x, pos_y, detourCheckPoint, speak_direction
        
     

        def provideDirections(self, nextCheckPoint, currentCheckPoint, pos_x, pos_y, speaker):
                print "start of provideDirections function"
                sayNextCheckPoint = 'your next checkpoint is %s\n' %(mapinfo['map'][nextCheckPoint - 1]['nodeName'])
		#speaker.threadedFeedback(sayNextCheckPoint)
                speaking_proc = Process(target=speaker.threadedFeedback, args=(sayNextCheckPoint,))
		speaking_proc.start()
		speaking_proc.join()
		detourCheckPoint = False
                reachCheckPoint = False
                start_time = time.time()
                time_to_speak = time.time()
		dist = None
		change_direction = None
		direction = '%s %lf degrees, %lf'
		speak_direction = '%s %d degrees, and walk %d point %d meters\n'
		isFirstTimeGivingDirections = False
		isFirstTimeProcDirections = True
		oneSecondHasPassed = False

                while True:
                        if (time.time() - start_time > 1) or isFirstTimeProcDirections:
                                start_time = time.time()
                                #distance, heading = input()
                                dataParser = DataParser()
                                step = dataParser.get_step_read()
                                print "NUMBER OF STEPS %d\n", step
                                compass_read = dataParser.get_compass_read()
				IR_read = dataParser.get_IR_read()
				#sonar_read = dataParser.get_ultrasound_read()
                                print "compass_read", compass_read
				warnUser = False
                                del dataParser
				
				head_IR_long = IR_read[0]
                                left_IR_short = IR_read[1]
                                right_IR_short = IR_read[2]
                                belt_IR_long = IR_read[3]
				
				print 'printing IR values'
				print 'head_IR_long', head_IR_long
				print 'left_IR_short', left_IR_short
				print 'right_IR_short', right_IR_short
				print 'belt_IR_long', belt_IR_long
	
				if head_IR_long < 100:
					print 'obstacle ahead'
					warningObstacle = 'obstacle ahead'
					#warnUser = True
				if left_IR_short < 50:
					print 'obstacle to your left'
					warningObstacle = 'warning'
					#warnUser = True
				if right_IR_short < 70:
					print 'obstacle to your right'
					warningObstacle = 'obstacle to your right'
					#warnUser = True
				if belt_IR_long > 150:
					print 'obstacle below'
					warningObstacle = 'obstacle below'
					#warnUser = True
				print 'clear the checking of IR values'	
				
				if warnUser:
					warnUser = False
					warn_time = time.time()
					if (time.time() - warn_time > 2):
						speaking_proc = Process(target=speaker.threadedFeedback, args=(warningObstacle,))
                                        	speaking_proc.start()
                                        	speaking_proc.join()
				else:
					print 'no warning'

				if not isFirstTimeProcDirections:
                                	if step == 0:
						print 'going into calcDisplacement\n\n\n'
                                	        reachCheckPoint, pos_x, pos_y, detourCheckPoint, speak_direction = self.calcDisplacement(0, compass_read, 5, pos_x, pos_y, nextCheckPoint, currentCheckPoint)
					else:
                                		for i in range(step):
							print 'step #', i
                                        		reachCheckPoint, pos_x, pos_y, detourCheckPoint, speak_direction = self.calcDisplacement(1, compass_read, i, pos_x, pos_y, nextCheckPoint, currentCheckPoint)
				if oneSecondHasPassed:
					isFirstTimeGivingDirections = True
					oneSecondHasPassed = False

				if isFirstTimeProcDirections:
					oneSecondHasPassed = True
					isFirstTimeProcDirections = False

                        if (time.time() - time_to_speak > 5) or isFirstTimeGivingDirections:
				isFirstTimeGivingDirections = False
                                time_to_speak = time.time()
                                speaking_proc = Process(target=speaker.threadedFeedback, args=(speak_direction,))
                                speaking_proc.start()

                        if reachCheckPoint:
                                speak_direction = 'checkpoint reached'
                                speaking_proc = Process(target=speaker.threadedFeedback, args=(speak_direction,))
                                speaking_proc.start()
                                break

                        if detourCheckPoint:
                                break
                                

                return reachCheckPoint, pos_x, pos_y, detourCheckPoint

        
        def provideNavigation(self, speaker):
                reachDestination = False
                calculatePath = True
                sayPathRoute = True
                
                while reachDestination == False:
                        if calculatePath == True:
                                path = myMap.SSSP(startNode, destNode)
                                calculatePath = False

                        if sayPathRoute:
                                routeSpeechInfo = ''
				for i in range(len(path)): #print in reverse order
                                        print path[len(path)-1-i], mapinfo['map'][path[len(path)-1-i]-1]['nodeName']
                                        if i == 0:
                                                firstNode = 'the path route starts from %s ' % mapinfo['map'][path[len(path)-1-i]-1]['nodeName']
                                                routeSpeechInfo = routeSpeechInfo + firstNode
                                        else:
                                                nextNode = 'followed by %s ' % mapinfo['map'][path[len(path)-1-i]-1]['nodeName']
                                                routeSpeechInfo = routeSpeechInfo + nextNode
                                routeSpeechInfo = routeSpeechInfo + '\n'
                                #speaker.threadedFeedback(routeSpeechInfo)
				speaking_proc = Process(target=speaker.threadedFeedback, args=(routeSpeechInfo,))
				speaking_proc.start()
				speaking_proc.join()
                                sayPathRoute = False

                        reachCheckPoint = True
                        currentCheckPoint = path.pop()
                        pos_x = mapinfo['map'][currentCheckPoint-1]['x']
                        pos_y = mapinfo['map'][currentCheckPoint-1]['y']
                        print "pos_x = ", pos_x, " pos_y = ", pos_y

                        while path:
                                if reachCheckPoint:
                                        reachCheckPoint = False
                                        nextCheckPoint = path.pop()
                                        if not path:
                                                reachDestination = True
                                        print nextCheckPoint, mapinfo['map'][nextCheckPoint-1]['nodeName']
                                try:
                                        reachCheckPoint, pos_x, pos_y, detourCheckPoint = self.provideDirections(nextCheckPoint, currentCheckPoint, pos_x, pos_y, speaker)
                                        if reachCheckPoint:
                                                currentCheckPoint = nextCheckPoint
                                        if detourCheckPoint:
                                                print 'detour!'
                                                sayDetour = 'detour! recalculating route'
                                                #speaker.threadedFeedback(sayDestinationReached)
                                                speaking_proc = Process(target=speaker.threadedFeeback, args=(sayDetour,))
						speaking_proc.start()
						speaking_proc.join()
						calculatePath = True
                                                path[:] = []
                                                sayPathRoute = True
                                                break
                                except Exception:
					print 'INVALID DISTANCE'

                        if ((not path) and (reachDestination == True)):
                                print "destination reached!"
                                sayDestinationReached = 'destination reached!\n'
                                #speaker.threadedFeedback(sayDestinationReached)
				speaking_proc = Process(target=speaker.threadedFeedback, args=(sayDestinationReached,))
				speaking_proc.start()
				speaking_proc.join()

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

    return int(result + current)


def stringParser(userInput):
        sample = re.split(r'\s*', userInput)
        userInput_proc = ''
        for j in range(len(sample)):
                if ((j > 0 or len(sample) == 1) and ( sample[j] == 'TO' or sample[j] == 'TOO')):
                        sample[j] = 'TWO'
                try:
                        number = text2int(sample[j].lower())
                        sample[j] = number
                except:
                        pass
        #print sample[j]
        for k in range(len(sample)):
                if sample[k] == 'COM':
                        userInput_proc = userInput_proc + sample[k]
                elif ( sample[k] == 'HYPHEN'):
                        userInput_proc = userInput_proc + '-'
                elif ( type(sample[k]) is type(str()) ) and ( len(sample[k]) == 1 ) and ( k < len(sample) - 1 ) \
                   and ( ( ( type(sample[k+1]) is type(str()) ) and ( len(sample[k+1]) == 1 ) ) or ( type(sample[k+1]) is int ) ):
                        userInput_proc = userInput_proc + sample[k]
                elif ( k < len(sample) - 1 ) and ( sample[k+1] == 'HYPHEN' ):
                        userInput_proc = userInput_proc + str(sample[k])
                else:
                        userInput_proc = userInput_proc + str(sample[k]) + ' '
        print userInput_proc
	return userInput_proc.rstrip()

def receiveUserInput(systemRequestType, speechInput, speaker, userInputInitialise):
	print 'in receiveUserInput function'
	proceedAhead = False

	def requestType(systemRequestType):
		systemQuery = None
		if systemRequestType == 'requestBuildingQuery':
			systemQuery = 'please enter building name'
			print 'entering name of building to systemQuery'
		elif systemRequestType == 'requestLevelQuery':
			systemQuery = 'please enter level of building'
		elif systemRequestType == 'requestStartQuery':
			systemQuery = 'please enter starting location'
		elif systemRequestType == 'requestDestQuery':
			systemQuery = 'please enter destination location'
		elif systemRequestType == 'requestRepeatQuery':
			systemQuery = 'please repeat input'
		elif systemRequestType == 'requestStartNavigation':
			systemQuery = 'would you like to start navigation'
		else:
			pass

		return systemQuery
	
	print 'systemRequestType is', systemRequestType	
	systemQuery = requestType(systemRequestType)
	print 'systemQuery is', systemQuery
	speaker_proc = Process(target=speaker.threadedFeedback, args=(systemQuery,))
	speaker_proc.start()
	speaker_proc.join()
	
	userInput = None
	if speechInput is not None:
		userInput = Queue()
		receive_input_proc = Process(target=speechInput.speechRecognise, args=(userInput,))
		receive_input_proc.start()
		receive_input_proc.join()
		userInput = userInput.get()
	else:
		voiceInput, numpadData = userInputInitialise.location_input()
		print 'numpadData is', numpadData
		try:
			userInput = int(''.join(map(str,numpadData)))
		except:
			print 'empty numpadData'
			return 'empty input'
		
		if ( systemQuery == 'please enter starting location' ) or ( systemQuery == 'please enter destination location' ):
			userInput = 'node %d' % userInput
	print 'userInput is', userInput
	userInput = str(userInput)
	return userInput


def confirmUserInput(userInputType, userInput, speechInput, speaker, userInputInitialise):
        print 'in confirmUserInput function'
	proceedAhead = False
        revertPrevProc = True

	def inputType(userInputType):
		inputQuery = None
		if userInputType == 'confirmBuildingQuery':
			inputQuery = 'please confirm that %s is the name of the building' % userInput
		elif userInputType == 'confirmLevelQuery':
			inputQuery = 'please confirm you are in level %s' %userInput
		elif userInputType == 'confirmStartQuery':
			inputQuery = 'please confirm that your starting location is %s' % userInput
		elif userInputType == 'confirmDestQuery':
			inputQuery = 'please confirm that your destination location is %s' % userInput
		elif userInputType == 'confirmStartNavigation':
			inputQuery = 'please acknowledge to begin navigation guidance'
		else:
			pass

		return inputQuery
        
	needToRepeatInput = False
        while revertPrevProc:
                print 'in revertPrevProc loop'
		revertPrevProc = False
                while not proceedAhead:
			if needToRepeatInput:
				print 'pls repeat input'
				#userInput = raw_input()
				userInput = receiveUserInput('requestRepeatQuery', speechInput, speaker, userInputInitialise)
				userInput = stringParser(userInput)
				if ( userInputType == 'confirmStartQuery' or userInputType == 'confirmDestQuery'):
					userInput = 'node %s' % userInput

                        inputQuery = inputType(userInputType)
                        speaking_proc = Process(target=speaker.threadedFeedback, args=(inputQuery,))
                        speaking_proc.start()
                        speaking_proc.join()

                        if speechInput is None:
                                voiceInput, numpadData = userInputInitialise.location_input()
                                if len(numpadData) == 0 or numpadData[0] == 2:
                                        revertPrevProc = True
					needToRepeatInput = True
                                elif numpadData[0] == 1:
					proceedAhead = True
				else:
                                        pass
                        else:
                                inputAnswer = Queue()
                                recog_proc = Process(target=speechInput.speechRecognise, args=(inputAnswer,))
                                recog_proc.start()
                                recog_proc.join()
				print 'processing inputAnswer'
				testInputAnswer = inputAnswer.get()
				print 'inputAnwer is ', testInputAnswer
                                if testInputAnswer == 'AFFIRMATIVE':
                                        proceedAhead = True
                                elif testInputAnswer == 'NEGATIVE':
                                        print 'acknowledged as needToRepeatInput'
					revertPrevProc = True
					needToRepeatInput = True
                                else:
                                        print 'inputAnswer is neither affirmative nor negative'
					pass
        #if userInput is None:
	#	userInput = True
	print 'userInput is', userInput
	return userInput
        


if __name__ == '__main__':
        signal.signal(signal.SIGINT, signal_handler)
        #building = raw_input()
        #level = raw_input()
        internetConnection = False
	speechInput = None
	speaker = AudioFeedback()
	userInputInitialise = DataParser()
        
	#initialising program announcement
	initProgAnnouncement = 'program initiated. press hex for key pad input or hold 5 for voice input'
	speaking_proc = Process(target=speaker.threadedFeedback, args=(initProgAnnouncement,))
	speaking_proc.start()
	speaking_proc.join()

	voiceInput, numpadData = userInputInitialise.location_input()

	if voiceInput:
                print 'using voice input'
                print 'initialising speech recognition'
                speechInput = SpeechRecognition()
        else:
                print 'using keypad input'
	
	print 'starting program'
	print 'please enter building name'
	#building = raw_input()
	building = receiveUserInput('requestBuildingQuery', speechInput, speaker, userInputInitialise)
	building = stringParser(building)
	building = confirmUserInput('confirmBuildingQuery', building, speechInput, speaker, userInputInitialise)
	
	print 'please enter level of building'
	#level = raw_input()
	level = receiveUserInput('requestLevelQuery', speechInput, speaker, userInputInitialise)
	level = stringParser(level)
	level = confirmUserInput('confirmLevelQuery', level, speechInput, speaker, userInputInitialise)
	

        s = shelve.open('map_cache.db')
        while not internetConnection:
                try:
                        url = 'http://showmyway.comp.nus.edu.sg/getMapInfo.php?Building=%s&Level=%s' %(building, level)
                        mapinfo = json.load(urllib.urlopen(url))
                        try:
                                s['map'] = mapinfo
                        finally:
                                s.close()
                        internetConnection = True
                except:
                        internetConnection = False
                        print "NO INTERNET SIGNAL!"
			downloadFailMsg = 'download failed, switching to cache'
			speaking_proc = Process(target=speaker.threadedFeedback, args=(downloadFailMsg,))
			speaking_proc.start()
			speaking_proc.join()
                        try:
                                mapinfo = s['map']
                                internetConnection = True
                        finally:
                                s.close()


        myMap = Map(mapinfo)

        print "northAt = ", myMap.northAt
	
	print 'please enter starting location'
        #startPlace = raw_input()
	startPlace = receiveUserInput('requestStartQuery', speechInput, speaker, userInputInitialise)
	startPlace = stringParser(startPlace)
	startPlace = confirmUserInput('confirmStartQuery', startPlace, speechInput, speaker, userInputInitialise)
	
	print 'please enter destination location'
	#destPlace = raw_input()
	destPlace = receiveUserInput('requestDestQuery', speechInput, speaker, userInputInitialise)
	destPlace = stringParser(destPlace)
	destPlace = confirmUserInput('confirmDestQuery', destPlace, speechInput, speaker, userInputInitialise)
	
	print 'startPlace is', startPlace
	print 'destPlace is', destPlace

        try:
                startNode = int(myMap.searchNodeId(startPlace))
		print 'startNode is', startNode
                destNode = int(myMap.searchNodeId(destPlace))
		print 'destNode is', destNode
        except Exception:
                print "INVALID LOCATION!"
        else:
		beginNaviGuide = receiveUserInput('requestStartNavigation', speechInput, speaker, userInputInitialise)
		beginNaviGuide = confirmUserInput('confirmStartNavigation', beginNaviGuide, speechInput, speaker, userInputInitialise)
		
		if beginNaviGuide is not None:		
			navi_proc = Process(target=myMap.provideNavigation, args=(speaker,))
			navi_proc.start()
		else:
			cancelNaviMsg = 'navigation guidance cancelled'
			speaking_proc = Process(target=speaker.threadedFeedback, args=(cancelNaviMsg,))
			speaking_proc.start()
			speaking_proc.join()

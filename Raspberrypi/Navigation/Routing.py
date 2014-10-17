import os
import sys
import math
import json
import urllib
import Queue as queue

import signal
import threading
import time
import shelve

from data_parser import DataParser
from espeak_cls import AudioFeedback
from Speechrecogniser import SpeechRecognition

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

        def __init__(self, mapinfo):
                northAt = int(mapinfo['info']['northAt'])
                nodeConnectList = []
                nodeConnectVector = []
                for i in range(len(mapinfo['map'])):
                        nodeId = mapinfo['map'][i]['nodeId']
                        linkTo = []
                        str = mapinfo['map'][i]['linkTo']
                        linkTo = [int(s) for s in str.split(",")]
                        nodeConnectVector.append(nodeId)
                        nodeConnectVector.append(linkTo)
                        nodeConnectList.append(nodeConnectVector)
                        nodeConnectVector = []

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
                for i in mapinfo['map']:
                        if i['nodeName'].lower() == nodeName.lower():
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

        def provideDirections(self, nextCheckPoint, currentCheckPoint, pos_x, pos_y, speaker):
                #threading.Timer(1.0, provideDirections(nextCheckPoint, currentCheckPoint, pos_x, pos_y)).start()
                print "start of function"
                sayNextCheckPoint = 'your next checkpoint is %s\n' %(mapinfo['map'][nextCheckPoint - 1]['nodeName'])
		speaker.threadedFeedback(sayNextCheckPoint)
                detourCheckPoint = False
                reachCheckPoint = False
                start_time = time.time()

                while True:
                        if (time.time() - start_time > 1):
                                start_time = time.time()
                                #distance, heading = input()
                                #dataParser = DataParser()
                                #distance = input()
                                #heading_arr = dataParser.get_compass_read()
                                #heading = heading_arr[0]
                                #del dataParser
                                dataParser = DataParser()
                                step = dataParser.get_step_read()
                                print "step", step
                                compass_read = dataParser.get_compass_read()
                                print "compass_read", compass_read
                                per_step_dist = 40
                                distance = step * per_step_dist
                                heading = compass_read[0]
                                del dataParser
                                print "distance", distance
                                print "heading", heading
                                if distance < 0:
                                        raise Exception("negative distance invalid")
                                print "distance is not the problem"
                                #compensating for map northAt
                                heading = int(heading) - (360 - Map.northAt)
                                heading %= 360

                                print "heading", heading
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
                                        print "still ok"
                                        for i in range(len(otherCheckPoint)):
                                                otherCheckPoint_x.append(int(mapinfo['map'][otherCheckPoint[i] - 1]['x']))
                                                otherCheckPoint_y.append(int(mapinfo['map'][otherCheckPoint[i] - 1]['y']))
                                        print "check borders"
                                        print otherCheckPoint_x
                                        print otherCheckPoint_y
                                        left_x = min(otherCheckPoint_x) if min(otherCheckPoint_x) < checkPoint_x else - sys.maxint - 1
                                        right_x = max(otherCheckPoint_x) if max(otherCheckPoint_x) > checkPoint_x else sys.maxint
                                        bottom_y = min(otherCheckPoint_y) if min(otherCheckPoint_y) < checkPoint_y else - sys.maxint - 1
                                        top_y = max(otherCheckPoint_y) if max(otherCheckPoint_y) > checkPoint_y else sys.maxint

                                        if (pos_x <= left_x or pos_x >= right_x) or (pos_y <= bottom_y or pos_y >= top_y):
                                                detourCheckPoint = True
                                                return reachCheckPoint, pos_x, pos_y, detourCheckPoint

                                try:
                                        tan_direction = checkpoint_direction[1] / checkpoint_direction[0]
                                except ZeroDivisionError:
                                        if checkpoint_direction[1] >= 0:
                                                tan_direction = sys.maxint
                                        else:
                                                tan_direction = - sys.maxint - 1

                                heading_direction = math.degrees(math.atan(tan_direction))
                                if checkpoint_direction[1] >= 0 and checkpoint_direction[0] >= 0:
                                        heading_direction = 90 - heading_direction
                                elif checkpoint_direction[1] < 0 and checkpoint_direction[0] >= 0:
                                        heading_direction = 90 - heading_direction
                                elif checkpoint_direction[1] < 0 and checkpoint_direction[0] < 0:
                                        heading_direction = 270 - heading_direction
                                else:
                                        heading_direction = 270 - heading_direction

                                direction = '%s %lf degrees, %lf'
                                speak_direction = '%s %d degrees, and walk %d point %d meters\n'
                                change_direction = heading_direction - heading
                                if change_direction > 180:
                                        change_direction = -1 * (change_direction - 180)
                                elif change_direction < -180:
                                        change_direction = -1 * (change_direction + 180)

                                if dist >= 20:
                                        if change_direction >= 10:
                                                turn_instruction = 'turn clockwise'
                                                direction = direction %(turn_instruction, change_direction, dist)
                                                speak_direction = speak_direction %(turn_instruction, int(change_direction), int(dist/100), int((dist%100)/10))
                                        elif change_direction <= -10:
                                                turn_instruction = 'turn anticlockwise'
                                                direction = direction %(turn_instruction, abs(change_direction), dist)
                                                speak_direction = speak_direction %(turn_instruction, int(abs(change_direction)), int(dist/100), int((dist%100)/10))
                                        else:
                                                turn_instruction = 'go straight'
                                                direction = direction %(turn_instruction, change_direction, dist)
                                                speak_direction = speak_direction %(turn_instruction, int(change_direction), int(dist/100), int((dist%100)/10))

                                        print direction
                                        speaker.threadedFeedback(speak_direction)
                                else:
                                        print "checkpoint reached!"
                                        sayReachCheckPoint = 'checkpoint reached!\n'
                                        speaker.threadedFeedback(sayReachCheckPoint)
                                        reachCheckPoint = True
                                        break

                return reachCheckPoint, pos_x, pos_y, detourCheckPoint
        def provideNavigation(self):
                reachDestination = False
                calculatePath = True
                sayPathRoute = True
                speaker = AudioFeedback()
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
                                speaker.threadedFeedback(routeSpeechInfo)
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
                                                speaker.threadedFeedback(sayDestinationReached)
                                                calculatePath = True
                                                path[:] = []
                                                sayPathRoute = True
                                                break
                                except Exception:
                                        print "INVALID DISTANCE!"

                        if ((not path) and (reachDestination == True)):
                                print "destination reached!"
                                sayDestinationReached = 'destination reached!\n'
                                speaker.threadedFeedback(sayDestinationReached)

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


def stringParser(userInput):
        sample = re.split(r'\s*', userInput)
        userInput_proc = ''
        for j in range(len(sample)):
                if (j > 0 and sample[j] == 'TO'):
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


if __name__ == '__main__':
        signal.signal(signal.SIGINT, signal_handler)
        #building = raw_input()
        #level = raw_input()
        internetConnection = False
	speechInput = SpeechRecognition()
	building = speechInput.speechRecognise()
	level = speechInput.speechRecognise()

        building = stringParser(building)
	level = text2int(level)

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
                        try:
                                mapinfo = s['map']
                                internetConnection = True
                        finally:
                                s.close()


        myMap = Map(mapinfo)

        print "northAt = ", myMap.northAt
        #startPlace = raw_input()
        #destPlace = raw_input()
	startPlace = speechInput.speechRecognise()
	destPlace = speechInput.speechRecognise()

	startPlace = stringParser(startPlace)
	destPlace = stringParser(destPlace)

        try:
                startNode = int(myMap.searchNodeId(startPlace))
                destNode = int(myMap.searchNodeId(destPlace))
        except Exception:
                print "INVALID LOCATION!"
        else:
                myMap.provideNavigation()

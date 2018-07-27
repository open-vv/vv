import sys
import os
import datetime
import numpy


def usage():
    print("Compute the enlapsed time for simulation with submitted, started and ended time")
    print("in ParentFolderName and its subfolder and starting by FileName.")
    print("The output is in statJobs.txt.")
    print("Usage:")
    print("python computeEnlapseTyme.py ParentFolderName FileName")
    print("eg: python computeEnlapseTyme.py spect_phs_build gate")
    return

def is_number(s):
    try:
        int(s)
        return True
    except ValueError:
        pass

def createDate(s):
    #Split the line to find the different element of the date
    words = s.split(' ')
    submittedDate = datetime.datetime(year=2000, month=1, day=1, hour=0, minute=0, second=0)
    for word in words:
        if is_number(word):
            #Find the year
            if int(word) > 2000:
                submittedDate = submittedDate.replace(year=int(word))
            #Find the day
            elif int(word) < 32:
                submittedDate = submittedDate.replace(day=int(word))
        #Find the month
        elif word == "Jan":
            submittedDate = submittedDate.replace(month=1)
        elif word == "Feb":
            submittedDate = submittedDate.replace(month=2)
        elif word == "Mar":
            submittedDate = submittedDate.replace(month=3)
        elif word == "Apr":
            submittedDate = submittedDate.replace(month=4)
        elif word == "May":
            submittedDate = submittedDate.replace(month=5)
        elif word == "Jun":
            submittedDate = submittedDate.replace(month=6)
        elif word == "Jul":
            submittedDate = submittedDate.replace(month=7)
        elif word == "Aug":
            submittedDate = submittedDate.replace(month=8)
        elif word == "Sep":
            submittedDate = submittedDate.replace(month=9)
        elif word == "Oct":
            submittedDate = submittedDate.replace(month=10)
        elif word == "Nov":
            submittedDate = submittedDate.replace(month=11)
        elif word == "Dec":
            submittedDate = submittedDate.replace(month=12)
        #Find the hour:minute:second
        elif word.count(':') == 2:
            times = word.split(':')
            submittedDate = submittedDate.replace(hour=int(times[0]))
            submittedDate = submittedDate.replace(minute=int(times[1]))
            submittedDate = submittedDate.replace(second=int(times[2]))
    return submittedDate

def computeEnlapsedTime():
    #Get the folder name ad the filename
    folderName = sys.argv[1]
    fileName = sys.argv[2]

    #Look for all files starting with the fileName
    filePaths = []
    for root, dirs, files in os.walk(folderName):
        for file in files:
            if file.startswith(fileName):
                filePaths.append(os.path.join(root, file))

    #For each file into filePaths, save the submitted, started end ended time
    submittedTime = []
    startedTime = []
    endedTime = []
    for file in filePaths:
        for line in open(file):
            if "Submitted" in line:
                submittedTime += [createDate(line)]
            if "Started" in line:
                startedTime += [createDate(line)]
            if "Ended" in line:
                endedTime += [createDate(line)]

    #Computation time
    computationTime = [a - b for a, b in zip(endedTime, startedTime)]
    computationTimeSecond = numpy.array([time.total_seconds() for time in computationTime])
    waitingTime = [a - b for a, b in zip(endedTime, submittedTime)]
    waitingTimeSecond = numpy.array([time.total_seconds() for time in waitingTime])
    computePart =computationTimeSecond / waitingTimeSecond * 100.0

    #Write the output
    with open('statJobs.txt', 'w') as output:
        output.write(str(len(filePaths)) + " run jobs\n\n")
        output.write("Starting submission at " + str(min(submittedTime)) + "\n")
        output.write("Finishing submission at " + str(max(submittedTime)) + "\n\n")
        output.write("First job started computation at " + str(min(startedTime)) + "\n")
        output.write("Last job started computation at " + str(max(startedTime)) + "\n\n")
        output.write("First job ended computation at " + str(min(endedTime)) + "\n")
        output.write("Last job ended computation at " + str(max(endedTime)) + "\n\n")
        output.write("Min computation time " + str(min(computationTime)) + "\n")
        output.write("Max computation time " + str(max(computationTime)) + "\n")
        output.write("Mean computation time " + str(datetime.timedelta(seconds=numpy.mean(computationTimeSecond))) + "\n")
        output.write("Std computation time " + str(datetime.timedelta(seconds=numpy.std(computationTimeSecond))) + "\n\n")
        output.write("Min waiting time " + str(min(waitingTime)) + "\n")
        output.write("Max waiting time " + str(max(waitingTime)) + "\n")
        output.write("Mean waiting time " + str(datetime.timedelta(seconds=numpy.mean(waitingTimeSecond))) + "\n")
        output.write("Std waiting " + str(datetime.timedelta(seconds=numpy.std(waitingTimeSecond))) + "\n\n")
        output.write("Min efficiency "  + str(min(computePart)) + " %\n")
        output.write("Max efficiency " + str(max(computePart)) + " %\n")
        output.write("Mean efficiency " + str(numpy.mean(computePart)) + " %\n")
        output.write("Std efficiency " + str(numpy.std(computePart)) + " %\n\n")
        output.write("Speed up " + str(numpy.mean(computationTimeSecond)*len(filePaths)/max(waitingTimeSecond)) + "\n\n")

    return

if __name__ == "__main__":
    if len(sys.argv) != 3:
        usage()
    else:
        computeEnlapsedTime()

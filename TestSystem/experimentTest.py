# File:         experimentTest.py
# Author:       Carter Hale, Owen Hichens
# Date Created: March 24, 2021
# Last Updated: March 30, 2021

# Element Tree for XML Parsing
import xml.etree.ElementTree as ET
# OS for System Calls to Run GA
import os

# Retrieve Settings from XML
tree = ET.parse('config.xml')
root = tree.getroot()

# Set Up XML Tree Root
rootGA = root.find("GeneticAlgo")
fitFun = rootGA.find("FitnessFunction")
idealMet = fitFun.find("IdealMetrics")

# Get user input for changing variable and step size
var = input("'Active', 'Percent', or 'Alive': ")
minSpace = float(input("Lower Bound of Search Space: "))
maxSpace = float(input("Upper Bound of Search Space: "))
bandSize = int(input("Number of Bands to Test: "))
otherWeights = float(input("Weights for Other Metrics: "))
stepSize = (maxSpace - minSpace) / bandSize

# First tests are for Active Metric
changingVar = idealMet.find(var)
changingMin = changingVar.find("Min")
changingMax = changingVar.find("Max")

# Set Other Metric Weights
weights = fitFun.find("Weights")
activeWeight = weights.find("ActiveWeight")
percentWeight = weights.find("PercentWeight")
aliveWeight = weights.find("AliveWeight")
if var == 'Active':
    percentWeight.text = str(otherWeights)
    aliveWeight.text = str(otherWeights)
    activeWeight.text = str(1.0)
elif var == 'Percent':
    aliveWeight.text = str(otherWeights)
    activeWeight.text = str(otherWeights)
    percentWeight.text = str(1.0)
elif var == 'Alive':
    percentWeight.text = str(otherWeights)
    activeWeight.text = str(otherWeights)
    aliveWeight.text = str(1.0)

# Set Active Metric
cMin = minSpace
cMax = minSpace + stepSize
changingMin.text = str(cMin)
changingMax.text = str(cMax)
tree.write('config.xml')

# Loop through Search Space
for i in range(bandSize):
    # Run GA and save output to logs
    os.system("unbuffer ./main.exe | tee " + str(i) + "test.log")
    # Append Info to Log File
    info = var + ": " + str(cMin) + " to " + str(cMax)
    os.system("echo '" + info.upper() + "' >> " + str(i) + "test.log")
    # Iterate over search space
    cMin = cMin + stepSize
    cMax = cMax + stepSize

    # Update Config File
    changingMin.text = str(cMin)
    changingMax.text = str(cMax)
    tree.write('config.xml')

    # Remove Past Simulation Files
    os.system("rm -r /home/CellAutomataGA/Desktop/Golly\ Patterns/Simulation")
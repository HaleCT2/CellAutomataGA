# File:         experiment.py
# Author:       Carter Hale, Owen Hichens
# Date Created: March 24, 2021
# Last Updated: March 29, 2021

# Element Tree for XML Parsing
import xml.etree.ElementTree as ET
# OS for System Calls to Run GA
import os

# THIS IS A SUPER BEGINNING, WE CAN INCORPORATE ALL THE METRICS AND WEIGHTS AND GET USER INPUT FOR
# WHAT WE WANT TO LOOK AT AND WHERE WE WANNA INTERATE THROUGH

# Retrieve Settings from XML
tree = ET.parse('config.xml')
root = tree.getroot()

# Set Up XML Tree Root
rootGA = root.find("GeneticAlgo")
fitFun = rootGA.find("FitnessFunction")
idealMet = fitFun.find("IdealMetrics")

# Get user input for changing variable and step size
var = input("'Active', 'Percent', or 'Alive': ")
minStep = float(input("Min step size: "))
maxStep = float(input("Max step size: "))

# First tests are for Active Metric
changingVar = idealMet.find(var)
changingMin = changingVar.find("Min")
changingMax = changingVar.find("Max")

# Set starting Metrics
if var == 'Active':
    cMin = 0.0
    cMax = 0.05
elif var == 'Percent':
    cMin = 0.0
    cMax = 0.4
elif var == 'Alive':
    cMin = 0.0
    cMax = 0.49
changingMin.text = str(cMin)
changingMax.text = str(cMax)
tree.write('config.xml')

# Loop through Search Space
for i in range(3):
    # Run GA and save output to logs
    os.system("unbuffer ./main.exe | tee ./test_logs/" + str(i) + "test.log")

    # Iterate over search space
    cMin = cMin + minStep
    cMax = cMax + maxStep

    # Update Config File
    changingMin.text = str(cMin)
    changingMax.text = str(cMax)
    tree.write('config.xml')

    # Remove Past Simulation Files
    os.system("rm -r /home/CellAutomataGA/Desktop/Golly\ Patterns/Simulation")
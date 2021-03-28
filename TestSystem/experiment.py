# File:         experiment.py
# Author:       Carter Hale
# Date Created: March 24, 2021
# Last Updated: March 24, 2021

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
# First tests are for Active Metric
active = idealMet.find("Active")
activeMin = active.find("Min")
activeMax = active.find("Max")
# Set starting Metrics
aMin = 0.0
aMax = 0.05
activeMin.text = str(aMin)
activeMax.text = str(aMax)
tree.write('config.xml')
# Loop through Search Space
for i in range(10):
    # Run GA and save output to logs
    os.system("./main.exe | tee " + str(i) + "test.log")
    # Iterate over search space
    aMin = aMin + .05
    aMax = aMax + .05
    # Update Config File
    activeMin.text = str(aMin)
    activeMax.text = str(aMax)
    tree.write('config.xml')
    # Remove Past Simulation Files
    os.system("rm -r /home/CellAutomataGA/Desktop/Golly\ Patterns/Simulation")

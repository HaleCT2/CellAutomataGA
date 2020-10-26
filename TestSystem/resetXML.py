# File:         resetXML.py
# Author:       Carter Hale
# Date Created: October 26, 2020
# Last Updated: October 26, 2020

# Element Tree for XML Parsing
import xml.etree.ElementTree as ET

# Retrieve Settings from XML
tree = ET.parse('config.xml')
root = tree.getroot()
# Set Up XML Tree Root
rootGA = root.find("GeneticAlgo")
newGen = rootGA.find("CurrentGeneration")
# Reset Current Generation to 0
newGen.text = str(0)
tree.write('config.xml')

# File:         golly-script.py
# Author:       Carter Hale
# Date Created: September 9, 2020
# Last Updated: October 21, 2020

# Golly doesn't offer standalone Library so the Module is instantiated
# when the Executable is launched. Script can only be ran from within Golly.
import golly as g
# OS Library used for File Management
import os
# Element Tree for XML Parsing
import xml.etree.ElementTree as ET


# -----------------------------------------------------------------------------
# Function to compare RLE Files for Homogeneity
def compare_rle(fileName1, fileName2):
    with open(fileName1, 'r') as file1:
        with open(fileName2, 'r') as file2:
            difference = set(file1).difference(file2)

    return (len(difference) == 1)
# -----------------------------------------------------------------------------


# -----------------------------------------------------------------------------
# Retrieve Settings from XML
tree = ET.parse('config.xml')
root = tree.getroot()
# Set Up XML Tree Roots
rootCA = root.find("CellAutomata")
rootGA = root.find("GeneticAlgo")
rootGrid = rootCA.find("StartingGrid")
# Determine Grid Properties
gridSize = rootGrid.find("GridSize").text
fillPerc = rootGrid.find("GridFillPerc").text
# Determine Number of CA Generations and the CA Population
timeElapsed = rootCA.find("TimeElapsed").text
numPopulation = rootGA.find("PopulationSize").text

# Check for Current GA Generation
currentGen = rootGA.find("CurrentGeneration").text
# -----------------------------------------------------------------------------


# -----------------------------------------------------------------------------
# Creates "Simulation" Folder within Directory if it does not Exist
fileLoc = g.getdir("app") + "Simulation\\"
if (os.path.isdir(fileLoc) is not True):
    os.mkdir(fileLoc)

# Creates "Generation_#" Folder within "Simulation"
generationDir = "Simulation\\" + "Generation_" + str(currentGen) + "\\"
fileLoc = g.getdir("app") + generationDir
if (os.path.isdir(fileLoc) is not True):
    os.mkdir(fileLoc)
# -----------------------------------------------------------------------------

# Open Current Generation's Rule Sets to Parse
rulesFileName = "rule_sets" + str(currentGen) + ".txt"
genRules = open(rulesFileName, 'r')

# Update Current Generation
newGen = rootGA.find("CurrentGeneration")
newGen.text = str(int(currentGen) + 1)
tree.write('config.xml')

for j in range(int(numPopulation)):
    # Create New Window and Fill X% of YxY Square Grid with Random Noise
    g.new("test-pattern")
    g.select([0, 0, int(gridSize), int(gridSize)])
    g.randfill(int(fillPerc))

    # Declare Algorithm and Rule
    g.setalgo("QuickLife")

    # Read In Rule Set from File
    rule = genRules.readline().strip()
    g.setrule(rule)

    # Set Directory Back to Parent Folder
    fileLoc = g.getdir("app") + generationDir
    # Creates Subfolder specific to Rule Set to hold Generation Patterns
    fileLoc += rule.replace("/", "_") + "\\"
    if (os.path.isdir(fileLoc) is not True):
        os.mkdir(fileLoc)

    # Prepare File Names for each Genereration's Pattern File
    fileNamePrefix = fileLoc + rule.replace("/", "_") + "_"

    # Loop and Save Patterns
    for i in range(int(timeElapsed) + 1):
        # Stop Loop if Universe is Empty
        if (g.empty()):
            break

        # Determine File Names
        fileNameRLE = fileNamePrefix + str(i) + ".rle"
        # Determine Previous File Names
        fileNamePrevRLE = fileNamePrefix + str(i-1) + ".rle"

        g.save(fileNameRLE, "rle")
        # Compare Previous Generation to Determine Class I Systems
        if (i > 0 and compare_rle(fileNameRLE, fileNamePrevRLE)):
            break

        g.run(1)

# Close
g.doevent("key q cmd")

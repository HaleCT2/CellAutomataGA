# File:         golly-script-test.py
# Author:       Carter Hale
# Date Created: September 9, 2020

# Golly doesn't offer standalone Library so the Module is instantiated
# when the Executable is launched. Script can only be ran from within Golly.
import golly as g
# Random Library used for Rule Generation
import random
# OS Library used for File Management
import os


# -----------------------------------------------------------------------------
# Function to create Random Rules
def make_rule():
    # Determine Number of Rules
    numBorn = random.randrange(0, 9)
    numSurvive = random.randrange(0, 9)

    # Randomly Generate Birth Conditions
    bornRule = ''
    for i in range(numBorn):
        num = random.randrange(1, 9)
        if(str(num) not in bornRule):
            bornRule += str(num)

    # Sort Numbers for Rule Notation
    bornRule = ''.join(sorted(bornRule))

    # Randomly Generate Survival Conditions
    surviveRule = ''
    for j in range(numSurvive):
        num = random.randrange(1, 9)
        if(str(num) not in surviveRule):
            surviveRule += str(num)

    surviveRule = ''.join(sorted(surviveRule))

    # Return Rule after concatenating Strings
    rule = "b" + bornRule + "/s" + surviveRule
    return rule
# -----------------------------------------------------------------------------


# Create New Window and fill X% of YxY Square Grid with Random Noise
g.new("test-pattern")
gridSize = g.getstring("Enter Side Length of Grid:", "100", "Grid Size")
fillPerc = g.getstring("Enter Grid Fill Percentage:", "50", "Fill Percentage")
g.select([0, 0, int(gridSize), int(gridSize)])
g.randfill(int(fillPerc))

# Declare Algorithm and Rule
g.setalgo("QuickLife")
customRule = g.getstring("If you wish to use a Specific Rule,\n" +
                         "enter it below with 'B0...8/S0...8' Notation.\n" +
                         "Otherwise, press 'OK' for Random Generation:",
                         "Random", "Specific Rule Set")

if (customRule == "Random"):
    rule = make_rule()
else:
    rule = customRule.lower()

g.setrule(rule)

# Creates "Test Patterns" Folder within Golly Directory if it does not Exist
fileLoc = g.getdir("app") + "Test Patterns\\"
if (os.path.isdir(fileLoc) is not True):
    os.mkdir(fileLoc)

# Creates Subfolder specific to Rule Set to hold Generation Patterns
fileLoc += rule.replace("/", "_") + "\\"
os.mkdir(fileLoc)
# Prepare File Names for each Genereration's Pattern File
fileNamePrefix = fileLoc + rule.replace("/", "_") + "_"

# Go through N Generations and save Pattern at each
numGenerations = g.getstring("Enter Number of Generations to Explore:",
                             "10", "Number of Generations")

for i in range(int(numGenerations) + 1):
    # Stop Loop if Universe is Empty
    if (g.empty()):
        break

    fileName = fileNamePrefix + str(i) + ".rle"
    g.save(fileName, "rle")
    g.run(1)

# Prepare for Viewing
g.fit()
g.setcursor("Select")

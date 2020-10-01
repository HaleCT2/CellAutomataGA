# File:         golly-script-test.py
# Author:       Carter Hale
# Date Created: September 9, 2020
# Last Updated: October 1, 2020

# Golly doesn't offer standalone Library so the Module is instantiated
# when the Executable is launched. Script can only be ran from within Golly.
import golly as g
# Random Library used for Rule Generation
import random
# OS Library used for File Management
import os
# Import WriteBMP from GLife
from glife.WriteBMP import WriteBMP
# Import FileCmp for BMP File Comparison
import filecmp


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


# -----------------------------------------------------------------------------
# Function to Create .BMP Images for each Pattern
def write_bmp(bmp_fileName):
    prect = g.getrect()
    x = prect[0]
    y = prect[1]
    wd = prect[2]
    ht = prect[3]

    colors = g.getcolors()
    state0 = (colors[1], colors[2], colors[3])

    # Create 2D Array of Pixels
    pixels = [[state0 for col in xrange(wd)] for row in xrange(ht)]

    cellcount = 0
    for row in xrange(ht):
        # Get a Row of Cells at a time
        cells = g.getcells([x, y + row, wd, 1])
        clen = len(cells)
        if clen > 0:
            inc = 2
        for i in xrange(0, clen, inc):
            pixels[row][cells[i]-x] = (colors[5], colors[6], colors[7])
            cellcount += 1

    WriteBMP(pixels, bmp_fileName)
# -----------------------------------------------------------------------------


# -----------------------------------------------------------------------------
# Function to compare RLE Files for Homogeneity
def compare_rle(fileName1, fileName2):
    with open(fileName1, 'r') as file1:
        with open(fileName2, 'r') as file2:
            difference = set(file1).difference(file2)

    return (len(difference) == 1)
# -----------------------------------------------------------------------------


# Ask User Configuration Questions
gridSize = g.getstring("Enter Side Length of Grid:", "100", "Grid Size")
fillPerc = g.getstring("Enter Grid Fill Percentage:", "50", "Fill Percentage")

customRule = g.getstring("If you wish to use a Specific Rule,\n" +
                         "enter it below with 'B0...8/S0...8' Notation.\n" +
                         "Otherwise, press 'OK' for Random Generation:",
                         "Random", "Specific Rule Set")

numPopulation = g.getstring("Enter Population Size of Individual CA Rules:\n" +
                            "(Note: Does not work with Specific Rule)",
                            "1", "Population Size")

numGenerations = g.getstring("Enter Number of CA Generations to Explore\n" +
                             "for Each Individual Rule in the Population:",
                             "10", "Number of Generations")

fileChoice = g.getstring("Enter Preferred Pattern File Format\n" +
                         "(Either 'RLE' or 'BMP'):", "RLE", "File Format")

# Loop Process for Each Individual Rule in the Population
for j in range(int(numPopulation)):
    # Create New Window and Fill X% of YxY Square Grid with Random Noise
    g.new("test-pattern")
    g.select([0, 0, int(gridSize), int(gridSize)])
    g.randfill(int(fillPerc))

    # Declare Algorithm and Rule
    g.setalgo("QuickLife")

    if (customRule == "Random"):
        rule = make_rule()
    else:
        rule = customRule.lower()

    g.setrule(rule)

    # Creates "Test Patterns" Folder within Directory if it does not Exist
    fileLoc = g.getdir("app") + "Test Patterns\\"
    if (os.path.isdir(fileLoc) is not True):
        os.mkdir(fileLoc)

    # Creates Subfolder specific to Rule Set to hold Generation Patterns
    fileLoc += rule.replace("/", "_") + "\\"
    if (os.path.isdir(fileLoc) is not True):
        os.mkdir(fileLoc)

    # Prepare File Names for each Genereration's Pattern File
    fileNamePrefix = fileLoc + rule.replace("/", "_") + "_"

    # Loop and Save Patterns
    for i in range(int(numGenerations) + 1):
        # Stop Loop if Universe is Empty
        if (g.empty()):
            break

        # Determine File Names
        fileNameRLE = fileNamePrefix + str(i) + ".rle"
        fileNameBMP = fileNamePrefix + str(i) + ".bmp"
        # Determine Previous File Names
        fileNamePrevRLE = fileNamePrefix + str(i-1) + ".rle"
        fileNamePrevBMP = fileNamePrefix + str(i-1) + ".bmp"

        if (fileChoice == "RLE"):
            g.save(fileNameRLE, "rle")
            # Compare Previous Generation to Determine Class I Systems
            if (i > 0 and compare_rle(fileNameRLE, fileNamePrevRLE)):
                break

        elif (fileChoice == "BMP" or fileChoice == "bmp"):
            write_bmp(fileNameBMP)
            # Compare Previous Generation to Determine Class I Systems
            if (i > 0 and filecmp.cmp(fileNameBMP, fileNamePrevBMP)):
                break

        g.run(1)

# Prepare for Viewing
g.fit()
g.setcursor("Select")

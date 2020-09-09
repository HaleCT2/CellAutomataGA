# Golly doesn't offer standalone Library so the Module is instantiated
# when the Executable is opened. Scripts can only be ran from within Golly.
import golly as g

# Create New Window and fill 30% of 100x100 Grid with Random Noise
g.new("test-pattern")
g.select([0, 0, 100, 100])
g.randfill(30)

# Declare Algorithm and Rule
g.setalgo("QuickLife")
rule = "b3/s23"
g.setrule(rule)


# Requires "Test Patterns" Folder within Golly Directory
fileLoc = g.getdir("app") + "Test Patterns\\"
fileNamePrefix = fileLoc + rule.replace("/", "_") + "_"

# Go through 10 Generations and save Pattern at each
for i in range(11):
    fileName = fileNamePrefix + str(i) + ".rle"
    g.save(fileName, "rle")
    g.run(1)

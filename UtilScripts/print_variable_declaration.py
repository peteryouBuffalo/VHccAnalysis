import sys

## == Necessary Variables =====================================================

## Data types we want to search
datatypes = [ "DATA_2016", "DATA_2017", "DATA_2018",
              "MC_2016",   "MC_2017",   "MC_2018"]

## Location of the proper files
listdir = "../VariableLists/"

## == Main Code ===============================================================

## Make sure that the user has included some input parameters.
if len(sys.argv) <= 1:
  print "Error: Please provide a variable to search for"
  print "       as a command line argument."
  exit()

varNames = sys.argv[1].split(',')
print "Variables provided: ", varNames

#loop over input variables
for var in varNames:
  
  print "\nChecking for", var
  ## Check through each datatype to see if the variable exists
  for datatype in datatypes:
    
    #open the file and read line by line to found matching variable name
    found = False
    filename = listdir + datatype + ".h"
    f = open(filename)

    ## Check to see if we have this variable in the file.
    ## If it's there, print out the declaration & information.
    for l in f.readlines():
      if var in l:
        print ">>> found declaration for", datatype
        #print f.name, " ", l
        print ">>>", l
        found = True
        continue  ## No need to loop through the rest if found
    
    
    ## If we did not find it, print out a message.
    if not found:
      print '>>> Variable ', var, ' not found in ', f.name




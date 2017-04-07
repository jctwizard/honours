from xml.etree import ElementTree
import fileinput
import os
import sys

#results = open('results.txt', 'r+')

for dirpath, dirnames, filenames in os.walk(os.getcwd()):
    for filepath in filenames:
        if filepath.endswith('.xml'):
            filename = os.path.join(dirpath, filepath)

            with open(filename) as f:
                s = f.read()

            for n in range(0,10):
                s = s.replace("<" + str(n), "<t" + str(n))
                s = s.replace("</" + str(n), "</t" + str(n))

            s = s.replace("<Create ", "<Create_")
            s = s.replace("</Create ", "</Create_")
            s = s.replace("<Grabbed Bad ", "<Grabbed_Bad_")
            s = s.replace("</Grabbed Bad ", "</Grabbed_Bad_")
            s = s.replace("<Score ", "<Score_")
            s = s.replace("</Score ", "</Score_")

            with open(filename, "w") as f:
                f.write(s)

            with open(filename, 'rt') as f:
                tree = ElementTree.parse(f)

            root = tree.getroot()

            successes = 0
            failures = 0
            bad_grabs = 0

            for element in root.iter():
                grab = element.find("GrabRight")
                bad = element.find("Grabbed_Bad_Bird")

                if bad is not None:
                    bad_grabs += 1

                if grab is None:
                    continue

                if int(grab.text) == 0:
                    failures += 1

                if int(grab.text) == 1:
                    successes += 1

            #results.write("Participant " + os.path.basename(os.path.normpath(dirpath)) + "\t")

            #if filepath.count("a") > 0:
                #results.write("Section A\n")
            #if filepath.count("b") > 0:
                #results.write("Section B\n")
            #if filepath.count("c") > 0:
                #results.write("Section C\n")

            sys.stdout.write(str(bad_grabs) + " & ")

            #results.write("Successes: " + str(successes) + "\t")
            #results.write("Failures: " + str(failures) + "\n\n")

#results.close()

from xml.etree import ElementTree
import fileinput
import os
import sys

#results = open('results.txt', 'r+')

successful_grab_list_a = []
successful_grab_list_b = []
successful_grab_list_c = []
failed_grab_list_a = []
failed_grab_list_b = []
failed_grab_list_c = []
bad_grab_list_a = []
bad_grab_list_b = []
bad_grab_list_c = []

for dirpath, dirnames, filenames in os.walk(os.getcwd()):
    for filepath in filenames:
        grid = False
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

                if len(element.getchildren()) == 49:
                    grid = True

                if grid is False:
                    if len(element.getchildren()) == 49:
                        grid = True

                    continue

                if bad is not None:
                    bad_grabs += 1

                if grab is None:
                    continue

                if int(grab.text) == 0:
                    failures += 1

                if int(grab.text) == 1:
                    successes += 1

            #results.write("Participant " + os.path.basename(os.path.normpath(dirpath)) + "\t")

            if filepath.count("a") > 0:
                successful_grab_list_a.append(successes)
                failed_grab_list_a.append(failures)
                bad_grab_list_a.append(bad_grabs)
                #results.write("Section A\n")
            if filepath.count("b") > 0:
                successful_grab_list_b.append(successes)
                failed_grab_list_b.append(failures)
                bad_grab_list_b.append(bad_grabs)
                #results.write("Section B\n")
            if filepath.count("c") > 0:
                successful_grab_list_c.append(successes)
                failed_grab_list_c.append(failures)
                bad_grab_list_c.append(bad_grabs)
                #results.write("Section C\n")

            #results.write("Successes: " + str(successes) + "\t")
            #results.write("Failures: " + str(failures) + "\n\n")

for grab in range(0, len(successful_grab_list_a)):
    sys.stdout.write(str(successful_grab_list_a[grab]) + " & ")
    sys.stdout.write(str(successful_grab_list_b[grab]) + " & ")
    sys.stdout.write(str(successful_grab_list_c[grab]) + " & ")

sys.stdout.write("\n\n")

for grab in range(0, len(failed_grab_list_a)):
    sys.stdout.write(str(failed_grab_list_a[grab]) + " & ")
    sys.stdout.write(str(failed_grab_list_b[grab]) + " & ")
    sys.stdout.write(str(failed_grab_list_c[grab]) + " & ")

sys.stdout.write("\n\n")

for grab in range(0, len(bad_grab_list_a)):
    sys.stdout.write(str(bad_grab_list_a[grab]) + " & ")
    sys.stdout.write(str(bad_grab_list_b[grab]) + " & ")
    sys.stdout.write(str(bad_grab_list_c[grab]) + " & ")


#results.close()

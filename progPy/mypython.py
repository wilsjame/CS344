# Program Py - CS 344
# James Wilson (wilsjame)
# Fri Nov 24, 2017

import random
import string

# Create 3 files in the same directory.

fList = []

f1 = open("elephantPy", 'w')
f2 = open("humanityPy", 'w')
f3 = open("taxiPy", 'w')

fList.append(f1)
fList.append(f2)
fList.append(f3)

# Fill files with 10 random characters from the lowercase alphabet (no spaces)
# followed by a newline character. Print the contents as well.

for file in fList:

	list = []

	for i in range(10):
		list.append(random.choice(string.ascii_lowercase))

	list.append('\n')

	for char in list:
		print(char, end = "")

	for i in list:
		file.write(i)

# Generate two random integers (1-42 range inclusive). 
# Print them and their product.

randNum1 = random.randint(1, 42)
randNum2 = random.randint(1, 42)
product = randNum1 * randNum2

print(str(randNum1) + "*" + str(randNum2) + "=" + str(product))


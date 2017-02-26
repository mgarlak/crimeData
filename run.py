## Michael Garlak
## 02-25-2017

import os

os.system('curl -o page.html http://louisville.edu/police/crime-log/february-2017-crime-log')
os.system('g++ crime2database.cpp -o crime2database -std=c++11 -l sqlite3')
os.system('./crime2database')
os.system('rm crime2database page.html')

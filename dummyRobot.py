import requests
from time import sleep

for i in range(100):
	x = requests.get('http://imi-dev.imi.kit.edu/web/trine/pollCommands.php').text
	print x
	sleep(1)

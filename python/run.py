import os
import threading

i = 0

def call_cpp():
	global i
	print "#", i
	i += 1

	os.system('../client/client ')

	threading.Timer(0.25, call_cpp).start()
	
call_cpp()

"""
This script can be used to run the tests. 
Use -f to provide an c source file
Use -r to provide the file that contains the expected output

"""

import sys, os, subprocess
import getopt
import shutil, shlex, re

class Options:
	"""Stores options for the tests"""
	srcfile = ""
	result = ""
	executable = "../build/c4"
	
	def __init__(self, s, r, e):
		srcfile = s
		result=r
		executable=e

def prepared():
	gEx = "../build/c4"

	invoke(Options("lexer/keywords_input.c","keywords_input.tok",gEx))

def main():
	# get cmd file
	try:
		opts, args = getopt.getopt(sys.argv[1:], "hf:r:e:p", ["help", "src", "result", "executable", "prepared"])
	except getopt.error, msg:
		print msg
		sys.exit(2)
	# handle options
	options = Options("", "", "")
	for o, a in opts:
		if o in ("-h", "--help"):
			print __doc__
			sys.exit(0)
		if o in ("-f", "--src"):
			options.srcfile = a
		if o in ("-r", "--result"):
			options.result = a
		if o in ("-e", "--executable"):
			options.executable = a
		if o in ("-p", "--prepared"):
			prepared()
			sys.exit(0)
	invoke(options)

def invoke(options):
	print "starting..."
	execCmd = [options.executable, "--tokenize", options.srcfile]
	result = subprocess.check_output(execCmd)
	f = open(options.result, 'r')
	fails = 0
	for line in f:
		if not result.startswith(line):
			fails = fails + 1
			print "Fail: expected: "+line+" but found: "+result[:result.find("\n")]
		result = result[result.find("\n"):]
	f.close()
	print "done"
	if 0!=fails:
		print "There were failures!"

main()

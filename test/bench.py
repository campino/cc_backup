"""
This script can be used to run the tests. 
Use -f to provide an c source file
Use -r to provide the file that contains the expected output
Use -p to execute the hard-coded tests

"""

import sys, os, subprocess
import getopt
import shutil, shlex, re

class Options:
	"""Stores options for the tests"""
	basedir = ""
	srcfile = ""
	result = ""
	executable = "../build/default/c4"
	
	def __init__(self, base, s, r, e):
		self.basedir = base
		self.srcfile = s
		self.result=r
		self.executable=e

def prepared():
	gEx = "../build/default/c4"

	invoke(Options("lexer/", "keywords_input.c","keywords_tokens.tok",gEx))
	invoke(Options("lexer/", "constants_input.c","constants_input.tok",gEx))
	invoke(Options("lexer/", "identifier_input.c","identifier_tokens.tok",gEx))
	invoke(Options("lexer/", "punctator_input.c","punctator_input.tok",gEx))
	invoke(Options("lexer/", "comments.c","comments.tok",gEx))
	invoke(Options("lexer/", "escape.c","escape.tok",gEx))
	

def main():
	# get cmd file
	try:
		opts, args = getopt.getopt(sys.argv[1:], "hf:r:e:p", ["help", "src", "result", "executable", "prepared"])
	except getopt.error, msg:
		print msg
		sys.exit(2)
	# handle options
	options = Options("", "", "", "")
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
	execCmd = [os.path.abspath(options.executable), "--tokenize", options.srcfile]
	try:
		result = subprocess.check_output(execCmd, cwd=options.basedir)
		f = open(os.path.join(options.basedir, options.result), 'r')
		fails = 0
		for line in f:
			if not result.startswith(line):
				fails = fails + 1
				print "Fail: expected: "+line+" but found: "+result[:result.find("\n")]
			result = result[result.find("\n")+1:]
		f.close()
		print "done"
		if 0!=fails:
			print "There were failures!"
	except subprocess.CalledProcessError, err:
		print "The lexer failed: " + str(err.returncode) + "."
		print "It printed: "+err.output

main()

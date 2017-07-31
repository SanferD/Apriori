import commands

minsup = [15, 20, 30, 50, 100, 500, 1000]
minconf = [.95, .9, .8]
hfrange = [20, 10, 5]
maxleafsize = [10, 50, 100]
inputfile = ["data/small", "data/inter"]

def run_program(_minsup, _minconf, _infile, _hfrange, _maxleafsize):
	_outfile = _infile+"_rules-"+str(_minsup)+"-"+str(_minconf)+"-"+str(_hfrange)+"-"+str(_maxleafsize)
	_outfile =  "data/debug/rules/"+_outfile.split("/").pop()
	lst = [_minsup, _minconf, _infile, _outfile, _hfrange, _maxleafsize]
	command = " ".join( map(lambda x: str(x), lst) )
	command = "./hcrminer " + command
	(ret, output) = commands.getstatusoutput(command)
	return output

def log_debug(output, _minsup, _minconf, _infile, _hfrange, _maxleafsize):
	_outfile = _infile+"_out-"+str(_minsup)+"-"+str(_minconf)+"-"+str(_hfrange)+"-"+str(_maxleafsize)
	_outfile =  "data/debug/stats/"+_outfile.split("/").pop()
	f = open(_outfile, 'w')
	f.write(output)

def get_results(output):
	lines = output.split("\n");
	print(lines)
	time_f = time_r = time_t = num_is = num_r = 0
	while (len(lines) > 0):
		line = lines.pop(0)
		if "time-frequent: " in line:
			time_f = float( line.split("time-frequent: ").pop() )
		if "time-rules: " in line:
			time_r = float( line.split("time-rules: ").pop() )
		if "time-total: " in line:
			time_t = float( line.split("time-total: ").pop() )
		if "# of frequent itemsets: " in line:
			num_is = float( line.split("# of frequent itemsets: ").pop() )
		if "# of rules: " in line:
			num_r = float( line.split("# of rules: ").pop() )
	return (time_f, time_r, time_t, num_is, num_r)

def main():
	f = open("log.txt", "w")
	for _minsup in minsup:
		for _minconf in minconf:
			for _hfrange in hfrange:
				for _infile in inputfile:
					for _maxleafsize in maxleafsize:
						output = run_program(_minsup, _minconf, _infile, _hfrange, _maxleafsize)
						log_debug(output, _minsup, _minconf, _infile, _hfrange, _maxleafsize)
						results = get_results(output)
						args = (_minsup, _minconf, _infile, _hfrange, _maxleafsize)
						t = (args, results)
						print(t)
						f.write(str(t)+'\n')
	f.close()

if __name__ == "__main__":
	main()
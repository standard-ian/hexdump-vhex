# hexdump-vhex
a hexdump clone, takes command line arguments 
		vhex [I:O:xdobAhv]
-I <ifile>: Name of (first) input file.
	If not specified, stdin is used.
-O <ofile>: Name of output file.
	If not specified, stdout is used.
-x        : Output is in hex (default).
-d        : Output is in decimal.
-o        : Output is in octal.
-b        : Output is in binary.
-A        : Don't show ASCII output.
-v        : Enable verbose level.
-h        : Show this amazing help.

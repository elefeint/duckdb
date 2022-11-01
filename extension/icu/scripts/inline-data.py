import sys

contents = bytearray(sys.stdin.buffer.read())

# encode the data into the stubdata file
result_text = ",".join(map(str, contents))

new_contents = """
// This file is generated by scripts/inline-data.py, do not edit it manually //

#include "unicode/utypes.h"
#include "unicode/udata.h"
#include "unicode/uversion.h"

extern "C" U_EXPORT const unsigned char U_ICUDATA_ENTRY_POINT [] = {
	%s
};
""" % (result_text,)

sys.stdout.write(new_contents)

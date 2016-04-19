#! /usr/bin/env python

import random
import string
import struct


FNAME_PREFIX    = 'afl-seed-msg'
MAX_UNAME_LEN   = 64
MAX_ADDR_LEN    = 64
OUTPUT_NUM      = 1


def get_one():
    return random.choice(string.letters+string.digits)


def get_range(upper):
    l = random.randrange(1, upper)
    return [get_one() for c in xrange(0, l)]    
    

for n in xrange(0, OUTPUT_NUM):
    addr = ''.join(get_range(MAX_ADDR_LEN))
    uname = ''.join(get_range(MAX_UNAME_LEN))

    with open(FNAME_PREFIX+"-"+str(n), 'w') as f:
        f.write(struct.pack('!BBBH{0}s{1}s'.format(len(addr), len(uname)),
            1, len(addr), len(uname),
            1+1+1+2+len(addr)+len(uname), addr, uname)
        )

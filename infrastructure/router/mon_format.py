#!/usr/bin/env python

import sys
from termcolor import colored

def format_counter(ratio):
    if ratio < 1000:
        return "  {:3.0f}".format(ratio)
    elif ratio < 1000*1000:
        return " {:3.0f}K".format(ratio/1000)
    else:
        return " {:3.0f}M".format(ratio/1000/1000);

def choose_color(ratio):
    if ratio < 1000*1000:
        return 'white'
    elif ratio < 5*1000*1000:
        return 'yellow'
    else:
        return 'red'

def get_ratio(counter):
    time, value = counter
    return float(value)/float(time)

def get_max_count(srcstat):
    def get_max_count(dststat):
        return max(map(get_ratio, dststat))
    return max(map(get_max_count, srcstat.itervalues()))

def get_attrs(bold):
    return ['bold'] if bold else []

def format_src(src, srcstat, bold):
    head = colored("{:4s}".format(src),
                   choose_color(get_max_count(srcstat)),
                   attrs = get_attrs(bold))
    statlines = []
    for indices in [[0,1], [2,3]]:
        statline = ""
        for dst in sorted(srcstat.keys()):
            dststat = srcstat[dst]
            for index in indices:
                ratio = get_ratio(dststat[index])
                statline += colored(format_counter(ratio),
                                    choose_color(ratio),
                                    attrs = get_attrs(bold))
        statlines.append(statline)
    return "{}{}\n    {}".format(head, statlines[0], statlines[1])

def format_head(dsts):
    rv = "         "
    for dst in dsts: rv += "{:7s}   ".format(dst)
    return rv

if __name__ == '__main__':
    counters = {}
    for line in sys.stdin:
        namedCounters = line.strip().split(',')
        for namedCounter in namedCounters:
            namepair, countspec = namedCounter.split(':', 1)
            src, dst = namepair.split('-')
            if not src in counters:
                counters[src] = {}
            counters[src][dst] = (map(lambda counter: (counter.split(':')),
                                      countspec.split('|')))

        print format_head(sorted(counters.keys()))

        bold = True
        for src in sorted(counters.keys()):
            print format_src(src, counters[src], bold)
            bold ^= True

        print
        print

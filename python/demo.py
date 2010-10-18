#!/usr/bin/python

from __future__ import with_statement
import pychq


data_x = [ 250.0,  350.0,  450.0,  550.0, 650.0,  750.0,  850.0,   950.0,  1050.0,  1150.0, 1250.0 ];
data_y = [ 10.1, 20.2, 10.1, 35.1, 40.2, 45.3, 30.35, 20.4, 10.35, 5.3,  1.0 ];

if __name__ == '__main__':
    pychq.plot(data_x, data_y)

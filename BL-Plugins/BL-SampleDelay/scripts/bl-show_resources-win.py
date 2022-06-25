#!/usr/bin/python

import os, datetime, fileinput, glob, sys, string, shutil, re

scriptpath = os.path.dirname(os.path.realpath(__file__))
projectpath = os.path.abspath(os.path.join(scriptpath, os.pardir))

#IPLUG2_ROOT = "../../.."
IPLUG2_ROOT = "../../../iPlug2"

fn_name_list = []
ext_list = []

def main():
  for line in fileinput.input(projectpath + "/config.h", inplace=0):
    if "_FN" in line:
      words=line.split()

      # fn names
      fn_name=words[1]
      fn_name_list.append(fn_name)
      
      # extensions
      file_name=words[2]
      ext_split=file_name.split(".")
      ext_split=ext_split[1].split("\"")
      ext = ext_split[0]
      ext = ext.upper()
      ext_list.append(ext);
  
  # dump 1
  print("    " + "\"#include \"\"..\\config.h\"\"\\r\\n\"") #, end='')
  for i in range(0, len(fn_name_list)):
    if i < len(fn_name_list) - 1:
      print("    " + "\"" + fn_name_list[i] + " " + ext_list[i] + " " + fn_name_list[i] + "\\r\\n\"")
    else:
      print("    " + "\"" + fn_name_list[i] + " " + ext_list[i] + " " + fn_name_list[i] + "\\0\"")
    
  print("")
  
  # dump 2
  print("#include \"..\config.h\"")
  for i in range(0, len(fn_name_list)):
      print(fn_name_list[i] + " " + ext_list[i] + " " + fn_name_list[i])
      
if __name__ == '__main__':
  main()

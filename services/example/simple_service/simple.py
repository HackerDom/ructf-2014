#!/usr/bin/python2
#Simple service by Bay, bay@hackerdom.ru

print("hello2")

params = raw_input().strip().split()
if params[0] == "put":
 flag_id, flag = params[1], params[2]
 open("/home/simple/flags/flags.txt","a").write("%s:%s\n" % (flag_id, flag))
if params[0] == "get":
 flag_id = params[1]
 for line in open("/home/simple/flags/flags.txt"):
  if line.startswith(flag_id + ":"):
   print(line.strip().split(":", 1)[1])
   break

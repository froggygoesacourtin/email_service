fp = open("orders_items_customers.csv", "r")

emailDict = {}
itemIdDict = {}

excludeDict = {'noemail@gmail.com' : 1,
 'nowmail@gmail.com' : 1, 
 'nowmai@gmail.com' : 1, 
 'saif.eliwat@gmail.com' : 1
}


firstFlag = True
errCnt = 0
for line in fp:
   elements = line.split(",")

   if firstFlag:
      firstFlag = False

      for i in range(0, len(elements)):
         element = elements[i].strip()
         if element == "item_id":
            itemIdOffset = i
         elif element == "customer_email":
            emailOffset = i

      print itemIdOffset, emailOffset
   else:
      try:
         itemId = elements[itemIdOffset].strip()
         email = elements[emailOffset].strip()
 
         if email.find("@") == -1:
            print line
            exit()

         if email not in excludeDict:
            if email in emailDict:
               dict = emailDict[email]
            else:
               dict = {}
               emailDict[email] = dict

            dict[itemId] = 1

            if itemId in itemIdDict:
               dict = itemIdDict[email]
            else:
               dict = {}
               itemIdDict[email] = dict

            dict[email] = 1
      except Exception as e:
         errCnt += 1

print "error count:", errCnt

hist = [0] * 100

for k, v in emailDict.iteritems():
   k = len(v)
   if k >= 100:
      print "high purchase customer:", k
      k = 99

   hist[k] += 1

for i in range(0, 100):
   print i, hist[i]

fp = open("result.dat", "w")
for k, v in emailDict.iteritems():
   fp.write(k + " " + str(len(v)) + "\n")

fp.close()

exit()

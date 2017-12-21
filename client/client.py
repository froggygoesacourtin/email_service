import mysql.connector

class Item(object):

   def __init__(self, itemId, title, price, description, URL, image):
      self.itemId = itemId
      self.title = title
      self.price = price
      self.description = description
      self.URL = URL
      self.image = image

class User(object):

   def __init__(self, email, first_name, last_name):
      self.email = email
      self.first_name = first_name
      self.last_name = last_name

itemDict = {}
userDict = {}
userBlackListDict = {}

popularItems = []
userItemDistMap = {}
itemItemClusterMap = {}

def eventWrite(email, itemId):
   cur.execute("insert into Events(email, itemId, created_at) values('%s', '%s', now())" % (email, itemId))

def itemFileRead(cur, filename):
   flag = True
   fd = open(filename, "r")

   for line in fd:
      if flag:
         flag = False
         elements = line.split(",")
         i = 0
         for element in elements:
            descriptor = element.strip().upper()
            if descriptor == "PRODUCTID":
               itemidIdx = i
            elif descriptor == "TITLE":
               titleIdx = i
            elif descriptor == "PRICE":
               priceIdx = i
            elif descriptor == "DESCRIPTION":
               descriptionIdx = i
            elif descriptor == "URL":
               urlIdx = i
            elif descriptor == "IMAGE":
               imageIdx = i

            i += 1
      else:
         elements = line.split(",")
         i = 0
         for element in elements:
            val = element.strip()
            if i == itemidIdx:
               itemId = val
            elif i == titleIdx:
               title = val
            elif i == priceIdx:
               price = float(val)
            elif i == descriptionIdx:
               description = val
            elif i == urlIdx:
               URL = val
            elif i == imageIdx:
               image = val

            i += 1

         item = Item(itemId, title, price, description, URL, image)
         itemDict[itemId] = item

         cur.execute("delete from Item where itemId = '%s'", (itemId))

   for itemId, item in itemDict.iteritems():
      cur.execute("insert into Item(itemId, title, price, description, URL, image)  values ('%s', '%s', %s, '%s', '%s', '%s')",
         (itemId, item.title, str(item.price), item.description, item.URL, item.image))

def UserFileRead(filename):
   flag = True
   fd = open(filename, "r")

   for line in fd:
      if flag:
         flag = False
         elements = line.split(",")
         i = 0
         for element in elements:
            descriptor = element.strip().upper()
            if descriptor == "EMAIL":
               emailIdx = i
            elif descriptor == "FIRST_NAME":
               first_nameIdx = i
            elif descriptor == "LAST_NAME":
               last_nameIdx = i

            i += 1
      else:
         elements = line.split(",")
         i = 0
         for element in elements:
            val = element.strip()
            if i == emailIdx:
               email = val
            elif i == first_nameIdx:
               first_name = val
            elif i == last_nameIdx:
               last_name = val

            i += 1

         user = User(email, first_name, last_name)
         userDict[email] = user

         cur.execute("delete from User where email= '%s'", (email))

   for email, user in userDict.iteritems():
      cur.execute("insert into User(email, first_name, last_name) values ('%s', '%s', '%s')",
         (email, user.first_name, user.last_name))

def userBlackListFileRead(filename):
   fd = open(filename, "r")
   for line in fd:
      elements = line.split(",")
      email = elements[0].strip()
      pl = elements[1].split(":")
      itemList = []
      for element in pl:
         itemList.append(element.strip())

      userBlackListDict[email] = itemList

def modelCreate(cur):
   popularItems = []
   userItemDistMap = {}
   itemItemClusterMap = {}
   itemTitles = {}
   itemDescriptions = {}
   itemPrices = {}
   itemImages = {}
   itemURLs = {}

   cur.execute("select item, cnt from PopularItems order by cnt desc")
   rows = cur.fetchall()
   for row in rows:
      itemId = row[0]
      popularItems.append(itemId)

   cur.execute("select email, weight, itemId from UserItemDist order by email, weight, itemId")
   rows = cur.fetchall()
   for row in rows:
      email = row[0]
      weight = float(row[1])
      itemId = row[2]

      if email not in userItemDistMap:
         lst = []
      else:
         lst = userItemDistMap[email]

      lst.append(itemId)

   cur.execute("select itemId1, weight, itemId2 from ItemItemCluster order by itemId1, weight, itemId2")
   rows = cur.fetchall()
   for row in rows:
      itemId1 = row[0]
      weight = float(row[1])
      itemId2 = row[2]

      if itemId1 not in itemItemClusterMap:
         lst = []
      else:
         lst = itemItemClusterMap[itemId1]

      lst.append(itemId2)

def getItemList(email, selItemDict, UserItemDict, UserItemBlackDict, maxResultListCount):
   resultList = []

   for itemId in UserItemDict:
      if itemId in selItemDict and itemId not in UserItemBlackList:
         resultList.append(itemId)
         if len(resultList) == maxResultListCount:
            return resultList

   if email in UserItemDistMap:
     itemList = UserItemDictMap[email]
   for itemId in itemList:
      if itemId in selItemDict and itemId not in UserItemBlackList:
         resultList.append(itemId)
         if len(resultList) == maxResultListCount:
            return resultList

   for itemId in popularItems:
      if itemId in selItemDict and itemId not in UserItemBlackList:
         resultList.append(itemId)
         if len(resultList) == maxResultListCount:
            return resultList


   return resultList

def createMail(template, email, selItemDict, UserItemDict, UserItemBlackDict, maxResultListCount):
   resultList = getItemList(email, selItemDict, UserItemDict, UserItemBlackDict, maxResultListCount)

   res = template.copy()
   for itemId in resultList:
      item = itemDict[itemId]
      for i in range(1, maxResultCount + 1):
         res = res.replace("%SmartMail_Title_Item" + str(i) + "%", item.title)
         res = res.replace("%SmartMail_Description_Item" + str(i) + "%", item.description)
         res = res.replace("%SmartMail_Price_Price" + str(i) + "%", item.price)
         res = res.replace("%SmartMail_URL_Item" + str(i + "%", item.URL)
         res = res.replace("%SmartMail_Image_Item" + str(i + "%", item.image)

   return res

def ProcessEmailList(template, emailList, selItemDict, UserItemDict, UserItemBlackDict):
   # derive the max number of items in template
   maxResultListCount = 0
   idx = 0
   while idx <> -1:
      idx = template.find("%SmartMail_Title_Item" + str(maxResultCount + 1) + "%")
      maxResultListCount += 1

   maxResultListCount -= 1

   for email in emailList:
      res = createMail(template, email, selItemDict, UserItemDict[email], UserItemBlackDict[email], maxResultListCount):

   ### mail out res

def ProcessEmailFiles(templateFile, emailListFile, selItemFile, UserItemFile, UserItemBlackFile):
   fd = open(templateFile, "r")
   template = ""
   for line in fd:
      template += line

   fd.close()

   fd = open(emailListFile, "r")
   emailList = []
   for line in fd:
      email = line.strip()
      emailList.append(email)

   fd.close()

   fd = open(selItemFile, "r")
   selItemDict = {}
   for line in fd:
      itemId = line.strip()
      selItemDict[itemId] = 1

   fd.close()

   fd = open(UserItemFile, "r")
   UserItemDict = {}
   for line in fd:
      elements = line.strip().split()
      email = elements[0].strip()
      itemId = elements[1].strip()
      if email not in UserItemDict:
         d = {}
         UserItemDict[email] = d
      else:
         d = UserItemDict[email]

      d[itemId] = 1

   fd.close()

   fd = open(UserItemBlackFile, "r")
   UserItemBlackDict = {}
   for line in fd:
      elements = line.strip().split()
      email = elements[0].strip()
      itemId = elements[1].strip()
      if email not in UserItemBlackDict:
         d = {}
         UserItemDict[email] = d
      else:
         d = UserItemBlackDict[email]

      d[itemId] = 1

   fd.close()

   ProcessEmailList(template, emailList, selItemDict, UserItemDict, UserItemBlackDict)

conn = mysql.connector.connect(user="SmartMail", database="Client")
cur = conn.cursor()

exit()

import pycurl
import json
import os

data = json.dumps({"personalizations": [{"to": [{"email": "killerdogface@hotmail.com"}]}],"from": {"email": "admin@smartmailbuilder.com"},"subject": "Can you see this in your inbox","content": [{"type": "text/plain", "value": "This is a much longer message regarding particular aspects that seem worthy of a test"}]})

c = pycurl.Curl()
c.setopt(c.URL, "https://api.sendgrid.com/v3/mail/send")
c.setopt(c.HTTPHEADER, ["Authorization: Bearer %s" % (os.environ.get("SENDGRID_API_KEY")),
 "Content-Type: application/json"])
c.setopt(c.POST, 1)
c.setopt(c.POSTFIELDS, data)
c.setopt(c.VERBOSE, 0)
c.perform()

print c.getinfo(c.RESPONSE_CODE)

c.close()

exit()

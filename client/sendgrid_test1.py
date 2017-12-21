import sendgrid
import os
from sendgrid.helpers.mail import *

key=os.environ.get('SENDGRID_API_KEY')
print key
sg = sendgrid.SendGridAPIClient(apikey=key)
from_email = Email("admin@smartmailbuilder.com")
subject = "Hello World from the SendGrid Python Library!"
to_email = Email("thermalstrategy@hotmail.com")
content = Content("text/plain", "Hello, Email!")
mail = Mail(from_email, subject, to_email, content)
response = sg.client.mail.send.post(request_body=mail.get())
print(response.status_code)
print "response body"
print(response.body)
print "response headers"
print(response.headers)

exit()

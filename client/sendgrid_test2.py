import sendgrid
import os

sg = sendgrid.SendGridAPIClient(apikey=os.environ.get('SENDGRID_API_KEY'))
data = {
  "personalizations": [
    {
      "to": [
        {
          "email": "thermalstrategy@hotmail.com"
        }
      ],
      "subject": "Hello World from the SendGrid Python Library!"
    }
  ],
  "from": {
    "email": "admin@smartmailbuilder.com"
  },
  "content": [
    {
      "type": "text/plain",
      "value": "Hello, Email!"
    }
  ]
}
response = sg.client.mail.send.post(request_body=data)
print(response.status_code)
print(response.body)
print(response.headers)

exit()

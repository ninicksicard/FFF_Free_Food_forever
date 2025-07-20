import smtplib


class Notifications:
    def __init__(self, settings):
        # Initialize SMTP server
        self.server = smtplib.SMTP(settings["smtp_server"], settings["smtp_port"])
        self.server.starttls()
        self.server.login(settings["email"], settings["password"])

        # Initialize recipient list
        self.recipients = settings["recipients"]

        # Store settings
        self.settings = settings

    def send_notifications(self, message):
        # Send email notifications
        for recipient in self.recipients:
            self.server.sendmail(self.settings["email"], recipient, message)

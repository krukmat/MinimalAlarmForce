#! /usr/bin/python3.6
import sys
sys.path.insert(0, '/rest /var/www/html/AlarmForce/python/')
import logging
logging.basicConfig(stream=sys.stderr)
from rest_app import app as application
application.secret_key = '*!)s6ZvD'
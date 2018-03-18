from datetime import datetime, time

from mecloud.lib import crypto

print crypto.encrypt('hahahah')

print long(datetime.now().strftime('%s'))*1000

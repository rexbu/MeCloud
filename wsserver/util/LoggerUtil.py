import logging.config

from mecloud.lib import log

logging.config.fileConfig('./logger.conf')
logger = logging.getLogger('log1')

# to use log from mecloud.lib
# logger = log

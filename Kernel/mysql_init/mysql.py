import config
import friendly_api
import MySQLdb


conn = MySQLdb.connect(host=config.HOST, user=config.USER, passwd=config.PASSWD, db=config.DATABASE, port=config.PORT)


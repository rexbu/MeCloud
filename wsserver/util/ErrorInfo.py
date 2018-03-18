# coding=utf-8
from mecloud.model.MeError import MeException

ERR_PAY_NOT_FIND_COIN_SETTING = MeException(3001, 'coinSettingId wrong', 'failed')

ERR_PAY_CREATE_ORDERN0_ERROR = MeException(3002, 'create wx order no. error', 'failed')
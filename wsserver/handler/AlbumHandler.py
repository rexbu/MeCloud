# coding=utf8
import time
import traceback

import tornado.web
from mecloud.api.BaseHandler import BaseHandler, ERR_PARA, MeQuery, ObjectId, \
    ClassHelper, json

from util import DetectClass
from wsserver import logger


class AlbumHandler(BaseHandler):
    # @tornado.web.authenticated
    def get(self, action=None):
        try:
            if action == 'index':
                self.index()
            elif action == 'list':
                self.list()
            elif action == 'faces':
                self.facesv4()
            elif action == 'faceMedias':
                self.faceMediasv3()
            else:
                print 'action error ', action
        except Exception, e:
            logger.error(e)
            msg = traceback.format_exc()
            logger.error(msg)
            self.write(ERR_PARA.message)

    @tornado.web.authenticated
    def post(self, action=None):
        # update
        print action
        r = {}
        r['errCode'] = 0
        self.write(r)

    # 相册首页
    def index(self):
        userid = self.user['_id']
        album_config_cursor = ClassHelper('AlbumConfig').find({'show': True}, sort={'weight': -1})
        # userid = '5a0188ccca714319e603c9e8'
        cursor = ClassHelper('AlbumStatCount').find({'device': self.user['device']})
        count_dict = {}
        # count_data = ClassHelper('StatCount').find_one({'name': 'uploader_' + userid + '_faces'})
        count = 0
        face_group_cursor = ClassHelper('FaceGroup').find({'uploader': self.user['device']}, sort={'group': -1}, limit=1)
        if face_group_cursor:
            face_group_list = []
            for face_group in face_group_cursor:
                face_group_list.append(face_group)
            if face_group_list:
                count = face_group_list[0]['group'] + 1
        logger.debug('count:%d', count)

        # if not count_data:
        #     count = 0
        # else:
        #     count = count_data['count']
        if cursor:
            for data in cursor:
                count_dict[data['species']] = data['count']
        list = DetectClass.createListForUser(count_dict, count, list_config=album_config_cursor)
        logger.debug('list:%s', list)
        logger.debug('total_count:%d', count)
        r = {}
        r['errCode'] = 0
        r['list'] = list
        r['total_count'] = count
        s = json.dumps(r)
        self.write(s)

    # 相册首页
    # def indexV2(self):
    #     userid = self.user['_id']
    #     album_config_cursor = ClassHelper('AlbumConfig').find({'show': True}, sort={'weight': -1})
    #     # userid = '5a0188ccca714319e603c9e8'
    #     cursor = ClassHelper('AlbumStatCount').find({'device': self.user['device']})
    #     count_dict = {}
    #     count_data = ClassHelper('StatCount').find_one({'name': 'uploader_' + userid + '_faces'})
    #     if not count_data:
    #         count = 0
    #     else:
    #         count = count_data['count']
    #     if cursor:
    #         for data in cursor:
    #             count_dict[data['species']] = data['count']
    #     list = DetectClass.createListForUser(count_dict, count, list_config=album_config_cursor)
    #     logger.debug('list:%s', list)
    #     if list:
    #         for v in list:
    #             print 'v:%s', v
    #             if v['type'] == 0 and v['count'] > 0:
    #                 list_final = []
    #                 query = {'name': {'$regex': '^' + userid + '_[0-9]*_faces$'}}
    #                 logger.debug('query:%s', query)
    #                 count_cursor = ClassHelper('StatCount').find(query, limit=20, sort={'count': -1})
    #                 group_info_list = []
    #                 group_index_list = []
    #                 if count_cursor:
    #                     for stat_count in count_cursor:
    #                         group_index = stat_count['name'].split('_')[1]
    #                         group_index_list.append(int(group_index))
    #                         data = {'count': stat_count['count'], 'face': group_index, 'group_index': int(group_index)}
    #                         group_info_list.append(data)
    #                 logger.debug('group_index_list:%s', group_index_list)
    #                 logger.debug('group_info_list:%s', group_info_list)
    #                 # logger.debug('cal group_index_list and group_info_list finish time:%f', (time.time() - start))
    #                 if group_info_list and group_index_list:
    #                     group_list = MeQuery('FaceGroup').aggregate(
    #                         [{"$match": {'group': {'$in': group_index_list}, 'uploader': userid}},
    #                          {"$group": {"_id": {"group": "$group"}, "group": {"$first": "$group"},
    #                                      "face": {"$first": "$face"},
    #                                      "groupid": {"$first": "$_id"}, "file": {"$first": "$file"}
    #                                      }}])
    #                     # logger.debug('group_list finish time:%f', (time.time() - start))
    #                     logger.debug('group_list:%s', group_list)
    #                     group_dict = {}
    #                     if group_list:
    #                         file_id_list = []
    #                         # file_dict = {}
    #                         face_id_list = []
    #                         for d in group_list:
    #                             file_id_list.append(d['file'])
    #                             face_id_list.append(d['face'])
    #                         face_dict = {}
    #                         face_cursor = ClassHelper('Face').find({'_sid': {'$in': face_id_list}}, {'rect': 1})
    #                         if face_cursor:
    #                             for face in face_cursor:
    #                                 face_dict[face['_id']] = face
    #                         logger.debug('face_dict:%s', face_dict)
    #                         media_dict = {}
    #                         media_cursor = ClassHelper('Media').find({'file': {'$in': file_id_list}},
    #                                                                  {'_sid': 1, 'uploader': 1, 'md5': 1, 'file': 1,
    #                                                                   'width': 1,
    #                                                                   'height': 1, 'faces': 1})
    #                         if media_cursor:
    #                             for media in media_cursor:
    #                                 logger.debug('media:%s', media)
    #                                 media_dict[media['file']] = media
    #                         logger.debug('media_dict:%s', media_dict)
    #                         for d in group_list:
    #                             media = media_dict.get(d['file'], None)
    #                             if media:
    #                                 if media['uploader'] == userid:
    #                                     if media.get('md5', None):
    #                                         rect = []
    #                                         for face in media['faces']:
    #                                             # logger.debug('face:%s', face)
    #                                             if face_dict.get(face, None):
    #                                                 rect = face_dict[face]['rect']
    #                                                 logger.debug('find face')
    #                                                 break
    #                                             else:
    #                                                 logger.debug('not find face')
    #                                                 pass
    #                                         group_dict[d['group']] = {'md5': media['md5'], 'width': media['width'],
    #                                                                   'height': media['height'], 'rect': rect}
    #
    #                                         logger.debug('group md5 data insert success!')
    #                                     else:
    #                                         logger.error('media not has md5, media id:%s', media['_id'])
    #                                 else:
    #                                     logger.error('uploader:%s is not current user:%s, mediaid:%s',
    #                                                  media['uploader'],
    #                                                  userid,
    #                                                  media['_id'])
    #                             else:
    #                                 logger.error('media not exists fileid:%s, facegroupid:%s', d['file'], d['groupid'])
    #                     logger.debug('group_dict:%s', group_dict)
    #                     # logger.debug('group_dict finish time:%f', (time.time() - start))
    #                     for data in group_info_list:
    #                         info = group_dict.get(data['group_index'], None)
    #                         if info:
    #                             data['md5'] = info['md5']
    #                             data['width'] = info['width']
    #                             data['height'] = info['height']
    #                             data['rect'] = info['rect']
    #                             list_final.append(data)
    #                         else:
    #                             logger.error('group_index:%d cannot find md5, userid:%s', data['group_index'], userid)
    #                 logger.debug('list_final:%s', list_final)
    #                 v['medias'] = list_final
    #                 logger.debug('v:%s', v)
    #             if v['type'] > 0 and v['count'] > 0:
    #                 name = DetectClass.getNameFromId(v['type'])
    #                 query = {'device': self.user['device'], 'species': name}
    #                 logger.debug('query:%s', query)
    #                 type_cursor = ClassHelper('Object').find(query, limit=20, sort={'time': -1})
    #                 list_r = []
    #                 if type_cursor:
    #                     for d in type_cursor:
    #                         logger.debug('data:%s', d)
    #                         if d.get('md5', None):
    #                             list_r.append({'md5': d['md5'], 'id': d['_id']})
    #                 v['medias'] = list_r
    #     logger.debug('total_count:%d', count)
    #     r = {}
    #     r['errCode'] = 0
    #     r['list'] = list
    #     r['total_count'] = count
    #     s = json.dumps(r)
    #     self.write(s)

    def list(self):
        userid = self.user['_id']
        # userid = '5a041db1ca7143760e482aad'
        logger.debug('album list userid:%s', userid)
        # userid = '5a0457b3ca71437840e6b86e'
        id = self.get_argument('id', None)
        size = int(self.get_argument('size', 20))
        type = int(self.get_argument('type', None))
        logger.debug('album list id:%s, size:%d, type:%d', id, size, type)
        if not type:
            self.write(ERR_PARA.message)
            return
        if size > 20:
            size = 20
        name = DetectClass.getNameFromId(type)
        if id:
            obj = ClassHelper('Object').find_one({'_id': id})
            logger.debug('obj:%s', obj)
            query = {'device': self.user['device'], 'species': name, "time": {"$lt": obj['time']}}
        else:
            query = {'device': self.user['device'], 'species': name}
        logger.debug('query:%s', query)
        cursor = ClassHelper('Object').find(query, limit=size, sort={'time': -1})
        r = {}
        list = []
        if cursor:
            for d in cursor:
                logger.debug('data:%s', d)
                if d.get('md5', None):
                    list.append({'md5': d['md5'], 'id': d['_id']})
        logger.debug('list:%s', list)
        r['list'] = list
        r['errCode'] = 0
        r['type'] = type
        self.write(r)

    def facesv4(self):
        # userid = self.user['_id']
        userid = self.user['device']
        logger.debug('userid:%s', userid)
        # userid = '5a01886eca714319e603c9e0'
        # userid = '5a0457b3ca71437840e6b86e'#linlin
        # userid = '5a1533dcca71431f973e61e0'  # ruita
        page = int(self.get_argument('page', 1))
        size = int(self.get_argument('size', 20))
        logger.debug('from client page:%d, size:%d', page, size)
        page = page - 1
        if size > 50:
            size = 50
        logger.debug('page:%s, size:%s', page, size)
        list_final = []
        start = time.time()
        query = {'name': {'$regex': '^' + userid + '_[0-9]*_faces$'}}
        logger.debug('query:%s', query)
        count_cursor = ClassHelper('StatCount').find(query, limit=size,
                                                     skip=(page * size),
                                                     sort={'count': -1})
        group_info_list = []
        group_index_list = []
        if count_cursor:
            for stat_count in count_cursor:
                group_index = stat_count['name'].split('_')[1]
                group_index_list.append(int(group_index))
                data = {'count': stat_count['count'], 'face': group_index, 'group_index': int(group_index)}
                group_info_list.append(data)
        logger.debug('group_index_list:%s', group_index_list)
        logger.debug('group_info_list:%s', group_info_list)
        logger.debug('cal group_index_list and group_info_list finish time:%f', (time.time() - start))
        if group_info_list and group_index_list:
            group_list = MeQuery('FaceGroup').aggregate(
                [{"$match": {'group': {'$in': group_index_list}, 'uploader': userid}},
                 {"$group": {"_id": {"group": "$group"}, "group": {"$first": "$group"}, "face": {"$first": "$face"},
                             "groupid": {"$first": "$_id"}, "file": {"$first": "$file"}
                             }}])
            logger.debug('group_list finish time:%f', (time.time() - start))
            logger.debug('group_list:%s', group_list)
            group_dict = {}
            if group_list:
                file_id_list = []
                # file_dict = {}
                face_id_list = []
                for d in group_list:
                    file_id_list.append(d['file'])
                    face_id_list.append(d['face'])
                face_dict = {}
                face_cursor = ClassHelper('Face').find({'_sid': {'$in': face_id_list}}, {'rect': 1})
                if face_cursor:
                    for face in face_cursor:
                        face_dict[face['_id']] = face
                logger.debug('face_dict:%s', face_dict)
                media_dict = {}
                media_cursor = ClassHelper('Media').find({'file': {'$in': file_id_list}},
                                                         {'_sid': 1, 'device': 1, 'md5': 1, 'file': 1, 'width': 1,
                                                          'height': 1, 'faces': 1})
                if media_cursor:
                    for media in media_cursor:
                        logger.debug('media:%s', media)
                        media_dict[media['file']] = media
                logger.debug('media_dict:%s', media_dict)
                for d in group_list:
                    media = media_dict.get(d['file'], None)
                    if media:
                        if media['device'] == userid:
                            if media.get('md5', None):
                                rect = []
                                f = face_dict.get(d['face'], None)
                                if f:
                                    rect = f['rect']
                                    logger.debug('find face')
                                else:
                                    logger.debug('find no face')
                                # for face in media['faces']:
                                #     # logger.debug('face:%s', face)
                                #     if face_dict.get(face, None):
                                #         rect = face_dict[face]['rect']
                                #         logger.debug('find face')
                                #         break
                                #     else:
                                #         logger.debug('not find face')
                                #         pass
                                group_dict[d['group']] = {'md5': media['md5'], 'width': media['width'],
                                                          'height': media['height'], 'rect': rect}

                                logger.debug('group md5 data insert success!')
                            else:
                                logger.error('media not has md5, media id:%s', media['_id'])
                        else:
                            logger.error('uploader:%s is not current user:%s, mediaid:%s', media['uploader'],
                                         userid,
                                         media['_id'])
                    else:
                        logger.error('media not exists fileid:%s, facegroupid:%s', d['file'], d['groupid'])
            logger.debug('group_dict:%s', group_dict)
            logger.debug('group_dict finish time:%f', (time.time() - start))
            for data in group_info_list:
                info = group_dict.get(data['group_index'], None)
                if info:
                    data['md5'] = info['md5']
                    data['width'] = info['width']
                    data['height'] = info['height']
                    data['rect'] = info['rect']
                    list_final.append(data)
                else:
                    logger.error('group_index:%d cannot find md5, userid:%s', data['group_index'], userid)

        # logger.debug('list_final.count:%d',list_final.__len__())
        r = {}
        r['list'] = list_final
        r['page'] = page + 1
        r['size'] = size
        r['errCode'] = 0
        logger.debug('r:%s', r)
        self.write(r)

    def faceMediasv3(self):
        # userid = self.user['_id']
        userid = self.user['device']
        # userid = '5a01886eca714319e603c9e0'
        # userid = '5a0188ccca714319e603c9e8'
        # userid = '5a1533dcca71431f973e61e0'  # ruita
        face = self.get_argument('face', None)
        id = self.get_argument('id', None)
        size = int(self.get_argument('size', 20))
        logger.debug('face: %s, id: %s, size: %s', face, id, size)
        if not face:
            self.write(ERR_PARA.message)
            return
        group_index = int(face)
        cursor = ClassHelper('FaceGroup').find({'uploader': userid, 'group': group_index})
        fileid_list = []
        faceid_list = []
        face_dict = {}
        if cursor:
            for d in cursor:
                # logger.debug('data:%s', d)
                fileid_list.append(d['file'])
                faceid_list.append(d['face'])
        logger.debug('fileid_list:%s', fileid_list)
        logger.debug('faceid_list:%s', faceid_list)
        face_cursor = ClassHelper('Face').find({'_sid': {'$in': faceid_list}}, {'rect': 1})
        if face_cursor:
            for face in face_cursor:
                face_dict[face['_id']] = face
        logger.debug('face_dict:%s', face_dict)

        query = {'device': userid, 'md5': {'$exists': True}, 'file': {'$in': fileid_list}}
        if id:
            query['_id'] = {"$lt": ObjectId(id)}
        # logger.debug('query:%s', json.dumps(query))
        cursor = ClassHelper('Media').find(query, limit=size)
        r = {}
        list = []
        if cursor:
            for d in cursor:
                # logger.debug('data:%s', d)
                if d.get('md5', None):
                    info = {'md5': d['md5'], 'id': d['_id'], 'width': d['width'], 'height': d['height']}
                    for face in d['faces']:
                        logger.debug('face:%s', face)
                        if face_dict.get(face, None):
                            info['rect'] = face_dict[face]['rect']
                            logger.debug('find face')
                            break
                        else:
                            logger.debug('not find face')
                            pass
                    list.append(info)

        logger.debug('list:%s', list)
        logger.debug('list.length:%s', list.__len__())
        r['list'] = list
        r['errCode'] = 0
        r['face'] = face
        self.write(r)

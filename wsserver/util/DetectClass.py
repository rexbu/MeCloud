# coding=utf-8
list = [{'id': 1, 'name': u'person'}, {'id': 2, 'name': u'bicycle'}, {'id': 3, 'name': u'car'},
        {'id': 4, 'name': u'motorcycle'}, {'id': 5, 'name': u'airplane'}, {'id': 6, 'name': u'bus'},
        {'id': 7, 'name': u'train'}, {'id': 8, 'name': u'truck'}, {'id': 9, 'name': u'boat'},
        {'id': 10, 'name': u'traffic light'}, {'id': 11, 'name': u'fire hydrant'}, {'id': 13, 'name': u'stop sign'},
        {'id': 14, 'name': u'parking meter'}, {'id': 15, 'name': u'bench'}, {'id': 16, 'name': u'bird'},
        {'id': 17, 'name': u'cat'}, {'id': 18, 'name': u'dog'}, {'id': 19, 'name': u'horse'},
        {'id': 20, 'name': u'sheep'}, {'id': 21, 'name': u'cow'}, {'id': 22, 'name': u'elephant'},
        {'id': 23, 'name': u'bear'}, {'id': 24, 'name': u'zebra'}, {'id': 25, 'name': u'giraffe'},
        {'id': 27, 'name': u'backpack'}, {'id': 28, 'name': u'umbrella'}, {'id': 31, 'name': u'handbag'},
        {'id': 32, 'name': u'tie'}, {'id': 33, 'name': u'suitcase'}, {'id': 34, 'name': u'frisbee'},
        {'id': 35, 'name': u'skis'}, {'id': 36, 'name': u'snowboard'}, {'id': 37, 'name': u'sports ball'},
        {'id': 38, 'name': u'kite'}, {'id': 39, 'name': u'baseball bat'}, {'id': 40, 'name': u'baseball glove'},
        {'id': 41, 'name': u'skateboard'}, {'id': 42, 'name': u'surfboard'}, {'id': 43, 'name': u'tennis racket'},
        {'id': 44, 'name': u'bottle'}, {'id': 46, 'name': u'wine glass'}, {'id': 47, 'name': u'cup'},
        {'id': 48, 'name': u'fork'}, {'id': 49, 'name': u'knife'}, {'id': 50, 'name': u'spoon'},
        {'id': 51, 'name': u'bowl'}, {'id': 52, 'name': u'banana'}, {'id': 53, 'name': u'apple'},
        {'id': 54, 'name': u'sandwich'}, {'id': 55, 'name': u'orange'}, {'id': 56, 'name': u'broccoli'},
        {'id': 57, 'name': u'carrot'}, {'id': 58, 'name': u'hot dog'}, {'id': 59, 'name': u'pizza'},
        {'id': 60, 'name': u'donut'}, {'id': 61, 'name': u'cake'}, {'id': 62, 'name': u'chair'},
        {'id': 63, 'name': u'couch'}, {'id': 64, 'name': u'potted plant'}, {'id': 65, 'name': u'bed'},
        {'id': 67, 'name': u'dining table'}, {'id': 70, 'name': u'toilet'}, {'id': 72, 'name': u'tv'},
        {'id': 73, 'name': u'laptop'}, {'id': 74, 'name': u'mouse'}, {'id': 75, 'name': u'remote'},
        {'id': 76, 'name': u'keyboard'}, {'id': 77, 'name': u'cell phone'}, {'id': 78, 'name': u'microwave'},
        {'id': 79, 'name': u'oven'}, {'id': 80, 'name': u'toaster'}, {'id': 81, 'name': u'sink'},
        {'id': 82, 'name': u'refrigerator'}, {'id': 84, 'name': u'book'}, {'id': 85, 'name': u'clock'},
        {'id': 86, 'name': u'vase'}, {'id': 87, 'name': u'scissors'}, {'id': 88, 'name': u'teddy bear'},
        {'id': 89, 'name': u'hair drier'}, {'id': 90, 'name': u'toothbrush'}]

list_use = [{'id': 18, 'name': u'dog', 'title': '可爱的狗狗'},
            {'id': 17, 'name': u'cat', 'title': '毛茸茸的猫咪们'},
            {'id': 31, 'name': u'handbag', 'title': '相册中的包袋'},
            {'id': 3, 'name': u'car', 'title': '相册中的小汽车'},
            {'id': 47, 'name': u'cup', 'title': '各种杯具'},
            {'id': 16, 'name': u'bird', 'title': '小鸟说早早早'},
            {'id': 88, 'name': u'teddy bear', 'title': '萌萌泰迪熊'},
            {'id': 51, 'name': u'bowl', 'title': '食为天 - 碗的照片'},
            {'id': 67, 'name': u'dining table', 'title': '食为天 - 我的餐桌'},
            {'id': 62, 'name': u'chair', 'title': '形形色色的座椅'},
            {'id': 63, 'name': u'couch', 'title': '舒适沙发'},
            {'id': 5, 'name': u'airplane', 'title': '飞机'},
            {'id': 38, 'name': u'kite', 'title': '啊呀是风筝呀'},
            {'id': 61, 'name': u'cake', 'title': '诱人蛋糕'},
            {'id': 58, 'name': u'hot dog', 'title': '热狗'},
            {'id': 55, 'name': u'orange', 'title': '橘子'},
            {'id': 52, 'name': u'banana', 'title': '香蕉'},
            {'id': 76, 'name': u'keyboard', 'title': '键盘'},
            {'id': 77, 'name': u'cell phone', 'title': '手机'}]


def getIdFromName(name):
    for data in list:
        if data['name'] == name:
            return data['id']


def getNameFromId(id):
    for data in list:
        if data['id'] == id:
            return data['name']


def createListForUser(count_dict,face_count, list_config=None):
    list = []
    person_data = {'type': 0, 'title': '出现次数最多的人脸', 'count': face_count}
    list.append(person_data)
    if not list_config:
        list_config = list_use
    for data in list_config:
        d = {'type': data['id'], 'title': data['title'], 'count': 0}
        if count_dict.get(data['name'], None):
            d['count'] = count_dict[data['name']]
        list.append(d)
    return list


if __name__ == '__main__':
    print getNameFromId(89)

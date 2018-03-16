#!/bin/bash
sudo apt-get install git build-essential python-dev

sudo apt-get install python-pycurl
git clone https://github.com/facebook/tornado.git tornado
cd tornado
#python setup.py build
sudo python setup.py install
cd ../

#install pymongo
git clone git://github.com/mongodb/mongo-python-driver.git pymongo
cd pymongo/
sudo python setup.py install
cd ../

#install asyncmongo
git clone  git://github.com/bitly/asyncmongo.git asyncmongo
cd asyncmongo
sudo python setup.py install
cd ..

#xmltodict
wget https://pypi.python.org/packages/source/x/xmltodict/xmltodict-0.10.1.tar.gz
tar -xvzf xmltodict-0.10.1.tar.gz
cd xmltodict-0.10.1
sudo python setup.py install

#jieba
pip install jieba
pip install isodate
pip install rsa

#install pil
apt-get install libjpeg8 libjpeg-dev libpng12-dev libpng++-dev libfreetype6-dev zlib1g-dev python-pip msttcorefonts
pip install pillow oss2 aliyun-python-sdk-sts
#ubuntu /usr/local/lib/python2.7/dist-packages/PIL/ImageFont.py第128行改为：
# self.font = core.getfont('/usr/share/fonts/truetype/msttcorefonts/'+font, size, index, encoding)
# mac /Library/Python/2.7/site-packages/PIL/ImageFont.py 第128行改为：
# self.font = core.getfont('/Library/Fonts/'+file, size, index, encoding)

#install zlib png freetype   jpeg
#wget http://sourceforge.net/projects/libpng/files/zlib/1.2.5/zlib-1.2.5.tar.gz/download?use_mirror=superb-dca2
#wget ftp://ftp.simplesystems.org/pub/libpng/png/src/libpng-1.5.6.tar.gz
#wget http://nchc.dl.sourceforge.net/project/freetype/freetype2/2.4.7/freetype-2.4.7.tar.gz
#wget http://www.ijg.org/files/jpegsrc.v8c.tar.gz

# 修改:/usr/share/fonts/truetype/msttcorefonts/
#wget http://effbot.org/media/downloads/Imaging-1.1.7.tar.gz
#tar -zxvf Imaging-1.1.7.tar.gz
#cd Imaging-1.1.7
#python setup.py build_ext -i
#sudo python setup.py install



#阿里云短信sdk依赖安装，在server下有一个目录aliyun-sms，里面有两个包，在两个包下面分别执行python setup install就可以了

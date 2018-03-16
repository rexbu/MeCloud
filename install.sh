git pull origin dev
rm -rf ./dist/*
python setup.py sdist
cd ./dist/
tar -xf mecloud-1.0.tar.gz
cd ./mecloud-1.0
python setup.py install

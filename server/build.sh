#!/bin/sh

cd api
./build.sh || { echo "Failed building api image. Exiting."; exit 1; }
cd -

cd nodecg
./build.sh || { echo "Failed building nodecg image. Exiting."; exit 1; }
cd -

cd nginx
./build.sh || { echo "Failed building nginx image. Exiting."; exit 1; }
cd -

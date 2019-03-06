#!/bin/sh
rm -rf public
hugo
cd public
aws s3 cp --recursive . s3://www.pwny.racing
cd ..

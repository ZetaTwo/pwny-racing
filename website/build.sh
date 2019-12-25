#!/bin/sh
rm -rf public
echo "Building website"
HUGO_ENV=production hugo
echo "Uploading files"
cd public
aws s3 cp --recursive . s3://www.pwny.racing
echo "Waiting 5 seconds before clearing cache."
sleep 5
aws cloudfront create-invalidation --distribution-id E7FEQCFSMPVTB --path '/*'
cd ..
echo "Site published"

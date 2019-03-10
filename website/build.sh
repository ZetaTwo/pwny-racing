#!/bin/sh
rm -rf public
echo "Building website"
hugo
echo "Uploading files"
cd public
aws --profile pwnyracing s3 cp --recursive . s3://www.pwny.racing
echo "Waiting 5 seconds before clearing cache."
sleep 5
aws --profile pwnyracing cloudfront create-invalidation --distribution-id E7FEQCFSMPVTB --path '/*'
cd ..
echo "Site published"

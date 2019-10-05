# Docker pwnable image

Docker setup for hosting pwnables

* Place the contents if this directory together with a file called "chall" and a file called "flag"
* Build the image with `docker build -t $CHALLENGE_NAME .`
* Run the image with `docker run -d -p ${PORT}:1337/tcp ${CHALLENGE_NAME}:latest`

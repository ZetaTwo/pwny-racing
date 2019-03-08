FROM keymetrics/pm2:latest-alpine

# Install npm dependencies
RUN apk add --no-cache git
RUN npm install -g bower

# Bundle APP files
RUN mkdir app
COPY nodecg app/nodecg/
COPY pwnybundle app/nodecg/bundles/pwnybundle/
COPY pm2.json app

# Install app dependencies
ENV NPM_CONFIG_LOGLEVEL warn
RUN cd app/nodecg && rm .git && git init
RUN cd app/nodecg && npm install --production && bower install --allow-root

EXPOSE 9090
WORKDIR app
CMD [ "pm2-runtime", "start", "pm2.json" ]

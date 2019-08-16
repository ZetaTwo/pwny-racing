FROM node:current as builder

WORKDIR /app
RUN git clone https://github.com/nodecg/nodecg.git .
RUN npm install --production
COPY pwnybundle bundles/pwnybundle/

FROM keymetrics/pm2:latest-alpine
WORKDIR /app

# Bundle APP files
COPY --from=builder /app .
COPY pm2.json .

ENV NPM_CONFIG_LOGLEVEL warn
EXPOSE 9090

CMD [ "pm2-runtime", "start", "pm2.json" ]

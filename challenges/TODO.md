* Auto checker docker


Add scripts:

Install:
```
apt install qemu-user qemu-user-binfmt
```

Run:
```
aws ecr get-login-password | sudo docker login --username AWS --password-stdin 928148438546.dkr.ecr.eu-west-1.amazonaws.com
for i in {1..22}; do sudo docker run --rm  -v '/etc/ssl/certs/ca-certificates.crt:/etc/ssl/certs/ca-certificates.crt' -d -p $((40000+$i)):1337/tcp 928148438546.dkr.ecr.eu-west-1.amazonaws.com/pwny-racing/challenges:chall${i}; done
```


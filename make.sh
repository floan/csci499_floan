#!/bin/bash

# Following code is to set up phase 1
# of the project, previously done manually

mkdir build
cd build
cmake .. 
make
cd .. #Returning to main directory

# Following code is to set up Go Part 
# of the project 
CURRDIR=$(pwd)
cd /usr/local
sudo wget -c https://golang.org/dl/go1.16.2.linux-amd64.tar.gz
sudo tar -C /usr/local -xvzf go1.16.2.linux-amd64.tar.gz
cd ~
echo "export GOROOT=/usr/local/go" >> .bashrc
echo "export GOPATH=$HOME/go" >> .bashrc
echo "export PATH=$GOPATH/bin:$GOROOT/bin:$PATH" >> .bashrc
source ~/.profile
cd $CURRDIR

export GO111MODULE=on
go get google.golang.org/protobuf/cmd/protoc-gen-go \
  google.golang.org/grpc/cmd/protoc-gen-go-grpc

cd protos
mkdir go_protos
cd go_protos
mkdir caw
mkdir faz
cd .. #Returning to proto directory

protoc --go_out=go_protos/caw --go_opt=paths=source_relative \
  --go-grpc_out=go_protos/caw --go-grpc_opt=paths=source_relative \
  caw.proto

protoc --go_out=go_protos/faz --go_opt=paths=source_relative \
  --go-grpc_out=go_protos/faz --go-grpc_opt=paths=source_relative \
  faz.proto

cd .. #Returning to main directory

go mod tidy
go build -o caw_go ./src/cli_tool/caw_cli.go
mv caw_go build
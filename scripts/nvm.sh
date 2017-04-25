#!/bin/sh

. ~/.nvm/nvm.sh

nvm install $1
nvm use $1
npm install

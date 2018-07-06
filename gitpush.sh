#!/bin/bash
git checkout master
git pull
git merge development
git push
git checkout development

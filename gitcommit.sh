#!/bin/bash
git add .
echo "comment: "
read cmt
git commit -m "$cmt"
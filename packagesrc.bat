@echo off
del /q bs2mgr-src.zip
zip -r bs2mgr-src.zip source\* -x *.git*
zip -r bs2mgr-src.zip LICENSE
zip -r bs2mgr-src.zip .gitignore
zip -r bs2mgr-src.zip README.MD
zip -r bs2mgr-src.zip packagesrc.bat


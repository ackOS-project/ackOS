@echo off

pushd %~dp0

docker build . -t ackos-build

popd

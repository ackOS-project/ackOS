@echo off

pushd %~dp0

docker attach (docker run --rm -dv %cd%:/mnt/host -w /mnt/host -ti ackos-build)

popd

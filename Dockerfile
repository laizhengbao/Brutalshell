
from alpine:3.23.0

run apk update
run apk add --no-cache build-base
run apk add --no-cache cmake
run apk add --no-cache git

workdir /tmp
copy . .

run cmake -B build . -DBUILD_TEST=ON -DCMAKE_BUILD_TYPE=Release
run cmake --build build --config Release
run ctest -C Release --test-dir build
run cmake --install build

workdir /workspace

cmd [ "/usr/local/bin/bsh" ]

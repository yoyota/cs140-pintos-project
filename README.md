# Pintos Projects

[spring 2019 lecture web site](http://web.stanford.edu/~ouster/cgi-bin/cs140-spring19/index.php)

## development environment

[pintos dev env kaist](https://github.com/hangpark/pintos-dev-env-kaist)

## Getting started

```sh
$ ./scripts/docker_run.sh
root@ac954084d33b:/pintos# cd utils/
root@ac954084d33b:/pintos/utils# make
```

## Test Example

```sh
make check TESTS='tests/userprog/args-none tests/userprog/args-single'
```

## Backtrace

```sh
backtrace build/kernel.o Call stack: 0xc0028e6f 0xc002101a
```
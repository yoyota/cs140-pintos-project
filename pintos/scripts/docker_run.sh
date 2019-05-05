REPOSITORY_PATH="$(cd $(dirname $(dirname $0)) && pwd -P)"
docker run --rm -it -v $REPOSITORY_PATH/src:/pintos --name pintos hangpark/pintos-dev-env-kaist bash

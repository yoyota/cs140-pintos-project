REPOSITORY_PATH="$(cd $(dirname $(dirname $0)) && pwd -P)"
docker run --rm -it -p 5000:5000 -v $REPOSITORY_PATH/src:/pintos --name pintos yoyota/pintos bash

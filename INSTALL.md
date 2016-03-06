Make sure you have Bash, Cmake, Git, FFmpeg and OpenCV installed.

If you're on Ubuntu, this should be enough:

```sh
    # this installs a prebuilt opencv with python bindings along with some other tools
    sudo apt-get install -y \
      git \
      pkg-config \
      build-essential \
      cmake \
      libopencv-dev \
      python-opencv \
      ffmpeg
```

Otherwise check:

* http://milq.github.io/install-opencv-ubuntu-debian/
* http://docs.opencv.org/2.4/doc/tutorials/introduction/linux_install/linux_install.html
* http://askubuntu.com/questions/432542/is-ffmpeg-missing-from-the-official-repositories-in-14-04
* https://help.ubuntu.com/community/OpenCV

After you have all the dependencies, the build is simple:

```sh
# assuming you installed from libopencv-dev in ubuntu/debian
# if not, look for the directory that contains OpenCVConfig.cmake
path_to_opencv=$(dpkg --listfiles libopencv-dev | grep --max-count=1 'OpenCVConfig.cmake$')

git clone https://github.com/MatrixAI/face-analysis-sdk
pushd face-analysis-sdk
    mkdir -p build
    pushd build
        cmake -DOpenCV_DIR="$path_to_opencv" -DFFMPEG=$(which ffmpeg) -DBASH=$(which bash) ..
        make
    popd
popd
```

root_dir=$(pwd .)
script_dir=$(dirname "$(readlink -f "$0")")
mkdir tmp_fldr
cd tmp_fldr
git clone --recurse-submodules \
          -b v1.66.0 \
          --depth 1 \
          --shallow-submodules \
          https://github.com/grpc/grpc

# Apply patch for third-party module re2 
# from https://github.com/google/re2/commit/b025c6a3ae05995660e3b882eb3277f4399ced1a
# to workaround compilation bug in Alpine containers
cd grpc/third_party/re2/util
git apply $script_dir/re2.patch
cd $root_dir/tmp_fldr

mkdir -p grpc/cmake/build
cd grpc/cmake/build
cmake -DgRPC_INSTALL=ON \
      -DgRPC_BUILD_TESTS=OFF \
      -DCMAKE_CXX_STANDARD=17 \
      -DCMAKE_INSTALL_PREFIX=/usr/local \
      ../..
make -j $(expr $(nproc --all) - 1)
make install -j $(expr $(nproc --all) - 1)
cd $root_dir
rm -rf tmp_fldr

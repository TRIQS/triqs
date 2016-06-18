class Triqs < Formula
  desc "Applications and Libraries for Physics Simulations"

  homepage "http://ipht.cea.fr/triqs/"
  url "https://github.com/TRIQS/triqs/archive/1.3.2.tar.gz"

  sha256 "09e44d78849fef6d4a12b4bdf08bc038d3a41ba2b5b5d43d8855668ac66165ff"
  head "https://github.com/TRIQS/triqs.git"

  option "with-test",   "Build and run shipped tests"

  depends_on "cmake" => :build
  depends_on :mpi => :cxx

  depends_on "boost"
  depends_on "hdf5"
  depends_on "cmake" => :build
  depends_on "fftw"
  depends_on "gmp"
  depends_on "python"

  depends_on :python => "mako"
  depends_on :python => "numpy"
  depends_on :python => "scipy"
  depends_on :python => "h5py"
  depends_on :python => "matplotlib"
  depends_on :python => "ipython"
  depends_on :python => "jupyter"
  depends_on :python => "mpi4py"

  def install
    ENV.cxx11
    args = std_cmake_args
    args.delete "-DCMAKE_BUILD_TYPE=None"
    args << "-DCMAKE_BUILD_TYPE=Release"

    args << ("-DBuild_Tests=" + ((build.with? "test") ? "ON" : "OFF"))

    args << ["-DPYTHON_INTERPRETER=/usr/local/bin/python", "-DALLOW_COMPILATION_IN_SOURCES=ON", "-DCMAKE_INSTALL_PREFIX=/usr/local"]

    mkdir "tmp" do
      args << ".."
      system "pip", "install", "--upgrade", "h5py"
      system "pip", "install", "--upgrade", "mako"
      system "pip", "install", "--upgrade", "mpi4py"
      system "cmake", *args
      system "make", "-j8"
      system "make", "test" if build.with? "test"
      system "make", "-j8", "install"
    end
  end

  test do
    system "python -c 'import pytriqs'"
  end
end

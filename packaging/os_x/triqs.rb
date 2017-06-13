class Triqs < Formula
  desc "Applications and Libraries for Physics Simulations"

  homepage "http://triqs.ipht.cnrs.fr/"
  url "https://github.com/TRIQS/triqs/archive/1.4.tar.gz"

  sha256 "98378d5fb934c02f710d96eb5a3ffa28cbee20bab73b574487f5db18c5457cc4"
  head "https://github.com/TRIQS/triqs.git"

  option "with-test",   "Build and run shipped tests"

  depends_on "cmake" => :build
  depends_on :mpi => :cxx

  depends_on "boost"
  depends_on "hdf5"
  depends_on "cmake" => :build
  depends_on "fftw"
  depends_on "gmp"
  depends_on "pkg-config" => :run
  depends_on :python if MacOS.version <= :snow_leopard

  depends_on "mako" => :python
  depends_on "numpy" => :python
  depends_on "scipy" => :python
  depends_on "h5py" => :python
  depends_on "mpi4py" => :python

  def install
    ENV.cxx11
    args = std_cmake_args
    args.delete "-DCMAKE_BUILD_TYPE=None"
    args << "-DCMAKE_BUILD_TYPE=Release"

    args << ("-DBuild_Tests=" + ((build.with? "test") ? "ON" : "OFF"))

    mkdir "tmp" do
      args << ".."
      system "cmake", *args
      system "make"
      system "make", "test" if build.with? "test"
      system "make", "install"
    end
  end

  def post_install
    chmod 0555, bin/"clang_parser.py"
    chmod 0555, bin/"cpp2doc_tools.py"
  end

  test do
    system "python -c 'import pytriqs'"
  end
end

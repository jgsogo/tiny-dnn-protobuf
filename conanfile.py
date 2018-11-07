
from conans import ConanFile, CMake


class TinyDNNCWrapper(ConanFile):
    name = "tiny_dnn_c_wrapper"
    version = "0.0"

    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False]}
    default_options = {"shared": False, "protobuf:shared": False, "protobuf:with_zlib": True}
    generators = "cmake"
    exports_sources = "src/*"

    def build_requirements(self):
        self.build_requires("protoc_installer/3.6.1@bincrafters/stable")

    def requirements(self):
        self.requires("protobuf/3.6.1@bincrafters/stable")
        self.requires("tiny-dnn/0.1.1@conan/stable")

    def source(self):
        print("*"*20)
        print(self.requires["protoc_install"].bindirs)

    def _generate_protos(self):
        ouptut_folder = self.build_folder

    def build(self):
        cmake = CMake(self)
        cmake.configure(source_folder=".")
        cmake.build()

    def package(self):
        self.copy("*.h", dst="include", src="src")
        self.copy("*.lib", dst="lib", keep_path=False)
        self.copy("*.dll", dst="bin", keep_path=False)
        self.copy("*.dylib*", dst="lib", keep_path=False)
        self.copy("*.so", dst="lib", keep_path=False)
        self.copy("*.a", dst="lib", keep_path=False)

    def package_info(self):
        self.cpp_info.libs = ["tiny_dnn_c_wrapper"]
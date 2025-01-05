from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout
from conan.tools.files import copy
import os

class SlogRecipe(ConanFile):
    name = "slog"
    version = "1.0.0"

    license = "MIT"
    author = "bkcarlos@outlook.com"
    url = "https://github.com/bkcarlos/slog"
    description = "Wrapper for spdlog"
    topics = ("spdlog", "log", "logger")

    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False], "fPIC": [True, False]}
    default_options = {"shared": False, "fPIC": True}

    # requires = "spdlog/1.11.0"

    exports_sources = "CMakeLists.txt", "include/*", "src/*", "spdlog/*"

    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

    def layout(self):
        cmake_layout(self)

    def generate(self):
        tc = CMakeToolchain(self)
        tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        # 使用 CMake 的安装目标
        cmake = CMake(self)
        cmake.install()

        # 如果需要额外复制头文件
        copy(self, "*.h", src=os.path.join(self.source_folder, "include"), dst=os.path.join(self.package_folder, "include"))
        copy(self, "*.hpp", src=os.path.join(self.source_folder, "include"), dst=os.path.join(self.package_folder, "include"))

    def package_info(self):
        self.cpp_info.libs = ["slog"]
        self.cpp_info.includedirs = ["include"]
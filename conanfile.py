from conan import ConanFile
from conan.tools.cmake import cmake_layout

class RTypeProjectConan(ConanFile):
    name = "rtype"
    version = "1.0.0"
    settings = "os", "compiler", "build_type", "arch"

    generators = ("CMakeDeps", "CMakeToolchain")

    def requirements(self):
        self.requires("sfml/2.6.2")
        self.requires("imgui-sfml/2.6.1")
        self.requires("imgui/1.91.8")
        self.requires("zlib/1.3.1")
        self.requires("asio/1.36.0")
        self.requires("catch2/3.10.0")
        self.requires("opengl/system")
        self.requires("glew/2.2.0")
        self.requires("openvr/1.16.8")
        self.requires("nlohmann_json/3.12.0")

        # Skip mesa-glu on Windows since it's already provided by the system
        if self.settings.os not in ["Windows"]:
            self.requires("mesa-glu/9.0.3")

    def layout(self):
        cmake_layout(self)

## CMake 构建步骤

### 快速开始

```bash
mkdir out && cd out
cmake .. -DCMAKE_BUILD_TYPE=Debug
```

*Release* 编译：

```bash
mkdir out_release && cd out_release
cmake .. -DCMAKE_BUILD_TYPE=Release
```

使用 *Makefile* ：

```bash
mkdir out && cd out
cmake .. -G "NMake Makefiles" -DSOUI_OUTPUT_DIR=bin
nmake
```

要编译`x86`或`x64`版本，分别使用相应版本的`Visual Studio 命令提示`

### 配置

`CMAKE_BUILD_TYPE`: 编译类型，可选参数`Debug`/`Release`/`RelWithDebInfo`/`MinSizeRel`，默认为`Debug`

`SOUI_OUTPUT_DIR`: 编译输出路径

`SOUI_BUILD_CORE_STATIC`: 编译SOUI核心为静态链接库，默认为`OFF`

`SOUI_BUILD_COMPONENTS_STATIC`: 编译SOUI组件为静态链接库，默认为`OFF`

`SOUI_BUILD_DEMOS`: 编译演示程序，默认为`ON`

`SOUI_WITH_UNICODE`: Unicode支持，默认为`ON`

`SOUI_WITH_STATIC_RUNTIME`: SOUI使用静态运行时，默认为`OFF`

`SOUI_WITH_XP_TOOLSET`: 支持XP，默认为`ON`

`SOUI_WITH_BUILTIN_WCHAR`: 编译`wchar_t`为C++的代码中内置整数类型的关键字，默认为`ON`

### 项目集成

若已有项目使用`CMake`构建，可直接将`SOUI`创建克隆至项目下使用，以下为使用示例：

```CMake
project(MyProject)

set(SOUI_WITH_STATIC_RUNTIME ON)
add_subdirectory(soui)

add_executable(mydemo ${CMAKE_CURRENT_SOURCE}/main.cc)
target_link_libraries(mydemo
  soui
  imgdecoder-wic
  render-gdi
)
```

```bash
mkdir out && cd out
cmake .. -G"NMake Makefiles" -DSOUI_BUILD_DEMOS=OFF
nmake
```

Have fun! :)

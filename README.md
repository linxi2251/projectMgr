# 项目管理器

基于Qt的雷达数据采集和处理项目管理器。

## 概述

ProjectMgr是一个C++库，提供了管理雷达数据采集项目的功能。它允许加载、保存和操作包含波束组、天线阵列和数据处理参数信息的项目文件。

## 功能特点

- 以XML格式加载和保存项目文件
- 管理波束组及其相关元数据
- 配置天线阵列及其属性
- 定义数据处理参数
- 设置数据处理过滤器
- 跟踪未保存的更改

## 项目结构

```
ProjectMgr/
├── ProjectMgr.cpp      - 项目管理器的实现
├── ProjectMgr.h        - 定义项目管理器接口的头文件
├── CMakeLists.txt      - 库的构建配置
test/
├── main.cpp            - 演示用法的测试应用程序
├── CMakeLists.txt      - 测试应用程序的构建配置
```

## 系统要求

- Qt 5（包含QtCore和QtXml模块）
- C++11兼容编译器
- CMake 3.0或更高版本

## 构建项目

```bash
mkdir build
cd build
cmake ..
make
```

## 使用示例

```cpp
#include "ProjectMgr.h"
#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    
    // 创建ProjectMgr实例
    ProjectMgr projectMgr;
    
    // 加载项目文件
    if (projectMgr.loadProject("path/to/project.iqproj")) {
        qDebug() << "项目加载成功";
        
        // 获取所有波束组
        QVector<SwathGroup> groups = projectMgr.getAllSwathGroups();
        
        // 访问波束组数据
        for (const SwathGroup& group : groups) {
            qDebug() << "组名称:" << group.name;
            // 处理组数据...
        }
        
        // 保存修改
        projectMgr.save();
    }
    
    return 0;
}
```

## 数据结构

- `SwathGroup`：包含雷达波束组信息
- `Array`：表示带有处理参数的天线阵列
- `DataProcessingParameters`：数据处理配置
- `FilterItems`：处理过程中应用的过滤器集合

## 许可证

[在此添加您的许可证信息]

## 联系方式

[在此添加您的联系信息] 
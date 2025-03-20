#include <QCoreApplication>
#include <QDebug>
#include <QFile>
#include <QDir>
#include "ProjectMgr.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    
    qDebug() << "测试 ProjectMgr 库";
    const QString rootPath = "D:/Users/buf/Desktop/projectMgr";
    // 直接使用现有的 RdcProject.uproj 文件
    QString projectFilePath = rootPath + "/RdcProject.iqproj";
    qDebug() << "将读取项目文件:" << projectFilePath;
    
    // 创建 ProjectMgr 实例
    ProjectMgr projectMgr;
    
    // 测试加载项目
    qDebug() << "加载项目...";
    if (projectMgr.loadProject(projectFilePath)) {
        qDebug() << "项目加载成功";
    } else {
        qDebug() << "项目加载失败";
        return 1;
    }
    
    // 获取所有 SwathGroup
    qDebug() << "获取所有 SwathGroup...";
    QVector<SwathGroup> groups = projectMgr.getAllSwathGroups();
    qDebug() << "找到" << groups.size() << "个 SwathGroup";
    
    // 显示所有 SwathGroup 的信息
    for (int i = 0; i < groups.size(); ++i) {
        const SwathGroup& group = groups[i];
        qDebug() << "\n--- SwathGroup " << (i+1) << " ---";
        qDebug() << "  名称:" << group.name;
        qDebug() << "  可见性:" << group.visible;
        qDebug() << "  文件夹:" << group.folder;
        qDebug() << "  传播速度:" << group.propagationVelocity;
        
        // 显示Array信息
        qDebug() << "  天线阵列数量:" << group.arrays.size();
        for (int j = 0; j < group.arrays.size(); ++j) {
            const Array& array = group.arrays[j];
            qDebug() << "    天线" << (j+1) << ":";
            qDebug() << "      名称:" << array.antennaName;
            qDebug() << "      ID:" << array.id;
            qDebug() << "      数据处理参数数量:" << array.processingParams.size();
            
            // 显示数据处理参数
            for (int k = 0; k < array.processingParams.size(); ++k) {
                const DataProcessingParameters& params = array.processingParams[k];
                qDebug() << "        参数" << (k+1) << ":";
                qDebug() << "          类型:" << params.cutType;
                qDebug() << "          名称:" << params.name;
                qDebug() << "          范围:" << params.rangeMin << "-" << params.rangeMax;
                qDebug() << "          模式:" << params.rangeMode;
                qDebug() << "          过滤器数量:" << params.filterItems.size();
                
                // 显示过滤器信息
                for (int l = 0; l < params.filterItems.size(); ++l) {
                    const QMap<QString, QString>& filter = params.filterItems[l];
                    qDebug() << "            过滤器" << (l+1) << ":";
                    qDebug() << "              启用:" << filter["enabled"];
                    qDebug() << "              名称:" << filter["name"];
                    for (auto it = filter.begin(); it != filter.end(); ++it) {
                        if (it.key() != "enabled" && it.key() != "name") {
                            qDebug() << "              " << it.key() << ":" << it.value();
                        }
                    }
                }
            }
        }
    }
    
    // 测试添加功能 - 添加一个新的SwathGroup
    qDebug() << "\n测试添加功能...";
    SwathGroup newGroup;
    newGroup.name = "Survey_2024-08-01_10-00-00";
    newGroup.visible = true;
    newGroup.folder = "Survey_2024-08-01_10-00-00";
    newGroup.propagationVelocity = 1500.0;
    
    // 添加一个天线阵列
    Array array;
    array.antennaName = "Antenna1";
    array.id = 1;
    
    // 添加数据处理参数
    DataProcessingParameters processingParams;
    processingParams.cutType = "Standard";
    processingParams.name = "Default";
    processingParams.rangeMin = 0.0;
    processingParams.rangeMax = 100.0;
    processingParams.rangeMode = 0;
    
    // 添加过滤器
    QMap<QString, QString> filter;
    filter["enabled"] = "1";
    filter["name"] = "StandardFilter";
    filter["threshold"] = "0.5";
    filter["window"] = "5";
    processingParams.filterItems.append(filter);
    
    array.processingParams.append(processingParams);
    newGroup.arrays.append(array);
    
    if (projectMgr.addSwathGroup(newGroup)) {
        qDebug() << "添加成功:" << newGroup.name;
    } else {
        qDebug() << "添加失败:" << newGroup.name;
    }
    
    // 测试查找功能 - 查找新添加的SwathGroup
    qDebug() << "\n测试查找功能...";
    SwathGroup* foundGroup = projectMgr.findSwathGroup("Survey_2024-08-01_10-00-00");
    if (foundGroup) {
        qDebug() << "找到SwathGroup:" << foundGroup->name;
        qDebug() << "  天线阵列数量:" << foundGroup->arrays.size();
        if (!foundGroup->arrays.isEmpty()) {
            const Array& firstArray = foundGroup->arrays[0];
            qDebug() << "  第一个天线名称:" << firstArray.antennaName;
            qDebug() << "  第一个天线ID:" << firstArray.id;
            if (!firstArray.processingParams.isEmpty()) {
                const DataProcessingParameters& firstParams = firstArray.processingParams[0];
                qDebug() << "  第一个处理参数名称:" << firstParams.name;
                qDebug() << "  第一个处理参数类型:" << firstParams.cutType;
            }
        }
    } else {
        qDebug() << "未找到SwathGroup";
    }
    
    // 测试修改功能 - 修改SwathGroup的参数
    qDebug() << "\n测试修改功能...";
    if (foundGroup) {
        foundGroup->visible = false;
        foundGroup->propagationVelocity = 1600.0;
        
        // 修改第一个天线的参数
        if (!foundGroup->arrays.isEmpty()) {
            Array& firstArray = foundGroup->arrays[0];
            firstArray.antennaName = "ModifiedAntenna";
            if (!firstArray.processingParams.isEmpty()) {
                DataProcessingParameters& firstParams = firstArray.processingParams[0];
                firstParams.name = "ModifiedParams";
                firstParams.rangeMax = 200.0;
            }
        }
        
        if (projectMgr.updateSwathGroup("Survey_2024-08-01_10-00-00", *foundGroup)) {
            qDebug() << "修改成功";
            qDebug() << "修改后visible值:" << foundGroup->visible;
            qDebug() << "修改后propagationVelocity值:" << foundGroup->propagationVelocity;

            if (!foundGroup->arrays.isEmpty()) {
                const Array& firstArray = foundGroup->arrays[0];
                qDebug() << "修改后第一个天线名称:" << firstArray.antennaName;
                if (!firstArray.processingParams.isEmpty()) {
                    const DataProcessingParameters& firstParams = firstArray.processingParams[0];
                    qDebug() << "修改后第一个处理参数名称:" << firstParams.name;
                    qDebug() << "修改后第一个处理参数范围:" << firstParams.rangeMax;
                }
            }
        } else {
            qDebug() << "修改失败";
        }
    }
    
    // 测试删除功能 - 删除第一个SwathGroup
    qDebug() << "\n测试删除功能...";
    if (!groups.isEmpty()) {
        QString firstGroupName = groups.first().name;
        qDebug() << "删除SwathGroup:" << firstGroupName;

        if (projectMgr.removeSwathGroup(firstGroupName)) {
            qDebug() << "删除成功";
        } else {
            qDebug() << "删除失败";
        }
    }
    
    // 保存到新文件
    QString newFilePath = QDir::currentPath() + "/RdcProject_Modified.uproj";
    qDebug() << "\n保存到新文件:" << newFilePath;
    
    if (projectMgr.saveProject(newFilePath)) {
        qDebug() << "保存成功";
    } else {
        qDebug() << "保存失败";
    }
    
    // 测试新的保存相关功能
    qDebug() << "\n测试新的保存相关功能...";
    
    // 测试获取当前文件路径
    qDebug() << "当前文件路径:" << projectMgr.getCurrentFilePath();
    
    // 测试是否有未保存的更改
    qDebug() << "是否有未保存的更改:" << projectMgr.hasUnsavedChanges();
    
    // 修改一个SwathGroup来测试未保存更改状态
    if (!groups.isEmpty()) {
        SwathGroup* group = projectMgr.findSwathGroup(groups.first().name);
        if (group) {
            group->propagationVelocity = 1700.0;
            qDebug() << "修改SwathGroup后是否有未保存的更改:" << projectMgr.hasUnsavedChanges();
        }
    }
    
    // 测试save()方法
    qDebug() << "\n测试save()方法...";
    if (projectMgr.save()) {
        qDebug() << "保存到当前文件成功";
        qDebug() << "保存后是否有未保存的更改:" << projectMgr.hasUnsavedChanges();
    } else {
        qDebug() << "保存到当前文件失败";
    }
    
    // 测试saveAs()方法
    QString saveAsFilePath = QDir::currentPath() + "/RdcProject_SaveAs.uproj";
    qDebug() << "\n测试saveAs()方法...";
    qDebug() << "另存为文件路径:" << saveAsFilePath;
    
    if (projectMgr.saveAs(saveAsFilePath)) {
        qDebug() << "另存为成功";
        qDebug() << "另存为后的当前文件路径:" << projectMgr.getCurrentFilePath();
        qDebug() << "另存为后是否有未保存的更改:" << projectMgr.hasUnsavedChanges();
    } else {
        qDebug() << "另存为失败";
    }
    
    // 显示修改后的所有SwathGroup
    qDebug() << "\n显示修改后的所有SwathGroup:";
    groups = projectMgr.getAllSwathGroups();
    qDebug() << "现在有" << groups.size() << "个SwathGroup";
    
    for (int i = 0; i < groups.size(); ++i) {
        const SwathGroup& group = groups[i];
        qDebug() << "\n--- SwathGroup " << (i+1) << " ---";
        qDebug() << "  名称:" << group.name;
        qDebug() << "  可见性:" << group.visible;
        qDebug() << "  文件夹:" << group.folder;
        qDebug() << "  传播速度:" << group.propagationVelocity;
        
        // 显示Array信息
        qDebug() << "  天线阵列数量:" << group.arrays.size();
        for (int j = 0; j < group.arrays.size(); ++j) {
            const Array& array1 = group.arrays[j];
            qDebug() << "    天线" << (j+1) << ":";
            qDebug() << "      名称:" << array1.antennaName;
            qDebug() << "      ID:" << array1.id;
            qDebug() << "      数据处理参数数量:" << array1.processingParams.size();

            // 显示数据处理参数
            for (int k = 0; k < array1.processingParams.size(); ++k) {
                const DataProcessingParameters& params = array1.processingParams[k];
                qDebug() << "        参数" << (k+1) << ":";
                qDebug() << "          类型:" << params.cutType;
                qDebug() << "          名称:" << params.name;
                qDebug() << "          范围:" << params.rangeMin << "-" << params.rangeMax;
                qDebug() << "          模式:" << params.rangeMode;
                qDebug() << "          过滤器数量:" << params.filterItems.size();
            }
        }
    }
    
    qDebug() << "\n测试完成";
    return 0;
}



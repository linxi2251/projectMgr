#include "ProjectMgr.h"
#include <QTextStream>
#include <QDir>

ProjectMgr::ProjectMgr() : isModified(false) {}

ProjectMgr::~ProjectMgr() {}

bool ProjectMgr::loadProject(const QString& filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }

    // Set the project root directory as the directory containing the project
    // file
    const QFileInfo fileInfo(filePath);

    QString errorMsg;
    int errorLine, errorColumn;
    if (!doc.setContent(&file, &errorMsg, &errorLine, &errorColumn)) {
        file.close();
        return false;
    }
    file.close();

    // Clear existing data
    swathGroups.clear();

    // Parse the document
    const QDomElement root = doc.documentElement();
    if (root.tagName() != "Project") {
        return false;
    }

    QDomNodeList swathGroupNodes = root.elementsByTagName("SwathGroup");
    for (int i = 0; i < swathGroupNodes.count(); ++i) {
        QDomElement swathGroupElement = swathGroupNodes.item(i).toElement();
        swathGroups.append(parseSwathGroup(swathGroupElement));
    }

    currentFilePath = filePath;
    isModified = false;
    return true;
}

bool ProjectMgr::saveProject(const QString& filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }

    // Create new document
    QDomDocument newDoc;
    newDoc.appendChild(newDoc.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\""));
    
    QDomElement projectElement = newDoc.createElement("Project");
    projectElement.setAttribute("version", "2");

    // Add all SwathGroups
    for (const SwathGroup& group : swathGroups) {
        projectElement.appendChild(createSwathGroupElement(group));
    }

    newDoc.appendChild(projectElement);

    // Write to file
    QTextStream out(&file);
    out << newDoc.toString(4); // 4 spaces indentation
    file.close();
    
    currentFilePath = filePath;
    isModified = false;
    return true;
}

bool ProjectMgr::save() {
    if (currentFilePath.isEmpty()) {
        return false;
    }
    return saveProject(currentFilePath);
}

bool ProjectMgr::saveAs(const QString& filePath) {
    return saveProject(filePath);
}

QString ProjectMgr::getCurrentFilePath() const {
    return currentFilePath;
}

bool ProjectMgr::hasUnsavedChanges() const {
    return isModified;
}

bool ProjectMgr::addSwathGroup(const SwathGroup& group) {
    if (findSwathGroup(group.name)) {
        return false; // Group with this name already exists
    }
    swathGroups.append(group);
    isModified = true;
    return true;
}

bool ProjectMgr::removeSwathGroup(const QString& name) {
    for (int i = 0; i < swathGroups.size(); ++i) {
        if (swathGroups[i].name == name) {
            swathGroups.removeAt(i);
            isModified = true;
            return true;
        }
    }
    return false;
}

bool ProjectMgr::updateSwathGroup(const QString& name, const SwathGroup& newGroup) {
    for (int i = 0; i < swathGroups.size(); ++i) {
        if (swathGroups[i].name == name) {
            swathGroups[i] = newGroup;
            isModified = true;
            return true;
        }
    }
    return false;
}

SwathGroup* ProjectMgr::findSwathGroup(const QString& name) {
    for (int i = 0; i < swathGroups.size(); ++i) {
        if (swathGroups[i].name == name) {
            return &swathGroups[i];
        }
    }
    return nullptr;
}

QVector<SwathGroup> ProjectMgr::getAllSwathGroups() const {
    return swathGroups;
}

SwathGroup ProjectMgr::parseSwathGroup(const QDomElement& element) {
    SwathGroup group;
    group.name = element.attribute("name");
    group.visible = element.attribute("visible") == "1";

    QDomElement folderElement = element.firstChildElement("Folder");
    group.folder = folderElement.text();

    QDomElement pathProcessorElement = element.firstChildElement("PathProcessorParameters");

    QDomElement processingElement = element.firstChildElement("Processing");
    if (!processingElement.isNull()) {
        QDomNodeList arrayNodes = processingElement.elementsByTagName("Array");
        for (int i = 0; i < arrayNodes.count(); ++i) {
            QDomElement arrayElement = arrayNodes.item(i).toElement();
            group.arrays.append(parseArray(arrayElement));
        }
    }

    QDomElement propagationVelocityElement = element.firstChildElement("PropagationVelocity");
    group.propagationVelocity = propagationVelocityElement.attribute("value").toDouble();

    return group;
}

Array ProjectMgr::parseArray(const QDomElement& element) {
    Array array;
    array.antennaName = element.attribute("antennaName");
    array.id = element.attribute("id").toInt();

    QDomNodeList processingParamsNodes = element.elementsByTagName("DataProcessingParameters");
    for (int i = 0; i < processingParamsNodes.count(); ++i) {
        QDomElement paramsElement = processingParamsNodes.item(i).toElement();
        array.processingParams.append(parseDataProcessingParameters(paramsElement));
    }

    return array;
}

DataProcessingParameters ProjectMgr::parseDataProcessingParameters(const QDomElement& element) {
    DataProcessingParameters params;
    params.cutType = element.attribute("cutType");
    params.name = element.attribute("name");

    QDomElement rangeElement = element.firstChildElement("Range");
    params.rangeMin = rangeElement.attribute("min").toDouble();
    params.rangeMax = rangeElement.attribute("max").toDouble();
    params.rangeMode = rangeElement.attribute("mode").toInt();

    params.filterItems = parseFilterItems(element);

    return params;
}

QVector<QMap<QString, QString>> ProjectMgr::parseFilterItems(const QDomElement& element) {
    QVector<QMap<QString, QString>> filterItems;
  const QDomNodeList filterNodes = element.elementsByTagName("FilterItem");
    
    for (int i = 0; i < filterNodes.count(); ++i) {
        QDomElement filterElement = filterNodes.item(i).toElement();
        QMap<QString, QString> filterItem;
        
        filterItem["enabled"] = filterElement.attribute("enabled");
        filterItem["name"] = filterElement.attribute("name");
        
        QDomNodeList paramNodes = filterElement.elementsByTagName("Parameter");
        for (int j = 0; j < paramNodes.count(); ++j) {
            QDomElement paramElement = paramNodes.item(j).toElement();
            QString paramName = paramElement.attribute("name");
            QString paramValue = paramElement.attribute("value");
            QString paramUom = paramElement.attribute("uom");
            
            if (!paramUom.isEmpty()) {
                paramValue += " " + paramUom;
            }
            
            filterItem[paramName] = paramValue;
        }
        
        filterItems.append(filterItem);
    }
    
    return filterItems;
}

QDomElement ProjectMgr::createSwathGroupElement(const SwathGroup& group) {
    QDomElement element = doc.createElement("SwathGroup");
    element.setAttribute("name", group.name);
    element.setAttribute("visible", group.visible ? "1" : "0");

    QDomElement folderElement = doc.createElement("Folder");
    folderElement.appendChild(doc.createTextNode(group.folder));
    element.appendChild(folderElement);

    QDomElement processingElement = doc.createElement("Processing");
    for (const Array& array : group.arrays) {
        processingElement.appendChild(createArrayElement(array));
    }
    element.appendChild(processingElement);

    QDomElement propagationVelocityElement = doc.createElement("PropagationVelocity");
    propagationVelocityElement.setAttribute("value", QString::number(group.propagationVelocity));
    element.appendChild(propagationVelocityElement);

    return element;
}

QDomElement ProjectMgr::createArrayElement(const Array& array) {
    QDomElement element = doc.createElement("Array");
    element.setAttribute("antennaName", array.antennaName);
    element.setAttribute("id", QString::number(array.id));

    for (const DataProcessingParameters& params : array.processingParams) {
        element.appendChild(createDataProcessingParametersElement(params));
    }

    return element;
}

QDomElement ProjectMgr::createDataProcessingParametersElement(const DataProcessingParameters& params) {
    QDomElement element = doc.createElement("DataProcessingParameters");
    element.setAttribute("cutType", params.cutType);
    element.setAttribute("name", params.name);

    QDomElement rangeElement = doc.createElement("Range");
    rangeElement.setAttribute("min", QString::number(params.rangeMin));
    rangeElement.setAttribute("max", QString::number(params.rangeMax));
    rangeElement.setAttribute("mode", QString::number(params.rangeMode));
    element.appendChild(rangeElement);

    element.appendChild(createFilterItemsElement(params.filterItems));

    return element;
}

QDomElement ProjectMgr::createFilterItemsElement(const QVector<QMap<QString, QString>>& filterItems) {
    QDomElement element = doc.createElement("FilterItems");
    
    for (const QMap<QString, QString>& filterItem : filterItems) {
        QDomElement filterElement = doc.createElement("FilterItem");
        filterElement.setAttribute("enabled", filterItem["enabled"]);
        filterElement.setAttribute("name", filterItem["name"]);
        
        for (auto it = filterItem.begin(); it != filterItem.end(); ++it) {
            if (it.key() != "enabled" && it.key() != "name") {
                QDomElement paramElement = doc.createElement("Parameter");
                paramElement.setAttribute("name", it.key());
                
                QString value = it.value();
                QString uom;
                int spacePos = value.indexOf(' ');
                if (spacePos != -1) {
                    uom = value.mid(spacePos + 1);
                    value = value.left(spacePos);
                }
                
                paramElement.setAttribute("value", value);
                if (!uom.isEmpty()) {
                    paramElement.setAttribute("uom", uom);
                }
                
                filterElement.appendChild(paramElement);
            }
        }
        
        element.appendChild(filterElement);
    }
    
    return element;
}
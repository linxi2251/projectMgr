#ifndef PROJECTMGR_H
#define PROJECTMGR_H

#include <QDomDocument>
#include <QFile>
#include <QVector>

struct DataProcessingParameters {
  QString cutType;
  QString name;
  double rangeMin;
  double rangeMax;
  int rangeMode;
  QVector<QMap<QString, QString>> filterItems;
};

struct Array {
  QString antennaName;
  int id;
  QVector<DataProcessingParameters> processingParams;
};

struct SwathGroup {
  QString name;
  bool visible;
  QString folder;
  QVector<Array> arrays;
  double propagationVelocity;
};

class ProjectMgr {
public:
  ProjectMgr();
  ~ProjectMgr();

  // File operations
  bool loadProject(const QString &filePath);
  bool saveProject(const QString &filePath);
  bool save();  // Save to current file
  bool saveAs(const QString &filePath);  // Save to new file
  QString getCurrentFilePath() const;  // Get current file path
  bool hasUnsavedChanges() const;  // Check if there are unsaved changes

  // SwathGroup operations
  bool addSwathGroup(const SwathGroup &group);
  bool removeSwathGroup(const QString &name);
  bool updateSwathGroup(const QString &name, const SwathGroup &newGroup);
  SwathGroup *findSwathGroup(const QString &name);
  QVector<SwathGroup> getAllSwathGroups() const;

private:
  QVector<SwathGroup> swathGroups;
  QDomDocument doc;
  QString currentFilePath;  // Track current file path
  bool isModified;  // Track if there are unsaved changes

  // Helper methods
  SwathGroup parseSwathGroup(const QDomElement &element);
  static Array parseArray(const QDomElement &element);
  static DataProcessingParameters
  parseDataProcessingParameters(const QDomElement &element);
  static QVector<QMap<QString, QString>>
  parseFilterItems(const QDomElement &element);
  QDomElement createSwathGroupElement(const SwathGroup &group);
  QDomElement createArrayElement(const Array &array);
  QDomElement
  createDataProcessingParametersElement(const DataProcessingParameters &params);
  QDomElement
  createFilterItemsElement(const QVector<QMap<QString, QString>> &filterItems);
};

#endif // PROJECTMGR_H

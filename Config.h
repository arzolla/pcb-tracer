#ifndef CONFIG_H
#define CONFIG_H

#include <QColor>
#include <QVariantMap>
#include <QMap>
#include "enums.h"

class Editor;
class Link;

// Configuration keys
namespace ConfigKeys {
    constexpr const char* LAST_DIRECTORY = "lastDirectory";
}

class Config {
public:
    static Config* instance();
    
    // Color and appearance settings
    QString color(LinkSide side) const;
    QString color(Color color) const;
    void apply();
    void updateFromConfigDialog(const QVariantMap& dialogConfig);
    void update(const QVariantMap& kwargs);
    QVariantMap toDict() const;
    void readConfigFromBinary(QDataStream& in);
    
    // Persistent configuration management
    QString getConfigValue(const QString& key, const QString& defaultValue = "") const;
    void setConfigValue(const QString& key, const QString& value);
    
    int m_linkWidth;
    int m_padSize;

private:
    Config();
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;

    static Config* m_instance;
    static const QString CONFIG_FILE_NAME;

    QMap<Color, QString> m_colors;
    
    // Helper methods
    QString getConfigFilePath() const;
    QStringList readConfigFile() const;
    void writeConfigFile(const QStringList& lines) const;

};

#endif // CONFIG_H
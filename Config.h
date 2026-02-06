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

    // Appearance settings
    constexpr const char* COLOR_FRONT = "colorFront";
    constexpr const char* COLOR_BACK = "colorBack";
    constexpr const char* COLOR_WIP = "colorWip";
    constexpr const char* COLOR_NOTES = "colorNotes";
    constexpr const char* COLOR_HIGHLIGHTED = "colorHighlighted";
    constexpr const char* COLOR_NODE = "colorNode";
    constexpr const char* LINK_WIDTH = "linkWidth";
    constexpr const char* PAD_SIZE = "padSize";
}

class Config {
public:
    static Config* instance();
    QString color(LinkSide side) const;
    QString color(Color color) const;
    void apply();
    void updateFromConfigDialog(const QVariantMap& dialogConfig);
    void update(const QVariantMap& kwargs);
    QVariantMap toDict() const;
    void readConfigFromBinary(QDataStream& in);
    
    // Generic persistent configuration management
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
    void loadSettingsFromConfig();
};

#endif // CONFIG_H
#include "Config.h"
#include "Link.h"
#include "Node.h"
#include "Editor.h"
#include "Component.h"
#include <QDebug>
#include <QApplication>
#include <QFile>
#include <QTextStream>

Config* Config::m_instance = nullptr;
const QString Config::CONFIG_FILE_NAME = "config.cfg";

Config* Config::instance() {
    if (!m_instance) {
        m_instance = new Config();
    }
    return m_instance;
}

Config::Config() {
    // Initialize default colors
    m_colors = {
        {Color::FRONT, "#FF0000"},
        {Color::BACK, "#00FF00"},
        {Color::WIP, "#00FFFF"},
        {Color::NOTES, "#FFFFFF"},
        {Color::HIGHLIGHTED, "#FFFFFF"},
        {Color::NODE, "#00FF00"}
    };

    // Initialize other default settings
    m_linkWidth = 6;
    m_padSize = 12;
    
    // Load settings from config file
    loadSettingsFromConfig();
}

void Config::loadSettingsFromConfig() {
    qDebug() << "Config: Loading settings from" << getConfigFilePath();
    
    // Load colors - check if present, else use defaults and save
    QString colorFront = getConfigValue(ConfigKeys::COLOR_FRONT);
    if (colorFront.isEmpty()) {
        setConfigValue(ConfigKeys::COLOR_FRONT, m_colors[Color::FRONT]);
    } else {
        m_colors[Color::FRONT] = colorFront;
    }
    
    QString colorBack = getConfigValue(ConfigKeys::COLOR_BACK);
    if (colorBack.isEmpty()) {
        setConfigValue(ConfigKeys::COLOR_BACK, m_colors[Color::BACK]);
    } else {
        m_colors[Color::BACK] = colorBack;
    }
    
    QString colorWip = getConfigValue(ConfigKeys::COLOR_WIP);
    if (colorWip.isEmpty()) {
        setConfigValue(ConfigKeys::COLOR_WIP, m_colors[Color::WIP]);
    } else {
        m_colors[Color::WIP] = colorWip;
    }
    
    QString colorNotes = getConfigValue(ConfigKeys::COLOR_NOTES);
    if (colorNotes.isEmpty()) {
        setConfigValue(ConfigKeys::COLOR_NOTES, m_colors[Color::NOTES]);
    } else {
        m_colors[Color::NOTES] = colorNotes;
    }
    
    QString colorHighlighted = getConfigValue(ConfigKeys::COLOR_HIGHLIGHTED);
    if (colorHighlighted.isEmpty()) {
        setConfigValue(ConfigKeys::COLOR_HIGHLIGHTED, m_colors[Color::HIGHLIGHTED]);
    } else {
        m_colors[Color::HIGHLIGHTED] = colorHighlighted;
    }
    
    QString colorNode = getConfigValue(ConfigKeys::COLOR_NODE);
    if (colorNode.isEmpty()) {
        setConfigValue(ConfigKeys::COLOR_NODE, m_colors[Color::NODE]);
    } else {
        m_colors[Color::NODE] = colorNode;
    }
    
    // Load link width - check if present, else use default and save
    QString linkWidthStr = getConfigValue(ConfigKeys::LINK_WIDTH);
    if (linkWidthStr.isEmpty()) {
        setConfigValue(ConfigKeys::LINK_WIDTH, QString::number(m_linkWidth));
    } else {
        m_linkWidth = linkWidthStr.toInt();
    }
    
    // Load pad size - check if present, else use default and save
    QString padSizeStr = getConfigValue(ConfigKeys::PAD_SIZE);
    if (padSizeStr.isEmpty()) {
        setConfigValue(ConfigKeys::PAD_SIZE, QString::number(m_padSize));
    } else {
        m_padSize = padSizeStr.toInt();
    }
    
    qDebug() << "Config: Settings loaded successfully";
}

void Config::apply() {
    int nodeRadius = m_padSize / 2;
    for (QGraphicsItem* item : Editor::instance()->scene()->items()) {
        if (auto* link = dynamic_cast<Link*>(item)) {
            link->setColor(ColorUtils::fromLinkSide(link->m_side));
            //link->setPen(QPen(QColor(color(ColorUtils::fromLinkSide(link->m_side))), m_linkWidth));
        } else if (auto* node = dynamic_cast<Node*>(item)) {
            node->setColor(Color::NODE);
        } else if (auto* pad = dynamic_cast<Pad*>(item)) {
            node->setColor(Color::NODE);
        }
    }
    // trigger re-rendering of color box
    Editor::instance()->setCurrentSide(Editor::instance()->m_currentSide); 
}

void Config::updateFromConfigDialog(const QVariantMap& dialogConfig) {
    // Update colors
    QVariantMap colorMap = dialogConfig["colors"].toMap();
    for (auto it = colorMap.begin(); it != colorMap.end(); ++it) {
        Color color = ColorUtils::fromString(it.key());
        m_colors[color] = it.value().toString();
    }

    // Update other settings
    m_linkWidth = dialogConfig["link_width"].toInt();
    m_padSize = dialogConfig["pad_size"].toInt();
    
    // Save colors to config file
    setConfigValue(ConfigKeys::COLOR_FRONT, m_colors[Color::FRONT]);
    setConfigValue(ConfigKeys::COLOR_BACK, m_colors[Color::BACK]);
    setConfigValue(ConfigKeys::COLOR_WIP, m_colors[Color::WIP]);
    setConfigValue(ConfigKeys::COLOR_NOTES, m_colors[Color::NOTES]);
    setConfigValue(ConfigKeys::COLOR_HIGHLIGHTED, m_colors[Color::HIGHLIGHTED]);
    setConfigValue(ConfigKeys::COLOR_NODE, m_colors[Color::NODE]);
    
    // Save other settings
    setConfigValue(ConfigKeys::LINK_WIDTH, QString::number(m_linkWidth));
    setConfigValue(ConfigKeys::PAD_SIZE, QString::number(m_padSize));
}

void Config::readConfigFromBinary(QDataStream& in) {
    in  >> m_colors[Color::FRONT]
        >> m_colors[Color::BACK]
        >> m_colors[Color::HIGHLIGHTED]
        >> m_colors[Color::NODE]
        >> m_colors[Color::NOTES]
        >> m_colors[Color::WIP]
        >> m_linkWidth
        >> m_padSize;
}

QString Config::color(LinkSide side) const {
    return m_colors.value(ColorUtils::fromLinkSide(side), "#000000");
}

QString Config::color(Color color) const {
    return m_colors.value(color, "#000000");
}

QVariantMap Config::toDict() const {
    QVariantMap result;
    QVariantMap colorMap;
    for (auto it = m_colors.begin(); it != m_colors.end(); ++it) {
        colorMap[QString::number(static_cast<int>(it.key()))] = it.value();
    }
    result["colors"] = colorMap;
    result["link_width"] = m_linkWidth;
    result["pad_size"] = m_padSize;
    return result;
}

QString Config::getConfigFilePath() const {
    return QApplication::applicationDirPath() + "/" + CONFIG_FILE_NAME;
}

QStringList Config::readConfigFile() const {
    QStringList lines;
    QFile file(getConfigFilePath());
    
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        while (!in.atEnd()) {
            QString line = in.readLine();
            if (!line.isEmpty()) {
                lines.append(line);
            }
        }
        file.close();
        qDebug() << "Config: Read" << lines.size() << "entries from" << getConfigFilePath();
    } else {
        qDebug() << "Config: Could not open file for reading:" << getConfigFilePath();
    }
    
    return lines;
}

void Config::writeConfigFile(const QStringList& lines) const {
    QFile file(getConfigFilePath());
    
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        for (const QString& line : lines) {
            out << line << "\n";
        }
        file.close();
        qDebug() << "Config: Wrote" << lines.size() << "entries to" << getConfigFilePath();
    } else {
        qDebug() << "Config: Failed to write config to:" << getConfigFilePath();
    }
}

QString Config::getConfigValue(const QString& key, const QString& defaultValue) const {
    QString searchKey = key + "=";
    QStringList lines = readConfigFile();
    
    for (const QString& line : lines) {
        if (line.startsWith(searchKey)) {
            QString value = line.mid(searchKey.length());
            qDebug() << "Config: Read" << key << "=" << value;
            return value;
        }
    }
    
    qDebug() << "Config: Key not found, using default for" << key;
    return defaultValue;
}

void Config::setConfigValue(const QString& key, const QString& value) {
    QString searchKey = key + "=";
    QStringList lines = readConfigFile();
    bool keyFound = false;
    
    // Update existing key or mark as not found
    for (int i = 0; i < lines.size(); ++i) {
        if (lines[i].startsWith(searchKey)) {
            lines[i] = searchKey + value;
            keyFound = true;
            break;
        }
    }
    
    // Append key if not found
    if (!keyFound) {
        lines.append(searchKey + value);
    }
    
    qDebug() << "Config: Setting" << key << "=" << value << (keyFound ? "(updated)" : "(new)");
    writeConfigFile(lines);
}

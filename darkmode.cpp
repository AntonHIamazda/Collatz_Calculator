#include "darkmode.h"
#include <QSettings>

bool isDarkMode() {
    QSettings settings(R"(HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Themes\Personalize)", QSettings::NativeFormat);
    return settings.value(L"AppsUseLightTheme", 1).toInt() == 0; // 0 - темна, 1 - світла
}

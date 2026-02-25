# Сборка Cinema 4D DollyZoom Plugin

Исходный код плагина был написан под очень старую версию Cinema 4D SDK (до R20), поэтому при попытке компиляции в современном окружении (2025+) возникало множество ошибок. 

## Что было исправлено в коде
1. **ToolData API:** Базовый класс инструмента `CPluginToolObject` и `CToolData` заменен на современный `cinema::ToolData`.
2. **Обработка мыши:** Устаревшие методы `MouseDown`, `MouseMove`, `MouseUp` заменены на современный цикл перетаскивания `win->MouseDragStart()`, `win->MouseDrag()` и `win->MouseDragEnd()`.
3. **Объекты и параметры:** Использование `BaseCameraObject` заменено на `cinema::CameraObject`. 
4. **Константы:** Старые макросы `QSHIFT_SHIFT` и `QSHIFT_CTRL` заменены на современные `QSHIFT` и `QCTRL` (из флага `BFM_INPUT_QUALIFIER`).
5. **Чтение/запись свойств (Parameters):** Старый способ чтения свойств через `DescLevel` заменен на использование современной обертки `ConstDescIDLevel(CAMERAOBJECT_FOV)`. Также устранен конфликт пространств имен (использовано `maxon::PI` вместо просто `PI`).
6. **Entry Points (Точки входа):** Добавлены обязательные стандартные функции инициализации плагина `PluginStart`, `PluginEnd` и `PluginMessage` внутри пространства имен `namespace cinema`, чтобы линкер смог собрать итоговую библиотеку (DLL).

## Как работает механизм сборки
Для поддержки разных версий Cinema 4D проект разделен на две папки SDK:
*   `sdk_2025` — для Cinema 4D 2025
*   `sdk_2026` — для Cinema 4D 2026

Каждая папка содержит свой файл `custom_paths.txt` со строкой `MODULE ..`, что позволяет системе сборки Maxon находить исходный код плагина в родительской директории.

## Как пересобрать плагин

**Условие:** Установлены Visual Studio 2022 (C++ Desktop development) и Python 3.

Для сборки используйте соответствующие пакетные файлы в корне проекта:

1.  **Для Cinema 4D 2025 (R25):** запустите [build_2025.bat](file:///c:/Users/user/Desktop/cpp/C4D_DollyZoom/build_2025.bat)
2.  **Для Cinema 4D 2026:** запустите [build_2026.bat](file:///c:/Users/user/Desktop/cpp/C4D_DollyZoom/build_2026.bat)

**Что делают скрипты:**
*   Очищают или создают папку `build` внутри соответствующего SDK.
*   Конфигурируют проект через CMake (используя Visual Studio 17 2022).
*   Компилируют плагин в режиме **Release**.
*   **Очистка:** Удаляют временные `C4D_DollyZoom.pdb` файлы, оставляя только готовый к работе плагин.

## Где найти скомпилированный плагин

Готовый плагин `C4D_DollyZoom.xdl64` (вместе с папкой ресурсов `res`) будет находиться в:
*   **R25:** `.../sdk_2025/build/bin/Release/plugins/C4D_DollyZoom/`
*   **2026:** `.../sdk_2026/build/bin/Release/plugins/C4D_DollyZoom/`

Для установки скопируйте файл `.xdl64` и папку `res` в папку `plugins` вашей версии Cinema 4D.

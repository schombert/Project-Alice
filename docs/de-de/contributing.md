## Interessiert an Mitarbeit?

Hilfe, die ich derzeit suche:

#### Kunst:

Ich suche jemanden, der im Grunde genommen alle vorhandenen Victoria 2 UI-Elemente in einem neuen künstlerischen Stil neu gestalten könnte, um dem Spiel eine eigene unverwechselbare visuelle Identität zu verleihen. Ja, ich weiß, dass dies keine triviale Aufgabe ist.

#### Tester:

Ich suche Leute, die ein C++-Projekt bauen und ausführen können, aber nicht Programmcode beitragen möchten (oder sich dabei nicht wohl fühlen). Tester sollen das Projekt bauen und regelmäßig ausprobieren, um Fehler zu finden. Insbesondere solche, die bei Software-/Hardware-Kombinationen auftreten, auf denen ich nicht entwickle. Sprechen Sie mit unserem Projektkoordinator, wenn Sie daran interessiert sind, Tester zu werden.

#### Freie Mitarbeiter

Da es sich um ein Open-Source-Projekt handelt, müssen Sie kein offizielles Mitglied des Teams sein, um beizutragen. Jeder kann einen Pull Request über GitHub erstellen und somit auf einer *ad-hoc*-Art und Weise beitragen.

### Das Projekt bauen

Das verwendete Buildsystem ist CMake und sollte "einfach funktionieren". Mit wenigen ärgerlichen Ausnahmen.

#### Windows

1. Sie benötigen eine vollständige Kopie von Git (https://git-scm.com/downloads), da die mit Visual Studio ausgelieferte Version für CMake aus irgendeinem Grund nicht ausreicht.
2. Stellen Sie sicher, dass Sie eine relativ aktuelle Version des Windows SDK installiert haben (Sie können Ihre lokale Version des Windows SDK über den Visual Studio Installer aktualisieren).
3. Öffnen Sie das Projekt in Visual Studio und lassen Sie es konfigurieren (CMake sollte im Ausgabefenster ausgeführt werden und Abhängigkeiten herunterladen; dies kann einige Zeit dauern).
4. Schauen Sie sich den Abschnitt "Final Touches" am Ende dieser Seite an.

Wenn Sie Probleme mit der Audiowiedergabe haben, möchten Sie möglicherweise Windows Media Player und das Windows Media Feature Pack installieren (https://support.microsoft.com/de-de/windows/get-windows-media-player-81718e0d-cfce-25b1-aee3-94596b658287), um MP3-Dateien ordnungsgemäß abspielen zu können (die Musik-Jukebox). 

#### Linux (Debian-basierte Distribution)

Stellen Sie sicher, dass Sie die erforderlichen Abhängigkeiten installiert haben.

```bash
sudo apt update
sudo apt install git build-essential clang cmake libgl1-mesa-dev libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev
```

Nach Abschluss der Installation laden Sie die Erweiterung `cmake-tools` für Visual Studio Code herunter und öffnen Sie das Projekt. CMake sollte das Projekt automatisch für Sie konfigurieren.

Wenn Sie den Fehler `/usr/bin/ld: cannot find -lstdc++` erhalten, könnte dies durch ein defektes gcc-12-Paket in Ubuntu Jammy verursacht werden.
```bash
sudo apt remove gcc-12
```
https://stackoverflow.com/questions/67712376/after-updating-gcc-clang-cant-find-libstdc-anymore

(Linux only) Die von uns verwendete Version der Intel TBB-Bibliothek scheint fehlerhaft zu kompilieren, wenn sich Leerzeichen im Pfad befinden. Stellen Sie daher sicher, dass der Projektordner keinen Leerzeichen im gesamten Pfad enthält (ja, das erscheint mir auch seltsam).

#### Linux (Generisch)

Dies gilt für nicht auf Debian basierende Distributionen. Benötigte Programme:
Grundlegende Kompilierungswerkzeuge (C-Compiler & C++-Compiler)
CMake
Git
onetbb
GLFW3
X11 (Unterstützung *sollte* für Wayland vorhanden sein, es gibt jedoch keine Garantie.)

Von hier aus ist das Kompilieren unkompliziert.
1. `cd Project-Alice`
2. `cmake -E make_directory build`
3. `cmake -E chdir build cmake ..`
4. `touch src/local_user_settings.hpp`
5. `nano src/local_user_settings.hpp` oder verwenden Sie den Texteditor Ihrer Wahl.
6. Fügen Sie die folgenden Zeilen hinzu:
    ```cpp
        #ifndef GAME_DIR
        #define GAME_DIR "[insert file path here]"
        #define OOS_DAILY_CHECK 1
        #endif

    ```
  Du solltest GAME_DIR auf den Ordner setzen, der deine Vic2-Dateien enthält. Wenn du das Spiel auf Steam heruntergeladen hast, kannst du mit einem Rechtsklick auf Vic2 > Lokale Dateien durchsuchen, und das wird dir den richtigen Pfad geben. Auf Linux ist es wichtig zu beachten, dass du keine \ vor Leerzeichen setzen musst. Wenn dein Linux-Dateipfad also /home/user/Victoria\ 2/ ist, schreibst du /home/user/Victoria 2/ bei GAME_DIR (natürlich in Anführungszeichen).
    Kopieren Sie den Dateipfad und ersetzen Sie damit [insert file path here], dann speichern Sie. Zusätzlich sollte `OOS_DAILY_CHECK` definiert werden, wenn Sie tägliche OOS-Überprüfungen anstelle der üblichen "monatlichen" möchten.

7. `cd build && cmake -DCMAKE_BUILD_TYPE=Debug ..`
8. `cmake --build . -j$(nproc)`

#### Letzte Handgriffe

Da das Projekt in seinem aktuellen Zustand die vorhandenen Spieldateien verwenden muss (als Quelle für Grafiken usw.), muss jeder dem Compiler mitteilen, wo seine Kopie des Spiels installiert ist. Dies geschieht, indem eine Datei mit dem Namen `local_user_settings.hpp` im Verzeichnis `src` erstellt wird.
Diese Datei sollte die folgenden vier Zeilen enthalten (die letzte ist eine leere Zeile):
```cpp
#ifndef GAME_DIR
#define GAME_DIR "C:\\Your\\Victoria2\\Files"
#define OOS_DAILY_CHECK 1
#endif
```
Hier ist die Übersetzung ins Deutsche:

Ersetzen Sie dabei diesen Pfad durch Ihren eigenen Installationsort.
Beachten Sie, dass auf Windows für jeden Pfadtrenner `\\` anstelle von nur `\` geschrieben werden muss. (Linux hat dieses Problem nicht, und Sie können einfach `/` schreiben.)
Zweite Anmerkung: Auf Windows hat BrickPi eine Änderung vorgenommen, sodass Sie, wenn Sie Victoria 2 installiert haben, möglicherweise das Erstellen von `local_user_settings.hpp` vollständig umgehen können. Sie möchten das vielleicht zuerst versuchen.
Schließlich bauen Sie einfach das Alice launch target, und Sie sollten das Spiel auf Ihrem Bildschirm sehen.

Denken Sie daran, dass `OOS_DAILY_CHECK` dazu führt, dass OOS-Überprüfungen täglich durchgeführt werden.

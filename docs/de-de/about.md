## �ber Project Alice

### Getting Started
Mehr Informationen gibt es in docs/contributing.md! Dort wird erkl�rt wie man das Projekt auf Windows und Linux selber baut.

### Installationsanleitung
[Installationsanleitung](../user_guide_de.md)

### Da weitermachen wo Open V2 aufgeh�rt hat

Das Ziel von Project Alice (nach [Alice Roosevelt Longworth](https://en.wikipedia.org/wiki/Alice_Roosevelt_Longworth) bennant) ist es, eine neue Version von [open v2](https://github.com/schombert/Open-V2) zu schaffen -- mein fr�herer Versuch einen Victoria 2 Klon zu erstellen -- und dann eine vollst�ndige und vollst�ndig funktionierende Version des Spiels zu schaffen. Das bedeutet, dass es zumindest anf�nglich nur wenige Unterschiede zu Victoria 2 geben wird, um das Projekt fokussiert und auf Schiene zu halten. Sobald die Version 1.0 Vollendet ist, wird sie uns als Sprungbrett f�r neue Experimente dienen. Stand Juli 2023 habe ich praktisch das Stadium von open v2 erreicht, ausgenommen weniger UI-M�ngel.

Ich hoffe Leute zu finden, die ein Kunst-Team bilden, um Ressourcen aus Victoria 2 zu ersetzen und diesem Projekt einen eigenen Kunststil und Wiedererkennungswert geben. Ich wei�, dass das eine schwere Aufgabe ist, aber zumindest wird keine 3D-Modellierung ben�tigt. Wir haben bereits Zugriff auf neue Flaggen die wir nutzen k�nnen, dank dem SOE Projekt.

### Vergleich mit anderen Projekten

Anders als SOE (Symphony of Empires) ist dieses Projekt eher ein direkter Victoria 2 Klon, w�hrend SOE ein eigenes Spiel ist. Nichtsdestotrotz sehe ich SOE als Schwesterprojekt. Es gibt ein paar Leute die in beiden Projekten aktiv sind (Ich hinterlasse dort gelegentlich die ein oder andere Empfehlung) und, wenn sich unter den Dingen die wir schaffen eine Sache finden l�sst die sie Nutzen k�nnten, bin ich mir sicher, dass der Beitrag anerkannt wird. Genauso werden wir sicherlich f�r teile ihres Werkes bei uns Verwendung finden.

Mit Hinsicht auf das OpenVic2 Projekt, obwohl es sich als Victoria 2 "Klon" vermarktet, wird dort nicht tats�chlich in diese Richtung gearbeit. Es wird eher beabsichtigt einen vollkommen neuen Satz an Assets, Events, Decicions usw. zu schaffen. Das macht OpenVic2 eher zu einem Victoria 2-artigen Spiel. Die Entwicklung von OpenVic2 l�uft auch Organisatorisch sehr anders ab. Bei Interesse an beiden Projekten, also Project Alice und OpenVic2, empfehle ich einfach bei beiden Projekten mitzuarbeiten.

### Lizenz

Jeglicher Programmcode der durch dieses Projekt hervorgeht untersteht der GPL3 Lizenz (wie bei open v2). Die Lizenz f�r grafische Assets steht dem jeweiligen K�nstler offen, jedoch w�rde ich irgendeine Art der Creative Commons bevorzugen.

### Programmier-Tools

Das Projekt selber verwendet CMake(Ja, ich wei�, dass wir es alle hassen.). Idealerweise sollte es mit einem der drei gro�en Compiler gebaut werden: MSVC, Clang/LLVM oder GCC. Ich pers�nlich verwende meist MSVC, aber benutze auch Clang auf Windows. Sicherzustellen, dass alles auch weiterhin mit GCC funktioniert wird jemand anderes Problem sein m�ssen. Und wenn niemand der Leute die zu dem Projekt beitragen sich herausstellt ein GCC-Nutznie�er zu sein, wird GCC-Kompatibilit�t wahrscheinlich von dieser Liste verschwinden.

Ich pers�nlich benutze Visual Studio 2022 (Die kostenlose community-edition) um an diesem Projekt zu arbeiten, aber wenn du zu diesem Projekt beitr�gst, solltest du in der Lage sein die Tools zu verwenden die dir gefallen; Inklusive VS Code, CLion, EMACS, VI usw.

### Wo wird sich dieses Projekt von Victoria 2 unterscheiden?

Anf�nglich wird Project Alice die Victoria 2 Spielmechaniken, mit ein paar wenigen Ausnahmen, imitieren, wo immer wir das hinkriegen. Sicher gibt es viele Dinge die wir verbessern k�nnten. Ich habe open v2 mit der Idee angefangen, dass ich einfach hier und da die ein oder andere Sache am Spiel verbessern w�rde. Das war einer der Hauptgr�nde warum das Projekt scheiterte und aufgegeben wurde. Ich m�chte diesen Fehler nicht noch einmal machen und werde diesmal, so gut wie m�glich, mich davon abhalten teile des Spiels zu verbessern bis wir ein vollst�ndig funktionierendes Spiel haben. Es gibt ein paar �nderungen die wir so oder so machen m�ssen. Die Globus-Kartendarstellung ist einfach zu gut um sie nicht in das Spiel zu packen. Und es gibt ein paar QOL(Quality-of-Life)-improvements wie Hyperlinks die einem die Provinz zeigen wovon ein Event berichtet oder zum Beispiel eine Zusammenfassung der Gr�nde f�r das Triggern eines Events. Diese �nderungen sind einfach und so gut, dass es eine Schande w�re, sie auszulassen. Aber diesmal gibt es keine gro�en �nderungen der Spielmechaniken! Versprochen!

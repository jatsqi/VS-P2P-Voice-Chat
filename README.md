# P2P Voice Chat

![Connect Remote Widget](img/connect_remote.JPG)

![Overview Widget](img/overview.JPG)

![Password Widget](img/password.JPG)

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

**WICHTIG: DAS PASSWORT ZU DEN STANDARTCHANNELN IST "passwort"**

Siehe dazu `m_MetadataServer->createChannel(name, passwort)` in `mainwindow.cpp`!

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

Mit diesem kleinen P2P Voice Chat Programm soll eine, jedenfalls was den Austausch der Sprache selbst angeht, ein serverloses Tool im Rahmen der Vorlesung "Verteilte Systeme" entwickelt werden.
Bevor der eigentliche Aufbau bzw. die Entwicklung im Detail besprochen wird, kurz noch ein paar kleinere Anmerkungen:

- Auf dem Server können vor dem Start dessen mehrere Channel definiert werden
- Aktuell ist es möglich, dass sich zwei Clients gleichzeitig in einem Channel befinden
- Die Anzahl der Channel ist theoretisch unbegrenzt, da die Serverlast extrem gering ist.
- Der Server hat **nichts** mit dem Austausch der Voice-Pakete zu tun!

## Projektkontext

- _Entwicklungszeit_: ca. 35h, ähnlich der Schätzung aus der Vorlesung
  - Die 35h setzen sich in etwa zusammen aus 5-6h Recherche, der Rest war die Einarbeitung in das Framework Qt und die Realisierung der Rechercheergebnisse.
- _Framework_: Da das Projekt eine GUI aufweist und **komplett** in C++ geschrieben wurde, habe ich mich für das Framework **Qt** entschieden. Es bietet neben GUI Elementen auch zahlreiche Tools für Netzwerkschnittstellen an und vereinfacht die Entwicklung in dieser hinsicht deutlich!
- _Testsystem_: Getestet wurde die Anwendung primär auf zwei Systemen mit Windows 10. Während des Tests verlief sowohl der Metadaten-Austausch als auch der Austausch der Voice-Pakete reibungslos. Beim Testen auf zwei Linux Systemen kam es, je nach Distro zu kleineren bis größeren Problemen, besonders bei der Audio-Qualität. Teilweise waren heftige Störgeräusche wahrnehmbar, was sich eventuell auf die Neuheit des Multimedia-Moduls in Qt 6 zurückführen lässt, als auch auf eventuell Probleme beim Buffering. Da die Projektzeit mit ca. 35h für eine Person bereits deutlich erreicht wurde, konnte ich auch aufgrund anderer Projekte (Bachelorarbeit...) keine weitere Zeit in das Bug-Fixing investieren :(. 

## Featureübersicht

* Benutzername ist konfigurierbar
* IP/Port konfigurierbar, jeweils vom Client, der sich connecten möchte, als auch vom lokalen Server (falls gestartet wird).
* Auf dem Server können (aktuell noch hard-coded), mehrere Channel für jeweils zwei Clients erstellt werden.
* UDP Holepunching ist implementiert und theoretisch funktionsfähig, allerdings konnte ich kein Testsystem zusammenbauen, um es zu testen.
* Channel können beliebig betreten bzw. verlassen werden.
* Sprache wird per UDP ausgetaucht, der Server ist **NICHT** beteiligt.
* Metadaten werden per TCP ausgetauscht. Dafür habe ich mir ein kleines und sehr simples Protokoll zum Austausch der Metadaten überlegt.
* Rudimentärer Jitter Buffer zum Abfangen von unterschiedlichen Paketlaufzeiten (Wie lange die Pakete brauchen, um anzukommen).

## Verwendete Tools

- Qt Creator 7.3 als IDE
- Qt 6.3 als Framework. **Bitte zum Builden Qt 6 oder höher verwenden, da ich viele der neuen Multimedia-Funktionen nutze!** 
- VS Code für alle Dokumente
- PlantUML für alle UML Diagramme

## Builden des Projektes

```shell
git clone https://github.com/jatsqi/VS-P2P-Voice-Chat
cd VS-P2P-Voice-Chat
mkdir build
cd build
cmake ..
```

Eventuell muss bei CMAKE noch die installierte Qt Version + Tool-Chain spezifiziert werden:

`cmake -DCMAKE_PREFIX_PATH=<QT-Installation-Path-With-Toolchain> ..`

Unter Windows könnte der Pfad beispielsweise so aussehen: `C:\Qt\6.3.0\mingw_64\`

Der Ordner, in dem QT Installiert wird (z.B. Qt\6.3.0) könnte eventuell nicht ausreichen, deshalb die Tool-Chain anhängen (z.B. mingw_64)!

Zum erfolgreichen Builden sind folgende Qt-Module erforderlich:

* Qt-Core
* Qt-Multimedia
* Qt-Network
* Qt-Widgets

Da ich ausgiebig die recht neuen Multimedia-Features nutze, bitte darauf achten, dass Qt6 oder höher installiert ist!

## Grober Aufbau der Anwendung

Prinzipiell ist die Anwendung in zwei Teile zerlegt:

1. Der **Client**, der den Mikrofoninput an alle ihm bekannten anderen Clients per **UDP** schickt. UDP wurde hier gewählt, da es bei Audio-Paketen prinzipiell verkraftbar ist, wenn einige **wenige** nicht, oder in der falschen Reihenfolge ankommen. Der "normale" User wird von kleinen Aussetzern normalerweise nichts mitbekommen. Des Weiteren ist UDP durch seine verbindungslose Natur **deutlich** schneller und besitzt weniger Overhead.
2. Ein **Metadatenserver**, der die Clients untereinander bekannt macht und das UDP Holepunching ermöglicht. Da für die Metadaten sowohl deren Reihenfolge wichtig ist, in der die Clients diese empfangen also die Sicherheit, dass diese ankommen, wird für den Austausch der Metadaten **TCP** genutzt. Wichtig hierbei ist, dass der Metadaten Server für alle Clients erreichbar sein muss, d.h. er darf sich z.B. nicht hinter einem NAT befinden. Die ursprüngliche Idee war eine baumartige Struktur für die Server: Jeder Client startet einen eigenen, lokalen Server und definiert einen optionalen "Upstream" Server, mitdem die Daten synchronisiert werden. Aufgrund der gesteigerten Komplexität wurde diese Idee allerdings schnell wieder verworfen.  Die aktuelle Architektur sieht vor, dass ein Client, der selber von allen anderen erreicht werden kann, den Server per GUI startet. Auf dem Server selbst sind im Code mehrere Channel erstellbar (siehe CSimpleMetadataServer#createChannel), zu denen sich jeweils zwei Clients connecten können.

### Protokoll zum Austausch der Metadaten

Wie in der vorherigen Sektion bereits erwähnt, wird TCP für den Austausch der Metadaten genutzt.
Die ausgetauschten Paketen besitzen immer denselben, grundlegenden Aufbau: `([AKTION] [PAYLOAD])`.
Aus der Aktion lässt sich die Stuktur der Payload ableiten, die, abhängig davon, ob der Client oder der Server die Aktion empfängt, fest vorgegeben ist.
Je nach dem, wer ein Paket mit einer bestimmten Aktion empfängt, ist die Interpretation eine andere:

| Aktion  | Interpretation des Servers  | Interpretation des Clients  | Payload vom Server | Payload vom Client |
|---|---|---|---|---|
| connect  | Ein Client möchte einem bestimmten Voice-Channel beitreten.  | Der Server hat meine Anfrage auf Beitriff bearbeitet und antwortet mit einem Status-Code. | Status-Code. | Channel + Passwort. |
| identification  | Ein Client möchte sich mit einem Usernamen identifizieren.  | Der Server möche, dass ich mich mit einem Usernamen ausweise.  | Leeres Paket. | Benutzername. |
| overview  | Ein Clients fordert eine Übersicht über alle Channels an.  | Ich fordere eine Übersicht über alle Channel an.  | Übersicht über alle Channel + User. | Leere Payload. |
| disconnect | Ein Client möchte einen Voice-Channel ordentlich verlassen. | Ich oder ein anderer Client hat einen Voice-Channel verlassen. | Betroffener Channel, Betroffener User. | Betroffener User, Betroffener Channel. | Leere Payload.
| client_joined | Ein Client möchte einem Voice-Channel beitreten. | Ich oder ein anderer Client trat einem Channel bei. | Betroffener Channel, Betroffener User.  | - |
| port_discovery | - | Der Server möchte prüfen, über welchen Port ich nach außen erreichbar bin (Vorbereitung UDP Hole Punching). Ich sende ein UDP Paket an den mitgelieferten Port. | - Port, zu dem der Client Paket schicken soll. | - |

Beispielsweise ist die Payload der Aktion "connect", mit der ein Client einem bestimmten Voice-Channel beitreten kann, auf folgende Weise definiert:

```c++
struct ChannelConnectRequest
{
    QString username;
    QString channelName;
    QString password;
};
```

Das Framework Qt bietet über seine eigene Klasse `QDataStream` die Möglichkeit an, komplexere Typen wie z.B. Strings zu serialisieren und als Byte-Array in ein Socket zu schreiben.

```c++
// Serialisieren, d.h. Elemente des Requests in den Stream schreiben
// Dies würde in diesem Fall der Client tun, und den fertigen Stream
// in ein offenes TCP Socket schreiben
inline QDataStream& operator<<(QDataStream &stream, const ChannelConnectRequest& request)
{
    stream << QString("connect");
    stream << request.username << request.channelName << request.password;
    return stream;
}

// Deserialisieren, dies geschieht auf Seite des Servers in diesem Fall. 
// Der Server auf der anderen Seite kann, wenn er die Aktion "connect" liest
// die Daten wieder extrahieren.
inline QDataStream& operator>>(QDataStream &stream, ChannelConnectRequest& request)
{
    stream >> request.username >> request.channelName >> request.password;
    return stream;
}
```

Dadruch ist es möglich, solche Payloads sehr einfach zu serialisieren bzw. zu de-serialiseren, was den Code **deutlich** lesbarer macht.

## Austausch der Sprachpakete

Die Sprachpakete selbst werden über UDP ausgetauscht. Der Metadaten-Server vermittelt dabei zwischen den Clients und tauscht sowohl deren IP als auch deren Port aus.
Jeder Client hört dabei **einen** UDP-Port ab. Somit müssen, falls mehrere Clients verbunden wären und an diesen Port senden würden, die Pakete wieder voneinander getrennt werden und entsprechend zusammen abgespielt werden.
In der aktuellen Version wird deshalb bereits der Username, von dem das UDP Paket kommt, vorne an die eigentliche PCM-Payload angehangen, um dies in Zukunft zu ermöglichen.
Die Pakete werden noch **nicht** voneinander getrennt bzw. gleichzeitig abgespielt, weswegen aktuell jeder Client mit genau einem anderen Client sprechen kann.

Jeder Client besitzt, um Unregelmäßigkeiten in der Anzahl der Empfangenen Pakete zu vermeiden, einen lokalen Buffer.
Dieser Buffer - auch _Jitter Buffer_ genannt - speichert genau eine Sekunde an Voice-Daten zwischen, bevor diese an die Ausgabe übergeben werden.
Da die PCM Daten aktuell **unkomprimiert** übertragen werden, wurde die Sprachqualität entsprechend angepasst, um zu große Datenmengen zu vermeiden:

* Channels: 2
* Abtastrate in Hz: 8000
* Größe pro Sample: 2 Byte (16 Bit)

In Zukunft soll das sehr bekannte Encoding namens _Opus_ eingesetzt werden, was u.a. in erfolgreichen Applikationen wie z.B. Discord eingesetzt wird.
Opus ist eine Verlustbehaftete Kodierung, erreicht allerdings akzeptable Sprachqualität.

## Aktuelle Implementierung des Bufferings

Da bei keinem Netzwerk wirklich garantiert werden kann, alle Pakete in einer **konstanten** Rate am Ziel ankommen, müssen die Sprachpakete gepuffert werden,
um plötzlicher aussetzer zu vermeiden. Dies erhöht natürlich die Latenz, was im Falle dieser Anwendung allerdings verkraftbar ist, sofern diese sich Rahmen hält.
Aktuell wird versucht, immer **eine Sekunde** zu puffern und die in dieser Zeit angekommenen Daten gesammelt und somit mit einer konstanten Rate an den Audio-Output
zu übergeben.
Vor diesem Ansatz sah die Implementierung vor, genau eine Sekunde an **Audio-Daten** zusammen (also tatsächlich genug Daten zusammen, dass eine Sekunde an Audio abgespielt werden kann).
Dies hat allerdings beio plötzlichen Sprechpausen dazu geführt, dass bei Wiederaufnahme Sprachsegmente abgespielt wurden, die teilweise sehr alt waren, da der Buffer z.B. nur bis zur Hälfe gefüllt war.
Der aktuelle Ansatz liest jede Sekunde die vorhandenen Daten aus dem Puffer und schreibt sie in das Audio-Device (`mainwindow.cpp`, `initAudio()` Methode) (versenden/empfangen des Audios: `voice_client.cpp`).
Dennoch hat sich auch mit diesem Ansatz herausgestellt, dass selbst kleine Gaps zu teilweise starken Störgeräuschen bzw. Artefakten führen können, weswegen dies in Zukunft noch ein wenig verbessert werden muss.
Dies tritt auf, wenn genau eine Sekunde an Audio im Buffer liegt, die Sekunde abgespielt wurde und anschließend die nächste Sekunde geholt wird. IDiese kurze Latenz zwischen Neue Daten holen und die Daten in das Device schreiben, reicht bereits, um hörbare Gaps zu produzieren. 

## Beispielhafter Ablauf der Identifizierung

![Identification](http://www.plantuml.com/plantuml/proxy?cache=no&src=https://raw.githubusercontent.com/jatsqi/VS-P2P-Voice-Chat/master/uml/seqIdent.puml&fmt=svg)

## Beispielhafter Ablauf des Beitreten zu einem Channel (stark vereinfacht, einige Komponenten nicht beachtet)

![Identification](http://www.plantuml.com/plantuml/proxy?cache=no&src=https://raw.githubusercontent.com/jatsqi/VS-P2P-Voice-Chat/master/uml/seqJoinChannel.puml&fmt=svg)

Die UDP Hole-Punching Implementierung funktioniert aktuell folgendermaßen:

1. Sobald sich ein Client connecten möchte, sendet der Server die Aktion `port_discovery` an den Client.
2. Das Paket beinhaltet einen Port, zu dem sich der Client verbinden soll per UDP.
3. In der zwischenzeit startet der Server ein UDP Socket und bindet es an den übermittelten Port.
4. Der Client sendet ein UDP Paket an den Server.
5. Sobald der Server dies liest, kennt er sowohl IP, als auch den durch das NAT bereitgestellten Port des Client, sofern dieser hinter einem NAT liegt. Nutzt der Client kein NAT, so kennt er den "echten" Port, vondem aus der Client gesendet hat.
6. Der Server sendet IP + Port an den anderen sich im Channel befindlichen Client und schickt an den neuen Client IP + Port des jeweils anderen.
7. Beide Clients können nun miteinander per UDP kommunizieren => Löcher wurden in das NAT geschlagen.

**ACHTUNG**: DIES FUNKTIONIERT **NICHT** BEI SYMETRISCHEN NATS, DA DER EINTRAG IN DER NAT TABELLE HIER NUR FÜR DEN SERVER GÜLTIG IST, ZU DEM DAS URSPRÜNGLICHE PAKET GESENDET WZRDE


## Kurzbeschreibung der Source-Dateien

| Datei  | Beschreibung  |
|---|---|
| channel_overview_widget  | Beinhaltet das Widget, welches die Übersicht über alle Channel darstellt.  |
| channel_widget | Beinhaltet das Widget, welches einen einzelnen Channel inklusive aller beigetretener User darstellt. |
| connect_remote_widget  | Beinhaltet das Widget, welches Eingabe wie z.B. IP/Port/Username etc. annimmt.  |
| main  | Main Methode. |
| mainwindow  | Vereint die anderen Widgets in einer Klasse + Handelt Erstellung von Metadaten-Server, Metadaten-Client + Voice-Client.  |
| **metadata**  | Beinhaltet die Definition aller Metadaten / Request bzw. Response Pakete.  |
| metadata_client  | Der Client, der sich zum Metadaten-Server verbindet, um Metadaten per TCP auszutauschen.  |
| metadata_server  | Der Server, welche das Handling von Metadaten per TCP abwickelt.  |
| port_discovery  | Beinhaltet sämtliche Utility Klassen für UDP Hole Punching.  |
| random_helper  | Kleinere Hilfsfunktionen.  |
| voice_client | Handelt das Senden/Empfangen von UDP Voice-Paketen.  |

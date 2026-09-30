import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.15
import Qt.labs.settings 1.0

ApplicationWindow {
    id: rootWindow
    visible: true
    width: 800
    height: 600
    title: "Deepframe3D"
    menuBar: menuBar
    
    // Settings for window state persistence
    Settings {
        id: windowSettings
        category: "window"
    }

    // Load saved window state
    Component.onCompleted: {
        width = windowSettings.value("width", 800)
        height = windowSettings.value("height", 600)
        x = windowSettings.value("x", 100)
        y = windowSettings.value("y", 100)
        backend.viewingMode = windowSettings.value("viewingMode", 0)
        backend.slideInterval = windowSettings.value("slideInterval", 2000)
        backend.wiggleInterval = windowSettings.value("wiggleInterval", 200)
    }

    Component.onDestruction: {
        windowSettings.setValue("width", width)
        windowSettings.setValue("height", height)
        windowSettings.setValue("x", x)
        windowSettings.setValue("y", y)
        windowSettings.setValue("viewingMode", backend.viewingMode)
        windowSettings.setValue("slideInterval", backend.slideInterval)
        windowSettings.setValue("wiggleInterval", backend.wiggleInterval)
    }

    // Viewing mode names
    readonly property var modeNames: ["Anaglyph (Red/Cyan)", "Side-by-Side", "Wiggle"]

    // Timer for slideshow
    Timer {
        id: slideTimer
        interval: backend.slideInterval
        running: backend.isPlaying && backend.fileCount > 1
        repeat: true
        onTriggered: backend.nextFile()
    }

    // Timer for wiggle mode
    Timer {
        id: wiggleTimer
        interval: backend.wiggleInterval
        running: backend.viewingMode === 2
        repeat: true
        onTriggered: wiggleState = !wiggleState
    }
    
    property bool wiggleState: false

    // Menu bar
    MenuBar {
        id: menuBar
        Menu {
            title: "File"
            MenuItem { text: "Exit"; onTriggered: Qt.quit() }
        }
        Menu {
            title: "View"
            MenuItem { 
                text: "Fullscreen"; 
                onTriggered: rootWindow.visibility === Window.FullScreen ? rootWindow.showNormal() : rootWindow.showFullScreen()
            }
        }
        Menu {
            title: "View Mode"
            Repeater {
                model: modeNames
                MenuItem {
                    text: modelData
                    checkable: true
                    checked: index === backend.viewingMode
                    onTriggered: backend.viewingMode = index
                }
            }
        }
        Menu {
            title: "Slideshow"
            MenuItem { 
                text: "Play/Pause"; 
                onTriggered: backend.togglePlay()
            }
            MenuItem { 
                text: "Previous"; 
                onTriggered: backend.prevFile()
            }
            MenuItem { 
                text: "Next"; 
                onTriggered: backend.nextFile()
            }
            MenuItem { 
                text: "Faster"; 
                onTriggered: backend.faster()
            }
            MenuItem { 
                text: "Slower"; 
                onTriggered: backend.slower()
            }
        }
    }

    // Status bar
    Label {
        id: statusBar
        text: backend.fileCount > 0 ? 
              ("File: " + backend.currentFileName + 
               " (" + (backend.currentFileIndex + 1) + "/" + backend.fileCount + ")" + 
               " - Mode: " + modeNames[backend.viewingMode]) : 
              "No file loaded"
        horizontalAlignment: Text.AlignHCenter
        font.pixelSize: 12
        height: 24
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        background: Rectangle { color: "#f0f0f0" }
    }

    // Main display area
    Item {
        id: displayArea
        anchors.fill: parent
        anchors.margins: 0
        anchors.bottomMargin: statusBar.height

        // Anaglyph mode - show left image only for now
        Image {
            id: anaglyphDisplay
            visible: backend.viewingMode === 0 && backend.frameCount >= 2
            anchors.fill: parent
            fillMode: Image.PreserveAspect
            source: "image://mpo/left"
        }

        // Side-by-side mode
        Row {
            id: sideBySideDisplay
            visible: backend.viewingMode === 1 && backend.frameCount >= 2
            anchors.fill: parent
            spacing: 0
            
            Image {
                source: "image://mpo/left"
                width: parent.width / 2
                height: parent.height
                fillMode: Image.PreserveAspect
            }
            Image {
                source: "image://mpo/right"
                width: parent.width / 2
                height: parent.height
                fillMode: Image.PreserveAspect
            }
        }

        // Wiggle mode
        Image {
            id: wiggleDisplay
            visible: backend.viewingMode === 2 && backend.frameCount >= 2
            anchors.fill: parent
            fillMode: Image.PreserveAspect
            source: wiggleState ? "image://mpo/right" : "image://mpo/left"
        }

        // Single image display
        Image {
            id: singleImageDisplay
            visible: backend.frameCount === 1
            anchors.fill: parent
            fillMode: Image.PreserveAspect
            source: "image://mpo/left"
        }
    }

    // Keyboard shortcuts - removed for now due to Qt6 compatibility

    // Drop area for drag-and-drop
    DropArea {
        anchors.fill: parent
        onDropped: {
            var paths = []
            for (var i = 0; i < drop.urls.length; i++) {
                var path = drop.urls[i].toString().replace("file://", "")
                paths.push(path)
            }
            if (paths.length > 0) {
                backend.loadFiles(paths)
            }
        }
    }

    // Error display
    Connections {
        target: backend
        function onError(message) {
            statusBar.text = "Error: " + message
        }
    }
}

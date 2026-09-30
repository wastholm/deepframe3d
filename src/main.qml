import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.15

ApplicationWindow {
    id: rootWindow
    visible: true
    width: 800
    height: 600
    title: "Deepframe3D"
    menuBar: menuBar
    
    // Window state persistence will be added back later
    // For now, use hardcoded defaults

    // Viewing mode names
    readonly property var modeNames: ["Anaglyph (Red/Cyan)", "Side-by-Side", "Wiggle"]
    
    // Actions with shortcuts
    Action { id: openAction; text: "Open..."; shortcut: "Ctrl+O"; onTriggered: {} }
    Action { id: exitAction; text: "Exit"; shortcut: "Ctrl+Q"; onTriggered: backend.quit() }
    Action { id: fullscreenAction; text: "Fullscreen"; shortcut: "F12"; onTriggered: rootWindow.visibility === Window.FullScreen ? rootWindow.showNormal() : rootWindow.showFullScreen() }
    
    // Action group for mutually exclusive viewing modes
    ActionGroup {
        id: modeActionGroup
        actions: [modeAnaglyph, modeSideBySide, modeWiggle]
    }
    Action { id: modeAnaglyph; text: "Anaglyph (Red/Cyan)"; checkable: true; checked: backend.viewingMode === 0; onTriggered: backend.viewingMode = 0 }
    Action { id: modeSideBySide; text: "Side-by-Side"; checkable: true; checked: backend.viewingMode === 1; onTriggered: backend.viewingMode = 1 }
    Action { id: modeWiggle; text: "Wiggle"; checkable: true; checked: backend.viewingMode === 2; onTriggered: backend.viewingMode = 2 }

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
            MenuItem { action: openAction }
            MenuItem { action: exitAction }
        }
        Menu {
            title: "View"
            MenuItem { action: fullscreenAction }
            MenuSeparator { }
            MenuItem { action: modeAnaglyph }
            MenuItem { action: modeSideBySide }
            MenuItem { action: modeWiggle }
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
    Rectangle {
        id: displayArea
        anchors.fill: parent
        anchors.margins: 0
        anchors.bottomMargin: statusBar.height
        color: "black"

        // Anaglyph mode
        Image {
            visible: backend.viewingMode === 0 && backend.frameCount >= 2
            anchors.fill: parent
            fillMode: Image.PreserveAspectFit
            source: (visible ? ("image://mpo/left?version=" + backend.imageVersion) : "")
        }

        // Side-by-side mode
        Row {
            visible: backend.viewingMode === 1 && backend.frameCount >= 2
            anchors.fill: parent
            spacing: 0
            
            Image {
                width: parent.width / 2
                height: parent.height
                fillMode: Image.PreserveAspectFit
                source: (parent.visible ? ("image://mpo/left?version=" + backend.imageVersion) : "")
            }
            Image {
                width: parent.width / 2
                height: parent.height
                fillMode: Image.PreserveAspectFit
                source: (parent.visible ? ("image://mpo/right?version=" + backend.imageVersion) : "")
            }
        }

        // Wiggle mode
        Image {
            visible: backend.viewingMode === 2 && backend.frameCount >= 2
            anchors.fill: parent
            fillMode: Image.PreserveAspectFit
            source: (visible ? (wiggleState ? ("image://mpo/right?version=" + backend.imageVersion) : ("image://mpo/left?version=" + backend.imageVersion)) : "")
        }

        // Single image display
        Image {
            visible: backend.frameCount === 1
            anchors.fill: parent
            fillMode: Image.PreserveAspectFit
            source: (visible ? ("image://mpo/left?version=" + backend.imageVersion) : "")
        }
    }

    // Keyboard shortcuts
    Shortcut {
        sequence: "Ctrl+Q"
        onActivated: backend.quit()
    }
    Shortcut {
        sequence: "F12"
        onActivated: rootWindow.visibility === Window.FullScreen ? rootWindow.showNormal() : rootWindow.showFullScreen()
    }

    // Drop area for drag-and-drop
    DropArea {
        anchors.fill: parent
        onDropped: function(drop) {
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

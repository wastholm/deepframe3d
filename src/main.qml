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
    
    // Track previous slide interval for toast messages
    property int _prevSlideInterval: 2000
    
    // Actions with shortcuts
    Action { id: openAction; text: "Open..."; shortcut: "Ctrl+O"; onTriggered: {} }
    Action { id: exitAction; text: "Exit"; shortcut: "Ctrl+Q"; onTriggered: backend.quit() }
    Action { id: fullscreenAction; text: "Fullscreen"; shortcut: "F12"; onTriggered: rootWindow.visibility === Window.FullScreen ? rootWindow.showNormal() : rootWindow.showFullScreen() }
    
    // Slideshow actions with shortcuts
    Action { id: playPauseAction; text: "Play/Pause"; shortcut: "Space"; onTriggered: backend.togglePlay() }
    Action { id: previousAction; text: "Previous"; shortcut: "Left"; onTriggered: backend.prevFile() }
    Action { id: nextAction; text: "Next"; shortcut: "Right"; onTriggered: backend.nextFile() }
    Action { id: fasterAction; text: "Faster"; shortcut: "Up"; onTriggered: backend.faster() }
    Action { id: slowerAction; text: "Slower"; shortcut: "Down"; onTriggered: backend.slower() }
    
    // Actions for viewing modes
    Action { id: modeAnaglyph; text: "Anaglyph (Red/Cyan)"; shortcut: "A"; checkable: true; onTriggered: backend.viewingMode = 0 }
    Action { id: modeSideBySide; text: "Side-by-Side"; shortcut: "S"; checkable: true; onTriggered: backend.viewingMode = 1 }
    Action { id: modeWiggle; text: "Wiggle"; shortcut: "W"; checkable: true; onTriggered: backend.viewingMode = 2 }
    ActionGroup { id: modeActionGroup; actions: [modeAnaglyph, modeSideBySide, modeWiggle] }
    
    // Initialize checked state based on backend
    Component.onCompleted: {
        modeAnaglyph.checked = backend.viewingMode === 0
        modeSideBySide.checked = backend.viewingMode === 1
        modeWiggle.checked = backend.viewingMode === 2
    }

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
            MenuItem { action: playPauseAction }
            MenuItem { action: previousAction }
            MenuItem { action: nextAction }
            MenuItem { action: fasterAction }
            MenuItem { action: slowerAction }
        }
    }

    // Status bar
    Label {
        id: statusBar
        text: backend.fileCount > 0 ? 
              ("File: " + backend.currentFileName + 
               " (" + (backend.currentFileIndex + 1) + "/" + backend.fileCount + ")" +
               (backend.frameCount >= 1 ? " - " + backend.leftFrameWidth + "x" + backend.leftFrameHeight : "") + 
               " - Mode: " + modeNames[backend.viewingMode] + 
               " - " + (backend.isPlaying ? "Playing (" + (backend.slideInterval / 1000).toFixed(1) + "s)" : "Paused")) : 
              "No file loaded"
        horizontalAlignment: Text.AlignHCenter
        font.pixelSize: rootWindow.font.pixelSize
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

        // Anaglyph mode - Dubois optimized
        Item {
            id: anaglyphContainer
            visible: backend.viewingMode === 0 && backend.frameCount >= 2
            anchors.fill: parent
            Image {
                anchors.fill: parent
                fillMode: Image.PreserveAspectFit
                source: (anaglyphContainer.visible ? ("image://mpo/anaglyph?version=" + backend.imageVersion) : "")
            }
        }

        // Side-by-side mode
        Item {
            id: sideBySideContainer
            visible: backend.viewingMode === 1 && backend.frameCount >= 2
            anchors.fill: parent
            
            // Calculate display dimensions for each frame independently
            readonly property real leftAspect: backend.leftFrameWidth / Math.max(1, backend.leftFrameHeight)
            readonly property real rightAspect: backend.rightFrameWidth / Math.max(1, backend.rightFrameHeight)
            readonly property real leftDisplayWidth: height * leftAspect
            readonly property real rightDisplayWidth: height * rightAspect
            readonly property real totalDisplayWidth: leftDisplayWidth + rightDisplayWidth
            readonly property real scale: Math.min(1, width / totalDisplayWidth)
            readonly property real leftScaledWidth: leftDisplayWidth * scale
            readonly property real rightScaledWidth: rightDisplayWidth * scale
            readonly property real displayHeight: height * scale
            
            Image {
                id: leftSideImage
                x: (sideBySideContainer.width - sideBySideContainer.totalDisplayWidth * sideBySideContainer.scale) / 2
                y: (sideBySideContainer.height - sideBySideContainer.displayHeight) / 2
                width: sideBySideContainer.leftScaledWidth
                height: sideBySideContainer.displayHeight
                fillMode: Image.PreserveAspectFit
                source: (sideBySideContainer.visible ? ("image://mpo/left?version=" + backend.imageVersion) : "")
            }
            Image {
                x: leftSideImage.x + leftSideImage.width
                y: (sideBySideContainer.height - sideBySideContainer.displayHeight) / 2
                width: sideBySideContainer.rightScaledWidth
                height: sideBySideContainer.displayHeight
                fillMode: Image.PreserveAspectFit
                source: (sideBySideContainer.visible ? ("image://mpo/right?version=" + backend.imageVersion) : "")
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

    // Sync viewing mode actions with backend changes
    Connections {
        target: backend
        function onViewingModeChanged() {
            modeAnaglyph.checked = backend.viewingMode === 0
            modeSideBySide.checked = backend.viewingMode === 1
            modeWiggle.checked = backend.viewingMode === 2
        }
    }
    
    // Toast notification
    Rectangle {
        id: toast
        visible: false
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: statusBar.top
        anchors.bottomMargin: 10
        width: 300
        height: 40
        color: "#404040"
        opacity: 0
        radius: 4
        
        Label {
            id: toastLabel
            anchors.fill: parent
            anchors.margins: 10
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            color: "white"
            font.pixelSize: rootWindow.font.pixelSize * 1.5
            text: ""
        }
        
        // Fade in and out animation
        SequentialAnimation {
            id: toastAnimation
            running: false
            PropertyAnimation { target: toast; property: "opacity"; to: 0.9; duration: 200 }
            PauseAnimation { duration: 1500 }
            PropertyAnimation { target: toast; property: "opacity"; to: 0; duration: 200 }
        }
    }
    
    // Error display
    Connections {
        target: backend
        function onError(message) {
            statusBar.text = "Error: " + message
        }
        function onIsPlayingChanged() {
            if (backend.isPlaying) {
                showToast("Playing");
            } else {
                showToast("Paused");
            }
        }
        function onSlideIntervalChanged() {
            var delta = backend.slideInterval - _prevSlideInterval;
            if (delta < 0) {
                showToast("Faster: " + (backend.slideInterval / 1000).toFixed(1) + "s");
            } else if (delta > 0) {
                showToast("Slower: " + (backend.slideInterval / 1000).toFixed(1) + "s");
            } else {
                showToast("Speed: " + (backend.slideInterval / 1000).toFixed(1) + "s");
            }
            _prevSlideInterval = backend.slideInterval;
        }
        function onViewingModeChanged() {
            showToast("Mode: " + modeNames[backend.viewingMode]);
        }
    }
    
    // Helper function to show toast
    function showToast(message) {
        toastLabel.text = message;
        toast.visible = true;
        toast.opacity = 0;
        toastAnimation.restart();
    }
}

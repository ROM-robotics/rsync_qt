import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qt.labs.folderlistmodel



Window {
    id: root
    width: 900
    height: 700
    minimumWidth: 900
    maximumWidth: 900
    minimumHeight: 700
    maximumHeight: 900
    visible: true
    title: qsTr("Rsync Helper")
    // Note: font.family at the root Window may not be supported in this import set.
    // If you want to use Roboto globally, either switch to an ApplicationWindow
    // from QtQuick.Controls 2 or use a FontLoader and assign font.family on controls.
    flags: Qt.FramelessWindowHint | Qt.Window

    // Load SF Pro Text (if provided in resources or installed on the system)
    // Note: SF Pro Text is an Apple font and may require proper licensing to embed.
    // If you do not want to embed the TTF, ensure the target system has "SF Pro Text" installed
    // and set the fallback accordingly.
    FontLoader {
        id: sfProLoader
        // Use the .otf file present in the project's resources
        source: "qrc:/fonts/SF-Pro-Text-Regular.otf"
        onStatusChanged: {
            if (status === FontLoader.Ready) {
                console.log("SF Pro Text font loaded: " + sfProLoader.name)
            } else {
                console.log("SF Pro Text not available via qrc; falling back to system fonts")
            }
        }
    }

    // Expose a property that controls can bind to for font.family
    // Fallback to the system font name "SF Pro Text" when the embedded file is not found.
    property string uiFontFamily: sfProLoader.status === FontLoader.Ready ? sfProLoader.name : "SF Pro Text"

    color: "transparent"

    // Gradient background similar to the screenshot
    Rectangle {
        anchors.fill: parent
        radius: 18
        gradient: Gradient {
            GradientStop { position: 0.0; color: "#5b3cc4" }
            GradientStop { position: 0.5; color: "#8a42e8" }
            GradientStop { position: 1.0; color: "#ff6f91" }
        }
    }

    // Top-right circular close button (×)
    Button {
        id: closeBtn
        text: "✕"
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.margins: 10
        width: 36
        height: 36
        font.pixelSize: 16
        font.bold: true
        hoverEnabled: true
        background: Rectangle {
            radius: width / 2
            color: closeBtn.hovered ? "#7d0209" : "#f53b44"
            border.width: 1
        }
        contentItem: Label {
            text: closeBtn.text
            color: closeBtn.hovered ? "#ffffff" : "#000000"
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            anchors.fill: parent
        }
        onClicked: Qt.quit()
        ToolTip.visible: hovered
        ToolTip.text: qsTr("Close")
        }

    // Quick action buttons anchored to the bottom
        Rectangle {
            id: actionFooter
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.margins: 18
            height: 140
            color: "transparent"

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 6
            spacing: 8

            RowLayout {
                Layout.fillWidth: true
                // Left spacer to pad the button grid
                Item { Layout.preferredWidth: 12 }

                GridLayout {
                    columns: 3
                    rowSpacing: 8
                    columnSpacing: 8
                    Layout.fillWidth: true

                    // Custom clickable rectangles used instead of Button to avoid style overlays
                    Rectangle {
                        id: fb1
                        height: 44
                        Layout.fillWidth: true
                        radius: 12
                        property bool hovered: false
                        gradient: Gradient {
                            GradientStop { position: 0.0; color: fb1.hovered ? "#963906" : "#c97447" }
                            GradientStop { position: 1.0; color: fb1.hovered ? "#803d16" : "#a65f35" }
                        }
                        border.width: 0
                        MouseArea {
                            anchors.fill: parent
                            hoverEnabled: true
                            onEntered: parent.hovered = true
                            onExited: parent.hovered = false
                            onClicked: {
                                // Clear logs and run remote destructive remove over SSH
                                rsyncRunner.clearLogs()
                                // remote path requested by user
                                const remotePath = "/home/mr_robot/rom_drivers_ws/"
                                // Run an explicit remove of all files in the workspace instead of calling delete_workspace
                                rsyncRunner.runRemoteCommand(ipEdit.text, passEdit.text, remotePath, "rm -rf /home/mr_robot/rom_drivers_ws/*")
                            }
                        }
                        Text {
                            anchors.centerIn: parent
                            text: "delete_workspace for rom_drivers_ws"
                            color: fb1.hovered ? "#ffffff" : "#000000"
                            font.pixelSize: 13
                        }
                    }

                    Rectangle {
                        id: fb2
                        height: 44
                        Layout.fillWidth: true
                        radius: 12
                        property bool hovered: false
                        gradient: Gradient {
                            GradientStop { position: 0.0; color: fb2.hovered ? "#963906" : "#c97447" }
                            GradientStop { position: 1.0; color: fb2.hovered ? "#803d16" : "#a65f35" }
                        }
                        border.width: 0
                        MouseArea {
                            anchors.fill: parent
                            hoverEnabled: true
                            onEntered: parent.hovered = true
                            onExited: parent.hovered = false
                            onClicked: {
                                // Clear logs and run remote destructive remove over SSH for rom_sdk_ws
                                rsyncRunner.clearLogs()
                                const remotePath = "/home/mr_robot/rom_sdk_ws/"
                                rsyncRunner.runRemoteCommand(ipEdit.text, passEdit.text, remotePath, "rm -rf /home/mr_robot/rom_sdk_ws/*")
                            }
                        }
                        Text {
                            anchors.centerIn: parent
                            text: "delete_workspace for rom_sdk_ws"
                            color: fb2.hovered ? "#ffffff" : "#000000"
                            font.pixelSize: 13
                        }
                    }

                    Rectangle {
                        id: fb3
                        height: 44
                        Layout.fillWidth: true
                        radius: 12
                        property bool hovered: false
                        gradient: Gradient {
                            GradientStop { position: 0.0; color: fb3.hovered ? "#963906" : "#c97447" }
                            GradientStop { position: 1.0; color: fb3.hovered ? "#803d16" : "#a65f35" }
                        }
                        border.width: 0
                        MouseArea {
                            anchors.fill: parent
                            hoverEnabled: true
                            onEntered: parent.hovered = true
                            onExited: parent.hovered = false
                            onClicked: {
                                // Clear logs and run remote destructive remove over SSH for rom_nav2_ws
                                rsyncRunner.clearLogs()
                                const remotePath = "/home/mr_robot/rom_nav2_ws/"
                                rsyncRunner.runRemoteCommand(ipEdit.text, passEdit.text, remotePath, "rm -rf /home/mr_robot/rom_nav2_ws/*")
                            }
                        }
                        Text {
                            anchors.centerIn: parent
                            text: "delete_workspace for rom_nav2_ws"
                            color: fb3.hovered ? "#ffffff" : "#000000"
                            font.pixelSize: 13
                        }
                    }

                    Rectangle {
                        id: fb4
                        height: 44
                        Layout.fillWidth: true
                        radius: 12
                        property bool hovered: false
                        gradient: Gradient {
                            GradientStop { position: 0.0; color: fb4.hovered ? "#66bb6a" : "#a5d6a7" }
                            GradientStop { position: 1.0; color: fb4.hovered ? "#1b5e20" : "#2e7d32" }
                        }
                        border.width: 0
                        MouseArea {
                            anchors.fill: parent
                            hoverEnabled: true
                            onEntered: parent.hovered = true
                            onExited: parent.hovered = false
                            onClicked: {
                                // Clear logs and run colcon build over SSH for rom_drivers_ws
                                rsyncRunner.clearLogs()
                                const remotePath = "/home/mr_robot/rom_drivers_ws/"
                                rsyncRunner.runRemoteCommand(ipEdit.text, passEdit.text, remotePath, "bash -lc 'source /opt/ros/humble/setup.bash && cd /home/mr_robot/rom_drivers_ws && /usr/bin/colcon build'")
                            }
                        }
                        Text {
                            anchors.centerIn: parent
                            text: "bb_save for rom_drivers_ws"
                            color: fb4.hovered ? "#ffffff" : "#000000"
                            font.pixelSize: 13
                        }
                    }

                    Rectangle {
                        id: fb5
                        height: 44
                        Layout.fillWidth: true
                        radius: 12
                        property bool hovered: false
                        gradient: Gradient {
                            GradientStop { position: 0.0; color: fb5.hovered ? "#66bb6a" : "#a5d6a7" }
                            GradientStop { position: 1.0; color: fb5.hovered ? "#1b5e20" : "#2e7d32" }
                        }
                        border.width: 0
                        MouseArea {
                            anchors.fill: parent
                            hoverEnabled: true
                            onEntered: parent.hovered = true
                            onExited: parent.hovered = false
                            onClicked: {
                                // Clear logs and run colcon build over SSH for rom_sdk_ws
                                rsyncRunner.clearLogs()
                                const remotePath = "/home/mr_robot/rom_sdk_ws/"
                                rsyncRunner.runRemoteCommand(ipEdit.text, passEdit.text, remotePath, "bash -lc 'source /opt/ros/humble/setup.bash && cd /home/mr_robot/rom_sdk_ws && /usr/bin/colcon build'")
                            }
                        }
                        Text {
                            anchors.centerIn: parent
                            text: "bb_save for rom_sdk_ws"
                            color: fb5.hovered ? "#ffffff" : "#000000"
                            font.pixelSize: 13
                        }
                    }

                    Rectangle {
                        id: fb6
                        height: 44
                        Layout.fillWidth: true
                        radius: 12
                        property bool hovered: false
                        gradient: Gradient {
                            GradientStop { position: 0.0; color: fb6.hovered ? "#66bb6a" : "#a5d6a7" }
                            GradientStop { position: 1.0; color: fb6.hovered ? "#1b5e20" : "#2e7d32" }
                        }
                        border.width: 0
                        MouseArea {
                            anchors.fill: parent
                            hoverEnabled: true
                            onEntered: parent.hovered = true
                            onExited: parent.hovered = false
                            onClicked: {
                                // Clear logs and run colcon build over SSH for rom_nav2_ws
                                rsyncRunner.clearLogs()
                                const remotePath = "/home/mr_robot/rom_nav2_ws/"
                                rsyncRunner.runRemoteCommand(ipEdit.text, passEdit.text, remotePath, "bash -lc 'source /opt/ros/humble/setup.bash && cd /home/mr_robot/rom_nav2_ws && /usr/bin/colcon build'")
                            }
                        }
                        Text {
                            anchors.centerIn: parent
                            text: "bb_save for rom_nav2_ws"
                            color: fb6.hovered ? "#ffffff" : "#000000"
                            font.pixelSize: 13
                        }
                    }
                }

                    // Right spacer to pad the button grid
                Item { Layout.preferredWidth: 12 }
                }

                // Extra row for reboot/shutdown with some spacing
                RowLayout {
                    Layout.fillWidth: true
                    spacing: 12
                    anchors.margins: 6

                    Item { Layout.preferredWidth: 12 }

                    Rectangle {
                        id: rebootBtn
                        height: 40
                        Layout.fillWidth: true
                        radius: 10
                        property bool hovered: false
                        gradient: Gradient {
                            GradientStop { position: 0.0; color: rebootBtn.hovered ? "#ff7043" : "#ff8a65" }
                            GradientStop { position: 1.0; color: rebootBtn.hovered ? "#e9f502" : "#a2ab00" }
                        }
                        border.width: 0
                        MouseArea {
                            anchors.fill: parent
                            hoverEnabled: true
                            onEntered: parent.hovered = true
                            onExited: parent.hovered = false
                            onClicked: {
                                // Open confirmation dialog for reboot
                                confirmActionDialog.confirmHost = ipEdit.text
                                confirmActionDialog.confirmPassword = passEdit.text
                                confirmActionDialog.confirmCommand = "sudo systemctl reboot"
                                confirmActionDialog.confirmMessage = "Reboot remote robot at " + ipEdit.text + "?"
                                confirmActionDialog.open()
                            }
                        }
                        Text { anchors.centerIn: parent; text: "R E B O O T   R O B O T"; color: rebootBtn.hovered ? "#ffffff" : "#000000"; font.pixelSize: 13 }
                    }

                    Rectangle {
                        id: shutdownBtn
                        height: 40
                        Layout.fillWidth: true
                        radius: 10
                        property bool hovered: false
                        gradient: Gradient {
                            GradientStop { position: 0.0; color: shutdownBtn.hovered ? "#ff7043" : "#ff8a65" }
                            GradientStop { position: 1.0; color: shutdownBtn.hovered ? "#fa050d" : "#8a0106" }
                        }
                        border.width: 0
                        MouseArea {
                            anchors.fill: parent
                            hoverEnabled: true
                            onEntered: parent.hovered = true
                            onExited: parent.hovered = false
                            onClicked: {
                                // Open confirmation dialog for shutdown
                                confirmActionDialog.confirmHost = ipEdit.text
                                confirmActionDialog.confirmPassword = passEdit.text
                                confirmActionDialog.confirmCommand = "sudo systemctl poweroff"
                                confirmActionDialog.confirmMessage = "Shutdown remote robot at " + ipEdit.text + "?"
                                confirmActionDialog.open()
                            }
                        }
                        Text { anchors.centerIn: parent; text: "S H U T D O W N   R O B O T"; color: shutdownBtn.hovered ? "#ffffff" : "#000000"; font.pixelSize: 13 }
                    }

                    Item { Layout.preferredWidth: 12 }
            }
            
        }
    }

    // Draggable strip along the top to move the frameless window
    MouseArea {
        id: dragStrip
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: 22
        hoverEnabled: false
        acceptedButtons: Qt.LeftButton
        // Keep it below the close button so clicks on the button are not intercepted
        z: closeBtn.z - 1
        onPressed: (mouse) => {
            // Start moving the window when user drags on the empty strip
            root.startSystemMove(mouse)
        }
    }

    // Confirmation dialog for reboot/shutdown actions
    Dialog {
        id: confirmActionDialog
        modal: true
        standardButtons: Dialog.Ok | Dialog.Cancel
        property string confirmHost: ""
        property string confirmPassword: ""
        property string confirmCommand: ""
        property string confirmMessage: "Are you sure?"

        title: qsTr("Confirm remote action")

        // Wrap the Column in a fixed-size Item to prevent Dialog from trying to
        // use contentItem.implicitWidth (which can create binding loops).
        contentItem: Item {
            width: 520
            Column {
                anchors.fill: parent
                anchors.margins: 8
                spacing: 8
                Text { text: confirmMessage; wrapMode: Text.Wrap; color: textColor }
                RowLayout { spacing: 6
                    Text { text: "Host:"; color: textColor; font.bold: true }
                    Text { text: confirmHost; color: textColor }
                }
                RowLayout { spacing: 6
                    Text { text: "Command:"; color: textColor; font.bold: true }
                    Text { text: confirmCommand; color: textColor; elide: Text.ElideLeft }
                }
            }
        }

        onAccepted: {
            rsyncRunner.clearLogs()
            rsyncRunner.runRemoteCommand(confirmHost, confirmPassword, "", confirmCommand)
            close()
        }

        onRejected: {
            close()
        }
    }

    

    // Theme colors
    property color cardBg: "#1e1e2f"
    property color textColor: "#e8e8ff"
    property color accent: "#7c4dff" // purple
    property color accent2: "#03dac6" // teal accent

    // Source folder is provided by C++ (RsyncRunner.sourceRoot)

    // Persist selection across delegate recycling
    property var selectedSet: ({})

    ColumnLayout {
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.margins: 20
        // anchor bottom to footer so content sits above buttons
        anchors.bottom: actionFooter.top
        spacing: 12

        // Header fields: IP + password + run button
        RowLayout {
            Layout.fillWidth: true
            spacing: 10

            TextField {
                id: ipEdit
                Layout.preferredWidth: 250
                height: 44
                font.pixelSize: 14
                focus: true
                placeholderText: "IP Address (e.g. 192.168.1.1)"
                text: "192.168.1.1"
                color: textColor
                background: Rectangle {
                    anchors.fill: parent
                    radius: 10
                    color: cardBg
                    border.color: ipEdit.focus ? "#ff5252" : "#5e5e8a"
                    border.width: ipEdit.focus ? 1 : 1
                }
            }

            TextField {
                id: passEdit
                Layout.preferredWidth: 250
                height: 44
                font.pixelSize: 14
                placeholderText: "Password"
                echoMode: TextInput.Password
                color: textColor
                background: Rectangle {
                    anchors.fill: parent
                    radius: 10
                    color: cardBg
                    border.color: passEdit.focus ? "#ff5252" : "#5e5e8a"
                    border.width: passEdit.focus ? 1 : 1
                }
            }

            Button {
                id: runBtn
                text: "rsync copy"
                Layout.preferredWidth: 250
                height: 44
                font.pixelSize: 14
                font.bold: true
                hoverEnabled: true
                background: Rectangle {
                    anchors.fill: parent
                    radius: 12
                    gradient: Gradient {
                        GradientStop { position: 0.0; color: "#7c4dff" }
                        GradientStop { position: 1.0; color: "#3d2db7" }
                    }
                    border.color: runBtn.focus ? "#ff5252" : "transparent"
                    border.width: runBtn.focus ? 1 : 0
                }
                contentItem: Label { text: runBtn.text; color: runBtn.hovered ? "#ffffff" : "#000000"; anchors.centerIn: parent; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter }
                onClicked: {
                    // Collect checked items and unchecked names for excludes
                    const selected = []
                    const excludes = []
                    for (let i = 0; i < folderModel.count; ++i) {
                        const name = folderModel.get(i, "fileName")
                        const sep = rsyncRunner.sourceRoot.endsWith('/') ? '' : '/'
                        const path = rsyncRunner.sourceRoot + sep + name
                        if (root.selectedSet[path]) {
                            selected.push(path)
                        } else {
                            // send the name as an exclude pattern (relative name)
                            excludes.push(name)
                        }
                    }
                    rsyncRunner.clearLogs()
                    rsyncRunner.run(ipEdit.text, passEdit.text, selected, excludes)
                }
            }
            // Spacer to push header items into a nice centered arrangement
            Item { Layout.fillWidth: true }
        }

        Component.onCompleted: {
            // Ensure the ipEdit has keyboard focus when the app starts
            ipEdit.forceActiveFocus()
        }

        // Content: file list + logs stacked vertically
        SplitView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            orientation: Qt.Vertical

            Rectangle {
                // files / directories area: reduced height
                SplitView.preferredHeight: 220
                color: cardBg      // dark background
                radius: 10
                border.color: "#2a2a45"

                // File list model (lists both files and directories)
                FolderListModel {
                    id: folderModel
                    folder: "file:///" + (rsyncRunner.sourceRoot.startsWith('/') ? rsyncRunner.sourceRoot.slice(1) : rsyncRunner.sourceRoot)
                    showDotAndDotDot: false
                    nameFilters: ["*"]
                    // When the model populates, pre-check common workspace folders if they exist
                    onCountChanged: {
                        var defaults = ["rom_maintain_ws", "rom_nav2_ws", "rom_sdk_ws", "rom_drivers_ws", "mytmux", "data", "thirdparty_drivers_ws"]
                        for (var i = 0; i < folderModel.count; ++i) {
                            var name = folderModel.get(i, "fileName")
                            if (defaults.indexOf(name) !== -1) {
                                var sep = rsyncRunner.sourceRoot.endsWith('/') ? '' : '/'
                                var path = rsyncRunner.sourceRoot + sep + name
                                root.selectedSet[path] = true
                            }
                        }
                    }
                }

                // File grid box with heading
                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 8
                    spacing: 6

                    // Heading label
                    Label {
                        text: "Development  PC  မှ  Robot  PC   သို့  အောက်ပါ ဖိုင်များကို ကူးယူမည် ။  ကူးယူမည့်  files  /  directories  များကို  အမှန်ခြစ်ပါ ။"
                        color: textColor
                        font.bold: true
                    }

                    // File grid view (horizontal alignment, wraps to next lines)
                    GridView {
                        id: grid
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        model: folderModel
                        clip: true
                        focus: false

                        // Tile sizing: exactly 4 columns based on available width
                        cellWidth: Math.floor(width / 4)
                        cellHeight: 44

                        delegate: Rectangle {
                            width: grid.cellWidth
                            height: grid.cellHeight
                            radius: 0
                            color: "transparent"   // no background
                            border.color: "transparent" // no border
                            anchors.margins: 0

                            // Use local absolute path as the key (not file URL)
                            property string fullPath: {
                                const sep = rsyncRunner.sourceRoot.endsWith('/') ? '' : '/'
                                return rsyncRunner.sourceRoot + sep + fileName
                            }

                            RowLayout {
                                anchors.fill: parent
                                anchors.margins: 0
                                spacing: 6

                                CheckBox {
                                    id: cb
                                    padding: 0
                                    leftPadding: 0
                                    rightPadding: 0
                                    topPadding: 0
                                    bottomPadding: 0
                                    checked: Boolean(root.selectedSet[fullPath])
                                    onToggled: root.selectedSet[fullPath] = checked
                                }

                                Label {
                                    text: fileName
                                    color: textColor
                                    padding: 0
                                    leftPadding: 0
                                    rightPadding: 0
                                    elide: Text.ElideRight
                                    Layout.fillWidth: true
                                }
                            }
                        }
                    }
                }
            }

            // Logs area (taller, above command prompt)
            Rectangle {
                color: "#00000020"
                SplitView.preferredHeight: 140
                radius: 8

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 8
                    spacing: 6

                    // Status label (shows success/error)
                    Label {
                        id: statusLabel
                        text: rsyncRunner.status
                        color: rsyncRunner.statusColor === "green" ? "#00c853" : (rsyncRunner.statusColor === "red" ? "#ff5252" : textColor)
                        visible: rsyncRunner.status !== undefined && rsyncRunner.status.length > 0
                        font.bold: true
                    }

                    TextArea {
                        id: logBox
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        readOnly: true
                        wrapMode: Text.Wrap
                        text: rsyncRunner.logs
                        color: textColor
                        background: Rectangle { color: "#0d0d1a"; radius: 8; border.color: "#333355" }
                    }
                }
            }

            // command prompt area removed per user request
        }
    }
}

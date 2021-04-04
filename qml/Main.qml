import QtQuick 2.7
import Ubuntu.Components 1.3
import Ubuntu.Components.Popups 1.3
import QtQuick.Layouts 1.3
import Qt.labs.settings 1.0

import Example 1.0

MainView {
    id: root
    objectName: 'mainView'
    applicationName: 'nfcp2pdemo.me.fredl.nfcp2pdemo'
    automaticOrientation: true

    width: units.gu(45)
    height: units.gu(75)

    property string receivedText : ""

    Page {
        anchors.fill: parent

        header: PageHeader {
            id: header
            title: i18n.tr('NFC P2P Demo')
        }

        ColumnLayout {
            spacing: units.gu(2)
            anchors {
                margins: units.gu(2)
                top: header.bottom
                left: parent.left
                right: parent.right
                bottom: parent.bottom
            }

            Item {
                Layout.fillHeight: true
            }

            Label {
                id: label
                Layout.alignment: Qt.AlignHCenter
                text: i18n.tr('Press the buttons below to play around with NFC!')
            }

            TextField {
                id: textField
                text: "This is a test!"
            }

            Button {
                Layout.alignment: Qt.AlignHCenter
                text: i18n.tr('Send text!')
                onClicked: Service.startService(textField.text)
            }

            Button {
                Layout.alignment: Qt.AlignHCenter
                text: i18n.tr('Receive text!')
                onClicked: Reader.listenService()
            }

            Item {
                Layout.fillHeight: true
            }
        }

        Connections {
            target: Reader
            onBytesReceived: {
                receivedText = bytes
                PopupUtils.open(dialog)
            }
        }

        Component {
            id: dialog
            Dialog {
                id: innerDialog
                title: qsTr("Message received")
                text: receivedText
                Button {
                    text: "Close"
                    onClicked: PopupUtils.close(innerDialog)
                }
            }
        }
    }
}

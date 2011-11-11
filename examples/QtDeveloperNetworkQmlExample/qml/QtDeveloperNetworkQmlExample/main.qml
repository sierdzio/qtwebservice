import QtQuick 1.1
import QtWebService 1.0
import QtQuick 1.0

Rectangle {
    width: 360
    height: 360
    color: "#b6b9b9"

    WebMethod {
        id: webMethod
        host: "http://developer.qt.nokia.com/qtapi/1/member/profile"
        httpMethod: "Get"
        protocol: "Json"

        Component.onCompleted: replyReady.connect(readMtdReply);
    }

    Text {
        id: result
        text: webMethod.hostUrl
        anchors.fill: parent
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            Qt.quit();
        }

        Text {
            id: textLogin
            x: 19
            y: 32
            text: qsTr("Login")
            font.pixelSize: 18
        }

        Text {
            id: textPassword
            x: 161
            y: 32
            text: qsTr("Password")
            font.pixelSize: 18
        }

        Rectangle {
            id: button
            x: 19
            y: 86
            width: 119
            height: 28
            color: "#867777"

            Text {
                id: textButton
                x: 7
                y: 7
                text: qsTr("Log me in!")
                font.bold: true
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                font.pixelSize: 22
            }

            MouseArea {
                id: mouseAreaButton
                anchors.fill: parent
                onClicked: {
                    webMethod.authenticate(textInputLogin.text, textInputPassword.text);
                    console.log("Invoking method");
                    webMethod.invokeMethod();
                    console.log("Finished invoking method");
                }
            }
        }

        Rectangle {
            x: 19
            y: 53
            width: 142
            height: 20
            border.width: 1

            TextInput {
                id: textInputLogin
                echoMode: TextInput.Normal
                font.pixelSize: 18
                anchors.fill: parent
            }
        }

        Rectangle {
            x: 162
            y: 53
            width: 150
            height: 20
            border.width: 1

            TextInput {
                id: textInputPassword
                echoMode: TextInput.Password
                font.pixelSize: 18
                anchors.fill: parent
            }
        }

        Text {
            id: textResult
            x: 19
            y: 135
            width: 322
            height: 203
            text: qsTr("Result " + webMethod.host + " " + webMethod.protocol + " " + webMethod.httpMethod)
            font.pixelSize: 14
        }
    }

    function readMtdReply() {
        if (webMethod.isReplyReady()) {
            var reply = webMethod.replyRead();
            console.log("Reading the reply: " + reply);
            textResult.text = reply;
        } else {
            console.log("Reaply is not available");
        }
    }
}

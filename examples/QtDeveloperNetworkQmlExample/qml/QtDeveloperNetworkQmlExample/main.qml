import QtQuick 1.1
import QtWebService 1.0

Rectangle {
    width: 360
    height: 360

    WebMethod {
        id: webMethod
        host: "http://developer.qt.nokia.com/qtapi/1/member/profile"
        httpMethod: "Get"
        protocol: "Json"

//        onReplyReady: {
//            readMtdReply("a");
//        }

        Component.onCompleted: {
            webMethod.authenticate("login", "password");
            webMethod.replyReady.connect(readMtdReply);
            console.log("invoking method");
            webMethod.invokeMethod();
        }
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
    }

    function readMtdReply(whatever) {
        var reply = webMethod.replyRead();
        console.log("reading the reply: " + reply);
        result.text = reply;
    }
}

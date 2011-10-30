import QtQuick 1.1
import QtWebService 1.0

Rectangle {
    width: 360
    height: 360

    WebMethod {
        id: webMethod
        host: "http://developer.qt.nokia.com/qtapi/1/member/profile"

//        Component.onCompleted: {
//            webMethod.setHttpMethod("Get");
//            webMethod.setProtocol("Json");
//        }
    }

    Text {
        text: webMethod.hostUrl
        anchors.centerIn: parent
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            Qt.quit();
        }
    }
}

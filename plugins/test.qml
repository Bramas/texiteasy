import QtQuick 2.0

GridView {
    width: 300; height: 200
    keyNavigationWraps: false
    flickableDirection: Flickable.HorizontalFlick
    opacity: 1
    clip: false

    model: ListModel {

	    ListElement {
	        name: "Jim Williams"
	    }
	    ListElement {
	        name: "John Brown"
	    }
	    ListElement {
	        name: "Bill Smyth"
	    }
	    ListElement {
	        name: "Sam Wise"
	    }
	}
    Component {
        id: contactsDelegate
            
	    Rectangle {
	        id: wrapper
	        MouseArea {
         		anchors.fill: parent
	            onClicked: grid.currentIndex = index
			}
            width: 80
        	height: 80
        	//color: GridView.isCurrentItem ? "grey" : "white"
            TextInput {
                id: contactInfo
                text: name
                selectByMouse: true
                
	        }
	    }
    }

    id: grid
    //anchors.fill: parent
    cellWidth: 80; cellHeight: 80

    //model: ContactModel {}
    delegate: contactsDelegate
    highlight: Rectangle { color: "lightsteelblue"; radius: 5 }
    focus: true
    }

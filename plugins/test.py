from PyQt5 import QtWidgets
from PyQt5 import QtGui

class TexitEasyHelper(QtWidgets.QDialog):
    name = ""
    textCursor = QtGui.QTextCursor()
    def __init__(self):  
        super(TexitEasyHelper, self).__init__()
    def setTextCursor(self, textCursor):
        self.textCursor = textCursor
        self.setWindowTitle("OK")




class TabularHelper(TexitEasyHelper):
    name = "Inserer un tableau"

    def __init__(self):     
        super(TabularHelper, self).__init__()
        self.setWindowTitle(self.name)

        self.table = QtWidgets.QTableWidget()
        self.submitButton = QtWidgets.QPushButton("Submit")



        self.table.setRowCount(2);
        self.table.setColumnCount(5);



        vLayout = QtWidgets.QVBoxLayout()
        self.setLayout(vLayout)

        vLayout.addWidget(self.table)
        vLayout.addWidget(self.submitButton)
 
        self.submitButton.clicked.connect(self.submit)

    def submit(self):
        col = []
        for i in range(self.table.rowCount()):
            row = []
            for j in range(self.table.columnCount()):
                if self.table.item(i, j):
                    row.append(self.table.item(i, j).text())
                else:
                    row.append(' ')
            col.append('&'.join(row))

        self.textCursor.insertText('\\\\\n'.join(col))
        self.accept()


pluginClassName = "TabularHelper"


def launch():
    app = QtWidgets.QApplication([])
    a = TabularHelper()
    a.show()
    app.exec_()


#launch()
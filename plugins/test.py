from PyQt5 import QtWidgets

class TabularHelper(QtWidgets.QDialog):

    name = "Inserer un tableau"
    t = QtWidgets.QDialog()

    def __init__(self):     
        super(TabularHelper, self).__init__()
        self.setWindowTitle(self.name)



pluginClassName = "TabularHelper"
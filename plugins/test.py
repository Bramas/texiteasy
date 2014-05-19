from PyQt5 import QtWidgets
from PyQt5.QtCore import *
from PyQt5.QtWidgets import (QAction, QApplication, QFileDialog, QMainWindow,
        QMessageBox, QTextEdit)
import sys

#app = QApplication(sys.argv)

class TabularHelper(QtWidgets.QDialog):

    name = "Inserer un tableau"

    def __init__(self):     
        super(TabularHelper, self).__init__()
        self.setWindowTitle(self.name)


pluginClassName = "TabularHelper"
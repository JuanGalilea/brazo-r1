from PyQt5 import QtCore, QtGui, QtWidgets
from PyQt5.QtWidgets import QDialog, QWidget, QLabel



class SampleRoutines(QWidget):
    closing_signal  = QtCore.pyqtSignal()

    def __init__(self):
        super().__init__() 
        self.setupUi(self)

    def setupUi(self, Form):        
        Form.setGeometry(300, 100, 1050, 875)
        Form.setMinimumSize(QtCore.QSize(1050, 875))
        Form.setMaximumSize(QtCore.QSize(1050, 875))

        self.page = QLabel(Form)
        self.page.setGeometry(QtCore.QRect(0, 0, 1050, 875))
        self.page.setStyleSheet("background-image: url(p4.png);")

        self.retranslateUi(Form)
        QtCore.QMetaObject.connectSlotsByName(Form)

    def retranslateUi(self, Form):
        self._translate = QtCore.QCoreApplication.translate
        Form.setWindowTitle(self._translate("ARM R1", "ARM R1"))
    
    def mousePressEvent(self, event):
        super(MoveByJoint, self).mousePressEvent(event)
        x, y = event.x(), event.y()

        back = 869 < x and x < 1012 and 814 < y and y < 843

        if back:                        # Volver al menu principal
                self.hide()
                self.closing_signal.emit()
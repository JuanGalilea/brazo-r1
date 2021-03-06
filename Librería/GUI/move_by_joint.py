
from PyQt5 import QtCore, QtGui, QtWidgets
from PyQt5.QtWidgets import QDialog, QWidget, QLabel


class MoveByJoint(QWidget):
    closing_signal = QtCore.pyqtSignal()

    def __init__(self):
        super().__init__()  
        self.elbow_range_value          = (-654654, 3213213)    # Setear con referencias correctas
        self.shoulder_range_value       = (-654654, 3213213)
        self.hip_range_value            = (-654654, 3213213)

        self.elbow_pos_value    = 0                             # Setear con valor actual
        self.shoulder_pos_value = 0
        self.hip_pos_value      = 0

        self.increment = 5                                      # Fijar valor real

        self.setupUi(self)
        
    def setupUi(self, Form):
        Form.setGeometry(300, 100, 1050, 875)
        Form.setMinimumSize(QtCore.QSize(1050, 875))
        Form.setMaximumSize(QtCore.QSize(1050, 875))

        self.page = QLabel(Form)
        self.page.setGeometry(QtCore.QRect(0, 0, 1050, 875))
        self.page.setStyleSheet("background-image: url(images/p3.png);")

        self.elbowRange = QtWidgets.QTextBrowser(Form)
        self.elbowRange.setGeometry(QtCore.QRect(30, 390, 381, 21))
        self.elbowRange.setStyleSheet("border-image: url(images/dark_gray.png);")
        self.elbowRange.setVerticalScrollBarPolicy(QtCore.Qt.ScrollBarAlwaysOff)
        self.elbowRange.setHorizontalScrollBarPolicy(QtCore.Qt.ScrollBarAlwaysOff)

        self.shoulderRange = QtWidgets.QTextBrowser(Form)
        self.shoulderRange.setGeometry(QtCore.QRect(30, 570, 381, 21))
        self.shoulderRange.setStyleSheet("border-image: url(images/dark_gray.png);")
        self.shoulderRange.setVerticalScrollBarPolicy(QtCore.Qt.ScrollBarAlwaysOff)
        self.shoulderRange.setHorizontalScrollBarPolicy(QtCore.Qt.ScrollBarAlwaysOff)

        self.hipRange = QtWidgets.QTextBrowser(Form)
        self.hipRange.setGeometry(QtCore.QRect(30, 740, 381, 21))
        self.hipRange.setStyleSheet("border-image: url(images/dark_gray.png);")
        self.hipRange.setVerticalScrollBarPolicy(QtCore.Qt.ScrollBarAlwaysOff)
        self.hipRange.setHorizontalScrollBarPolicy(QtCore.Qt.ScrollBarAlwaysOff)

        self.elbowPos = QtWidgets.QTextBrowser(Form)
        self.elbowPos.setGeometry(QtCore.QRect(340, 360, 181, 21))
        self.elbowPos.setStyleSheet("border-image: url(images/yellow.png);")
        self.elbowPos.setVerticalScrollBarPolicy(QtCore.Qt.ScrollBarAlwaysOff)
        self.elbowPos.setHorizontalScrollBarPolicy(QtCore.Qt.ScrollBarAlwaysOff)

        self.shoulderPos = QtWidgets.QTextBrowser(Form)
        self.shoulderPos.setGeometry(QtCore.QRect(340, 540, 181, 21))
        self.shoulderPos.setStyleSheet("border-image: url(images/yellow.png);")
        self.shoulderPos.setVerticalScrollBarPolicy(QtCore.Qt.ScrollBarAlwaysOff)
        self.shoulderPos.setHorizontalScrollBarPolicy(QtCore.Qt.ScrollBarAlwaysOff)

        self.hipPos = QtWidgets.QTextBrowser(Form)
        self.hipPos.setGeometry(QtCore.QRect(340, 710, 181, 21))
        self.hipPos.setStyleSheet("border-image: url(images/yellow.png);")
        self.hipPos.setVerticalScrollBarPolicy(QtCore.Qt.ScrollBarAlwaysOff)
        self.hipPos.setHorizontalScrollBarPolicy(QtCore.Qt.ScrollBarAlwaysOff)

        self.retranslateUi(Form)
        QtCore.QMetaObject.connectSlotsByName(Form)

    def retranslateUi(self, Form):
        self._translate = QtCore.QCoreApplication.translate
        Form.setWindowTitle(self._translate("ARM R1", "ARM R1"))

        self.set_range_text(self.elbowRange,    self.elbow_range_value)
        self.set_range_text(self.shoulderRange, self.shoulder_range_value)
        self.set_range_text(self.hipRange,      self.hip_range_value)

        self.set_text(self.elbowPos,    self.elbow_pos_value)
        self.set_text(self.shoulderPos, self.shoulder_pos_value)
        self.set_text(self.hipPos,      self.hip_pos_value)

    def set_text(self, text_edit, text):        # Acualizar posicion de articulacion
        text_edit.setHtml(self._translate("Form", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
        "<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
        "p, li { white-space: pre-wrap; }\n"
        "</style></head><body style=\" font-family:\'Cantarell\'; font-size:11pt; font-weight:400; font-style:normal;\">\n"
        f"<p align=\"right\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-family:\'Ubuntu Mono\'; font-size:14pt; font-weight:600; color:#363633;\">pos: {text}</span></p></body></html>"))
    
    def set_range_text(self, text_edit, value): # Setear rango de articulacion
        min_ref, max_ref = value
        text_edit.setHtml(self._translate("Form", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
        "<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
        "p, li { white-space: pre-wrap; }\n"
        "</style></head><body style=\" font-family:\'Ubuntu Mono\'; font-size:14pt; font-weight:600; font-style:normal; color:#a6a6a6;\">\n"
        f"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:12pt; color:#a6a6a6;\">movement range: {min_ref} to {max_ref}</span></p></body></html>"))

    def mouseReleaseEvent(self, event):
        print("released")

    def mousePressEvent(self, event):
        super(MoveByJoint, self).mousePressEvent(event)
        x, y = event.x(), event.y()

        x_lbtn_all      = 622 < x and x < 767
        x_rbtn_all      = 803 < x and x < 950
        y_btn_elbow     = 295 < y and y < 384
        y_btn_shoulder  = 472 < y and y < 563
        y_btn_hip       = 644 < y and y < 734

        back = 869 < x and x < 1012 and 814 < y and y < 843
        
        if x_rbtn_all:                  # Botones de la derecha
                if y_btn_elbow:
                        self.elbow_pos_value += self.increment
                        self.set_text(self.elbowPos,    str(self.elbow_pos_value))

                elif y_btn_shoulder:
                        self.shoulder_pos_value += self.increment
                        self.set_text(self.shoulderPos, str(self.shoulder_pos_value))

                elif y_btn_hip:
                        self.hip_pos_value += self.increment
                        self.set_text(self.hipPos,      str(self.hip_pos_value))

        elif x_lbtn_all:               # Botones de la izquierda
                if y_btn_elbow:
                        self.elbow_pos_value -= self.increment
                        self.set_text(self.elbowPos,    str(self.elbow_pos_value))

                elif y_btn_shoulder:
                        self.shoulder_pos_value -= self.increment
                        self.set_text(self.shoulderPos, str(self.shoulder_pos_value))

                elif y_btn_hip:
                        self.hip_pos_value -= self.increment
                        self.set_text(self.hipPos,      str(self.hip_pos_value))      

        if back:                        # Volver al menu principal
                self.hide()
                self.closing_signal.emit()

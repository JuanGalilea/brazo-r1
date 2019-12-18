from PyQt5 import QtCore, QtGui, QtWidgets
from PyQt5.QtWidgets import QDialog, QWidget, QLabel


class ToCoordinates(QWidget):
    closing_signal  = QtCore.pyqtSignal()   
    send_value      = QtCore.pyqtSignal()   

    def __init__(self):
        super().__init__()  
        self.setupUi(self)

    def setupUi(self, Form):
        Form.setMinimumSize(QtCore.QSize(1050, 875))
        Form.setMaximumSize(QtCore.QSize(1050, 875))
        Form.setGeometry(300, 100, 1050, 875)

        self.page = QLabel(Form)
        self.page.setGeometry(QtCore.QRect(0, 0, 1050, 875))
        self.page.setStyleSheet("background-image: url(p2.png);")

        self.statusText = QtWidgets.QTextBrowser(Form)
        self.statusText.setGeometry(QtCore.QRect(80, 830, 600, 21))
        self.statusText.setStyleSheet("border-image: url(dark_gray.png);")
        self.statusText.setVerticalScrollBarPolicy(QtCore.Qt.ScrollBarAlwaysOff)
        self.statusText.setHorizontalScrollBarPolicy(QtCore.Qt.ScrollBarAlwaysOff)

        self.xCartesian = QtWidgets.QTextEdit(Form)
        self.xCartesian.setGeometry(QtCore.QRect(150, 430, 271, 51))
        self.xCartesian.setStyleSheet("border-image: url(light_gray.png);\n" "font: 1 20pt \"Cantarell\" rgb(21, 21, 20);")

        self.yCartesian = QtWidgets.QTextEdit(Form)
        self.yCartesian.setGeometry(QtCore.QRect(150, 550, 271, 51))
        self.yCartesian.setStyleSheet("border-image: url(light_gray.png);\n" "font: 1 20pt \"Cantarell\" rgb(21, 21, 20);")

        self.zCartesian = QtWidgets.QTextEdit(Form)
        self.zCartesian.setGeometry(QtCore.QRect(150, 660, 271, 51))
        self.zCartesian.setStyleSheet("border-image: url(light_gray.png);\n" "font: 1 20pt \"Cantarell\" rgb(21, 21, 20);")
        
        self.rCilindric = QtWidgets.QTextEdit(Form)
        self.rCilindric.setGeometry(QtCore.QRect(680, 430, 271, 51))
        self.rCilindric.setStyleSheet("border-image: url(light_gray.png);\n" "font: 1 20pt \"Cantarell\" rgb(21, 21, 20);")

        self.thetaCilindric = QtWidgets.QTextEdit(Form)
        self.thetaCilindric.setGeometry(QtCore.QRect(680, 550, 271, 51))
        self.thetaCilindric.setStyleSheet("border-image: url(light_gray.png);\n""font: 1 20pt \"Cantarell\" rgb(21, 21, 20);")
        
        self.zCilindric = QtWidgets.QTextEdit(Form)
        self.zCilindric.setGeometry(QtCore.QRect(680, 660, 271, 51))
        self.zCilindric.setStyleSheet("border-image: url(light_gray.png);\n""font: 1 20pt \"Cantarell\" rgb(21, 21, 20);")
        self.zCilindric.setObjectName("zCilindric")

        self.retranslateUi(Form)
        QtCore.QMetaObject.connectSlotsByName(Form)

    def retranslateUi(self, Form):
        self._translate = QtCore.QCoreApplication.translate
        Form.setWindowTitle(self._translate("ARM R1", "ARM R1"))

    def set_text(self, text_edit, text):
        text_edit.setHtml(self._translate("Form", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
        "<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
        "p, li { white-space: pre-wrap; }\n"
        "</style></head><body style=\" font-family:\'Ubuntu Mono\'; font-size:14pt; font-weight:600; font-style:normal; color:#a6a6a6;\">\n"
        f"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">{text}<span style=\" font-size:14pt; color:#a6a6a6;\"></span></p></body></html>"))
    
    def move_cartesian(self):
        x = self.xCartesian.toPlainText()
        y = self.yCartesian.toPlainText()
        z = self.zCartesian.toPlainText()

        if x.isdigit() and y.isdigit() and z.isdigit():
                text = f"moving arm to x: {x} y: {y} z: {z}, please wait..."
                self.set_text(self.statusText, text)
                return(int(x), int(y), int(z))

        self.set_text(self.statusText, "incorrect input")

    def move_cilindric(self):
        r       = self.rCilindric.toPlainText()
        theta   = self.thetaCilindric.toPlainText()
        z       = self.zCilindric.toPlainText()

        if r.isdigit() and theta.isdigit() and z.isdigit():
                text = f"moving arm to r: {r} theta: {theta} z: {z}, please wait..."
                self.set_text(self.statusText, text)
                return(int(r), int(theta), int(z))

        self.set_text(self.statusText, "incorrect input")

    def mousePressEvent(self, event):
        super(ToCoordinates, self).mousePressEvent(event)
        x, y = event.x(), event.y()
        
        y_move      = 756 < y and y < 779
        x_move_cart = 205 < x and x < 323
        x_move_cil  = 729 < x and x < 846

        back = 869 < x and x < 1012 and 814 < y and y < 843

        if y_move:
                if x_move_cart:
                        self.move_cartesian()
                elif x_move_cil:
                        self.move_cilindric()
        
        if back:
                self.hide()
                self.closing_signal.emit()
                



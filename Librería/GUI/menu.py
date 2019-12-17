from PyQt5              import QtCore, QtGui, QtWidgets
from PyQt5.QtWidgets    import QDialog, QWidget, QApplication, QLabel, QMainWindow
from PyQt5.QtCore       import pyqtSignal, QObject

from GUI.move_by_joint      import *
from GUI.sample_routines    import *
from GUI.to_coordinates     import * 
#from ARMR1.__init__         import *
from GUI.images            import *

class Menu(QMainWindow):
    def __init__(self):
        super().__init__()   
        self.init_arduino()
        self.onMenu         = True

        self.onGetToCoords = False
        self.GetToCoordsWindow = ToCoordinates()
        self.GetToCoordsWindow.closing_signal.connect(self.close_window)

        self.onMoveByJoint = False
        self.MoveByJointWindow = MoveByJoint()
        self.MoveByJointWindow.closing_signal.connect(self.close_window)

        self.onSampleRoutines = False
        self.SampleRoutinesWindow = SampleRoutines()
        self.SampleRoutinesWindow.closing_signal.connect(self.close_window)
    
    def init_arduino(self):
        self.initUI()
        #self.ARMR1 = ARMR1()
        self.setStyleSheet("background-image: url(p1.png);")


    def initUI(self):      
        self.setMinimumSize(QtCore.QSize(1050, 875))
        self.setMaximumSize(QtCore.QSize(1050, 875))
        self.setGeometry(300, 100, 1050, 875)
        self.setStyleSheet("background-image: url(GUI/images/p0.png);")
        self.setWindowTitle('ARM R1')
        self.show()
    
    def close_window(self):
        actual = self.actual_window()

        if actual   == "onMenu":
            self.close()

        elif actual == "onGetToCoords":
            self.onGetToCoords = False
            self.GetToCoordsWindow.hide()

        elif actual == "onMoveByJoint":
            self.onMoveByJoint = False
            self.MoveByJointWindow.hide()

        elif actual == "onSampleRoutines":
            self.onSampleRoutines = False
            self.onSampleRoutines.hide()
        
        self.onMenu = True                                  # Volver al Menu principal
        self.show()



    def button_press(self, window, x, y):
        if window == "onMenu":
            x_all_menu  = 54  < x and x < 546               # No hay botones del tipo QPushButton, 
            y_to_coords = 329 < y and y < 418               # sino "botones" según la posicion que se clickea
            y_by_joints = 467 < y and y < 556
            y_samples   = 608 < y and y < 697
            
            if x_all_menu:
                if y_to_coords:                             # Boton "Get to Coordinates"
                    self.onMenu = False
                    self.hide()
                    
                    self.onGetToCoords = True
                    self.GetToCoordsWindow.show()

                elif y_by_joints:                           # Boton "Move by Joint"
                    self.onMenu = False
                    self.hide()

                    self.onMoveByJoint = True
                    self.MoveByJointWindow.show()

                elif y_samples:                             # Boton "Sample Routines"
                    self.onMenu = False
                    self.hide()

                    self.onSampleRoutines = True
                    self.SampleRoutinesWindow.show()


    def actual_window(self):
        if self.onMenu:
            return("onMenu")
        elif self.onGetToCoords:
            return("onGetToCoords")
        elif self.onMoveByJoint:
            return("onMoveByJoint")
        elif self.onSampleRoutines:
            return("onSampleRoutines")

    def mousePressEvent(self, event):
        x, y = event.x(), event.y()
        self.button_press(self.actual_window(), x, y)

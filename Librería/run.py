from PyQt5.QtWidgets import QApplication

from GUI                     import images
from GUI.move_by_joint       import MoveByJoint
from GUI.to_coordinates      import ToCoordinates
from GUI.sample_routines     import SampleRoutines
from GUI.menu                import Menu

from ARMR1.__init__ import *


if __name__ == '__main__':
    import sys
    app = QApplication(sys.argv)
    ex = Menu()
    sys.exit(app.exec_())
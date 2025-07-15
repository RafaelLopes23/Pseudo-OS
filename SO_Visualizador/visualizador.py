import sys
from PyQt6.QtWidgets import QApplication, QWidget, QMainWindow

class SimuladorWindow(QMainWindow):
    def __init__(self):
        super().__init__()
        
        # Configurações da janela principal
        self.setWindowTitle('HELLO WORLD - Simulador de SO - JeffsDev')
        self.setGeometry(100, 100, 1280, 720) # Posição X, Posição Y, Largura, Altura

def main():
    app = QApplication(sys.argv)
    window = SimuladorWindow()
    window.show()
    sys.exit(app.exec())

if __name__ == '__main__':
    main()
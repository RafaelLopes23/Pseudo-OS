import sys
from PyQt6.QtWidgets import (
    QApplication, QWidget, QMainWindow, 
    QLabel, QFrame, QHBoxLayout, QVBoxLayout, QTextEdit
)
from PyQt6.QtCore import Qt
from PyQt6.QtGui import QFont

class SimuladorWindow(QMainWindow):
    def __init__(self):
        super().__init__()
        
        # Configurações da janela principal
        self.setWindowTitle('Visualizador do Pseudo-SO')
        self.setGeometry(100, 100, 1280, 720)
        
        # Widget central para conter todo o layout
        main_widget = QWidget()
        self.setCentralWidget(main_widget)
        
        # Layout principal (horizontal) que divide a janela em colunas
        main_layout = QHBoxLayout(main_widget)

        # --- Coluna 1: Filas de Processos ---
        filas_layout = self.criar_layout_filas()
        main_layout.addLayout(filas_layout, stretch=2) # Ocupa 2/4 da largura

        # --- Coluna 2: CPU, Memória e Log ---
        sistema_layout = self.criar_layout_sistema()
        main_layout.addLayout(sistema_layout, stretch=2) # Ocupa 2/4 da largura

    def criar_layout_filas(self):
        # Layout vertical para a coluna das filas
        layout = QVBoxLayout()
        
        titulo = QLabel("FILAS DE PROCESSOS")
        titulo.setFont(QFont('Arial', 16))
        titulo.setAlignment(Qt.AlignmentFlag.AlignCenter)
        layout.addWidget(titulo)

        # Área para cada fila
        layout.addWidget(QLabel("Prontos (Ready):"))
        self.fila_prontos_area = QFrame()
        self.fila_prontos_area.setFrameShape(QFrame.Shape.StyledPanel)
        self.fila_prontos_area.setMinimumHeight(100)
        layout.addWidget(self.fila_prontos_area)

        layout.addWidget(QLabel("Bloqueados (Blocked):"))
        self.fila_bloqueados_area = QFrame()
        self.fila_bloqueados_area.setFrameShape(QFrame.Shape.StyledPanel)
        self.fila_bloqueados_area.setMinimumHeight(100)
        layout.addWidget(self.fila_bloqueados_area)

        layout.addWidget(QLabel("Background:"))
        self.fila_background_area = QFrame()
        self.fila_background_area.setFrameShape(QFrame.Shape.StyledPanel)
        self.fila_background_area.setMinimumHeight(100)
        layout.addWidget(self.fila_background_area)
        
        layout.addWidget(QLabel("Suspensos (Suspended):"))
        self.fila_suspensos_area = QFrame()
        self.fila_suspensos_area.setFrameShape(QFrame.Shape.StyledPanel)
        self.fila_suspensos_area.setMinimumHeight(100)
        layout.addWidget(self.fila_suspensos_area)

        layout.addStretch() # Adiciona espaço flexível no final
        return layout

    def criar_layout_sistema(self):
        # Layout vertical para a coluna de CPU, Memória e Log
        layout = QVBoxLayout()

        # Área da CPU
        cpu_label = QLabel("CPU")
        cpu_label.setFont(QFont('Arial', 16))
        layout.addWidget(cpu_label)
        self.cpu_area = QFrame()
        self.cpu_area.setFrameShape(QFrame.Shape.StyledPanel)
        self.cpu_area.setMinimumHeight(50)
        layout.addWidget(self.cpu_area)

        # Área da Memória
        mem_label = QLabel("Memória")
        mem_label.setFont(QFont('Arial', 16))
        layout.addWidget(mem_label)
        self.memoria_area = QFrame()
        self.memoria_area.setFrameShape(QFrame.Shape.StyledPanel)
        self.memoria_area.setMinimumHeight(50)
        layout.addWidget(self.memoria_area)

        # Área de Log
        log_label = QLabel("Log do Sistema")
        log_label.setFont(QFont('Arial', 16))
        layout.addWidget(log_label)
        self.log_area = QTextEdit()
        self.log_area.setReadOnly(True) # Torna a área de texto apenas para leitura
        layout.addWidget(self.log_area)

        return layout

def main():
    app = QApplication(sys.argv)
    window = SimuladorWindow()
    window.show()
    sys.exit(app.exec())

if __name__ == '__main__':
    main()
from PyQt6.QtWidgets import QApplication
import sys
# Importing class from login.py
from login import LoginRegisterUI  

def main():
    # Create the application
    app = QApplication(sys.argv)
    
    # Starting the app with the login window
    login_window = LoginRegisterUI()
    login_window.show()
    
    # Run application
    sys.exit(app.exec())

if __name__ == "__main__":
    main()
    
from PyQt6 import QtWidgets, uic
from PyQt6.QtWidgets import QApplication, QWidget
import sys
from user_dashboard import UserDashboardUI
# Import the database function
from database import register_user  

class RegisterUI(QWidget):
    def __init__(self):
        super().__init__()
        
        # Load the UI file
        uic.loadUi('register.ui', self)
        
        # Connect buttons to functions
        self.btnRegister.clicked.connect(self.handle_register)
        self.btnLogin.clicked.connect(self.handle_login)
        
        # Show the window
        self.show()
    
    def handle_register(self):
        # Get input values
        username = self.lineEditUsername.text().strip()
        email = self.lineEditEmail.text().strip()
        password = self.lineEditPassword.text()
        role = self.comboRole.currentText()
        
        # Basic validation
        if not username or not email or not password:
            self.show_message("Error", "Please fill all fields")
            return
            
        # Email validation
        if "@" not in email or "." not in email:
            self.show_message("Error", "Please enter a valid email address")
            return
        
        # Password length validation
        if len(password) < 4:
            self.show_message("Error", "Password must be at least 4 characters long")
            return
        
        # Register user in database
        success, message, user_id = register_user(username, email, password, role)
        
        if success:
            self.show_message("Registration Successful", message)
            
            # Clear form after successful registration
            self.lineEditUsername.clear()
            self.lineEditEmail.clear()
            self.lineEditPassword.clear()
            self.comboRole.setCurrentIndex(0)
            
            # Open the appropriate dashboard
            if role == "Member":
                self.open_user_dashboard(username)
            else:  # Librarian
                self.open_admin_dashboard(username)
        else:
            self.show_message("Registration Failed", message)
    
    def handle_login(self):
        """Navigate back to login page"""
        from login import LoginRegisterUI
        self.login_window = LoginRegisterUI()
        self.login_window.show()
        self.hide()
    
    def open_user_dashboard(self, username):
        """Open user dashboard"""
        self.dashboard = UserDashboardUI(username)
        self.dashboard.show()
        self.hide()
    
    def open_admin_dashboard(self, username):
        """Open admin dashboard for librarians"""
        from admin_dashboard import AdminDashboardUI
        self.dashboard = AdminDashboardUI(username)
        self.dashboard.show()
        self.hide()
    
    def show_message(self, title, message):
        """Show message box"""
        msg = QtWidgets.QMessageBox()
        msg.setWindowTitle(title)
        msg.setText(message)
        msg.exec()
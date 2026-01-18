from PyQt6 import QtWidgets, uic
from PyQt6.QtWidgets import QApplication, QWidget, QMessageBox
import sys
# Import relevant functions
from database import authenticate_user, get_user_email  

class LoginRegisterUI(QWidget):
    def __init__(self):
        super().__init__()
        
        # Load pyqt file
        uic.loadUi('login.ui', self)
        
        # Connect buttons to functions
        self.btnLogin.clicked.connect(self.handle_login)
        self.btnRegister.clicked.connect(self.handle_register)
        
        # Show the window
        self.show()
    
    def handle_login(self):
        # Get input values
        username = self.lineEditUsername.text().strip()
        email = self.lineEditEmail.text().strip()
        password = self.lineEditPassword.text()
        
        # Basic validation
        if not username or not email or not password:
            self.show_message("Error", "Please enter all fields")
            return
        
        # Email validation
        if "@" not in email or "." not in email:
            self.show_message("Error", "Please enter a valid email address")
            return
        
        # Authenticate user against database with all three fields
        success, message, role, user_id = authenticate_user(username, email, password)
        
        if success:
            self.show_message("Login Successful", message)
            
            # Getting user ki email from database for display
            user_email = get_user_email(username, role)
            
            # Opening appropriate dashboard
            if role == "Member":
                self.open_user_dashboard(username, user_email)
            else:
                self.open_admin_dashboard(username, user_email)
        else:
            self.show_message("Login Failed", message)
    
    def handle_register(self):
        """Navigate to register page"""
        from register import RegisterUI
        self.register_window = RegisterUI()
        self.register_window.show()
        self.hide()
    
    def open_user_dashboard(self, username, email):
        """Open user dashboard"""
        from user_dashboard import UserDashboardUI
        self.dashboard = UserDashboardUI(username)
        self.dashboard.show()
        self.hide()
    
    def open_admin_dashboard(self, username, email):
        """Open admin dashboard for librarians"""
        from admin_dashboard import AdminDashboardUI
        self.dashboard = AdminDashboardUI(username)
        self.dashboard.show()
        self.hide()
    
    def show_message(self, title, message):
        """Show message box"""
        msg = QMessageBox()
        msg.setWindowTitle(title)
        msg.setText(message)
        msg.exec()
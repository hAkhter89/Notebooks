from PyQt6 import QtWidgets, uic
from PyQt6.QtWidgets import QApplication, QWidget
import sys

class UserDashboardUI(QWidget):
    def __init__(self, username="User"):
        super().__init__()
        
        # Load the UI file
        uic.loadUi('user_dashboard.ui', self)
        
        # Store username for navigation
        self.username = username
        
        # Set the username in the welcome message
        self.lblWelcome.setText(f"Welcome, {username}")
        
        # Connect buttons to functions
        self.btnSearchBooks.clicked.connect(self.search_books)
        self.btnViewProfile.clicked.connect(self.view_profile)
        self.btnLogout.clicked.connect(self.logout)
        
        # Show the window
        self.show()
    
    def search_books(self):
        """Open search books page"""
        from user_search_books import UserSearchBooksPage
        self.search_page = UserSearchBooksPage(self.username)
        self.search_page.show()
        self.hide()
    
    def view_profile(self):
        """Open user profile page"""
        from user_profile import UserProfilePage
        self.profile_page = UserProfilePage(self.username)  # Don't pass email, let it fetch from DB
        self.profile_page.show()
        self.hide()
    
    def logout(self):
        """Logout and return to login page"""
        from login import LoginRegisterUI
        self.login_window = LoginRegisterUI()
        self.login_window.show()
        self.hide()
    
    def show_message(self, title, message):
        msg = QtWidgets.QMessageBox()
        msg.setWindowTitle(title)
        msg.setText(message)
        msg.exec()
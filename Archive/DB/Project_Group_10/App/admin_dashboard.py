from PyQt6 import QtWidgets, uic
from PyQt6.QtWidgets import QApplication, QWidget
import sys

class AdminDashboardUI(QWidget):
    def __init__(self, username="Admin"):
        super().__init__()
        
        # Load the UI file
        uic.loadUi('admin_dashboard.ui', self)
        
        # Store username for navigation
        self.username = username
        
        # Set the username in the welcome message
        self.lblWelcome.setText(f"Welcome, Admin {username}")
        
        # Connect buttons to functions
        self.btnManageInventory.clicked.connect(self.manage_inventory)
        self.btnCheckOut.clicked.connect(self.check_out)
        self.btnCheckIn.clicked.connect(self.check_in)
        self.btnLogout.clicked.connect(self.logout)
        
        # Show the window
        self.show()
    
    def manage_inventory(self):
        from manage_current_books import ManageCurrentBooksUI
        self.inventory_window = ManageCurrentBooksUI(self.username)
        self.inventory_window.show()
        self.hide()
    
    def check_out(self):
        from admin_checkout import CheckoutUI
        self.checkout_window = CheckoutUI(self.username)
        self.checkout_window.show()
        self.hide()
    
    def check_in(self):
        from admin_checkin import CheckinUI
        self.checkin_window = CheckinUI(self.username)
        self.checkin_window.show()
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

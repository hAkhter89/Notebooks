from PyQt6 import QtWidgets, uic
from PyQt6.QtWidgets import QApplication, QWidget, QTableWidgetItem
from PyQt6.QtCore import Qt
import sys
from datetime import datetime, timedelta
from database import DatabaseConnection

class UserProfilePage(QWidget):
    def __init__(self, username="User", email=None):
        super().__init__()
        
        # Load the UI file
        uic.loadUi('user_profile.ui', self)
        
        # Store user information
        self.username = username
        
        # Initialize database connection
        self.db = DatabaseConnection()
        
        # Connect signals and slots
        self.connect_signals()
        
        # Get user ID and actual email from database
        self.user_id = self.get_user_id()
        self.email = self.get_user_email() if email is None else email
        
        # Update the profile display
        self.update_profile_display()
        
        # Populate tables with data from database
        self.populate_currently_borrowed_table()
        self.populate_borrowing_history_table()
        
        # Show the window
        self.show()
    
    def connect_signals(self):
        """Connect all signals to their respective slots"""
        self.pushButtonBack.clicked.connect(self.go_back)
    
    def get_user_id(self):
        """Get the user's Member_id from their username"""
        try:
            query = "SELECT Member_id FROM Member WHERE Name = ?"
            result = self.db.execute_scalar(query, (self.username,))
            return result
        except Exception as e:
            print(f"DEBUG: Error getting user ID: {e}")
            return None
    
    def get_user_email(self):
        """Get the user's actual email from database"""
        try:
            query = "SELECT Email FROM Member WHERE Name = ?"
            result = self.db.execute_scalar(query, (self.username,))
            if result:
                return result
            return f"{self.username}@example.com"
        except Exception as e:
            print(f"DEBUG: Error getting user email: {e}")
            return f"{self.username}@example.com"
    
    def get_currently_borrowed_books(self):
        """Get currently borrowed books from database (Return_Date IS NULL)"""
        try:
            if not self.user_id:
                return []
            
            query = """
            SELECT 
                b.Name AS BookTitle,
                bor.ISBN,
                STUFF((
                    SELECT ', ' + ba2.Author 
                    FROM Book_Author ba2 
                    WHERE ba2.ISBN = b.ISBN 
                    FOR XML PATH(''), TYPE).value('.', 'NVARCHAR(MAX)'), 1, 2, ''
                ) AS Authors,
                bor.Borrow_Date,
                bor.Return_Due_Date
            FROM Borrow bor
            INNER JOIN Book b ON bor.ISBN = b.ISBN
            WHERE bor.Member_Id = ? AND bor.Return_Date IS NULL
            ORDER BY bor.Borrow_Date DESC  -- Most recent first
            """
            
            results = self.db.execute_query(query, (self.user_id,))
            
            books = []
            if results:
                for title, isbn, authors, borrow_date, due_date in results:
                    books.append({
                        "title": title,
                        "isbn": str(isbn),
                        "author": authors if authors else "Unknown Author",
                        "borrow_date": str(borrow_date),
                        "due_date": str(due_date)
                    })
            
            return books
            
        except Exception as e:
            print(f"DEBUG: Error getting currently borrowed books: {e}")
            return []
    
    def get_borrowing_history(self):
        """Get complete borrowing history from database (all borrow records)"""
        try:
            if not self.user_id:
                return []
            
            query = """
            SELECT 
                b.Name AS BookTitle,
                bor.ISBN,
                STUFF((
                    SELECT ', ' + ba2.Author 
                    FROM Book_Author ba2 
                    WHERE ba2.ISBN = b.ISBN 
                    FOR XML PATH(''), TYPE).value('.', 'NVARCHAR(MAX)'), 1, 2, ''
                ) AS Authors,
                bor.Borrow_Date,
                bor.Return_Due_Date,
                bor.Return_Date
            FROM Borrow bor
            INNER JOIN Book b ON bor.ISBN = b.ISBN
            WHERE bor.Member_Id = ?
            ORDER BY bor.Borrow_Date DESC  -- Most recent first
            """
            
            results = self.db.execute_query(query, (self.user_id,))
            
            books = []
            if results:
                for title, isbn, authors, borrow_date, due_date, return_date in results:
                    # Format return date - show "Not returned yet" if NULL
                    return_date_display = str(return_date) if return_date else "Not returned yet"
                    
                    books.append({
                        "title": title,
                        "isbn": str(isbn),
                        "author": authors if authors else "Unknown Author",
                        "borrow_date": str(borrow_date),
                        "return_date": return_date_display
                    })
            
            return books
            
        except Exception as e:
            print(f"DEBUG: Error getting borrowing history: {e}")
            return []
    
    def update_profile_display(self):
        """Update the profile information display with actual data"""
        self.usernameLabel.setText(f"Username: {self.username}")
        self.emailLabel.setText(f"Email: {self.email}")
    
    def populate_currently_borrowed_table(self):
        """Populate the currently borrowed books table with REAL data"""
        try:
            # Get real data from database
            self.currently_borrowed_books = self.get_currently_borrowed_books()
            
            self.tableWidgetCurrentlyBorrowed.setRowCount(len(self.currently_borrowed_books))
            
            for row, book in enumerate(self.currently_borrowed_books):
                self.tableWidgetCurrentlyBorrowed.setItem(row, 0, QTableWidgetItem(book["title"]))
                self.tableWidgetCurrentlyBorrowed.setItem(row, 1, QTableWidgetItem(book["isbn"]))
                self.tableWidgetCurrentlyBorrowed.setItem(row, 2, QTableWidgetItem(book["author"]))
                self.tableWidgetCurrentlyBorrowed.setItem(row, 3, QTableWidgetItem(book["borrow_date"]))
                self.tableWidgetCurrentlyBorrowed.setItem(row, 4, QTableWidgetItem(book["due_date"]))
            
            # Resize columns to content
            self.tableWidgetCurrentlyBorrowed.resizeColumnsToContents()
            
            if not self.currently_borrowed_books:
                self.tableWidgetCurrentlyBorrowed.setRowCount(1)
                self.tableWidgetCurrentlyBorrowed.setItem(0, 0, QTableWidgetItem("No books currently borrowed"))
                for col in range(1, 5):
                    self.tableWidgetCurrentlyBorrowed.setItem(0, col, QTableWidgetItem(""))
                    
        except Exception as e:
            print(f"DEBUG: Error populating currently borrowed table: {e}")
            self.tableWidgetCurrentlyBorrowed.setRowCount(1)
            self.tableWidgetCurrentlyBorrowed.setItem(0, 0, QTableWidgetItem("Error loading currently borrowed books"))
    
    def populate_borrowing_history_table(self):
        """Populate the borrowing history table with REAL data"""
        try:
            # Get real data from database
            self.borrowing_history = self.get_borrowing_history()
            
            self.tableWidgetBorrowingHistory.setRowCount(len(self.borrowing_history))
            
            for row, book in enumerate(self.borrowing_history):
                self.tableWidgetBorrowingHistory.setItem(row, 0, QTableWidgetItem(book["title"]))
                self.tableWidgetBorrowingHistory.setItem(row, 1, QTableWidgetItem(book["isbn"]))
                self.tableWidgetBorrowingHistory.setItem(row, 2, QTableWidgetItem(book["author"]))
                self.tableWidgetBorrowingHistory.setItem(row, 3, QTableWidgetItem(book["borrow_date"]))
                self.tableWidgetBorrowingHistory.setItem(row, 4, QTableWidgetItem(book["return_date"]))
            
            # Resize columns to content
            self.tableWidgetBorrowingHistory.resizeColumnsToContents()
            
            if not self.borrowing_history:
                self.tableWidgetBorrowingHistory.setRowCount(1)
                self.tableWidgetBorrowingHistory.setItem(0, 0, QTableWidgetItem("No borrowing history"))
                for col in range(1, 5):
                    self.tableWidgetBorrowingHistory.setItem(0, col, QTableWidgetItem(""))
                    
        except Exception as e:
            print(f"DEBUG: Error populating borrowing history table: {e}")
            self.tableWidgetBorrowingHistory.setRowCount(1)
            self.tableWidgetBorrowingHistory.setItem(0, 0, QTableWidgetItem("Error loading borrowing history"))
    
    def go_back(self):
        """Return to user dashboard"""
        from user_dashboard import UserDashboardUI
        self.dashboard = UserDashboardUI(self.username)
        self.dashboard.show()
        self.hide()
    
    def show_message(self, title, message):
        """Show message box"""
        msg = QtWidgets.QMessageBox()
        msg.setWindowTitle(title)
        msg.setText(message)
        msg.exec()
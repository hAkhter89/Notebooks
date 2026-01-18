from PyQt6 import QtWidgets, uic
from PyQt6.QtWidgets import QApplication, QWidget, QMessageBox, QCompleter, QTableWidgetItem
from PyQt6.QtCore import Qt
import sys
from datetime import datetime, timedelta
from database import DatabaseConnection

class CheckinUI(QWidget):
    def __init__(self, admin_username="Admin"):
        super().__init__()
        
        # Load the UI file
        uic.loadUi('admin_checkin.ui', self)
        
        # Store admin username for reference
        self.admin_username = admin_username
        
        # Initialize database connection
        self.db = DatabaseConnection()
        
        # Populate dropdowns with data from database
        self.populate_member_dropdown()
        
        # Set up autocomplete using Qts builtin completer
        self.setup_autocomplete()
        
        # Connect signals and slots
        self.connect_signals()
        
        # Show the member name for the initially selected ID
        self.update_member_name_display()
        
        # Initialize selected book data
        self.selected_book_data = None
        
        # Show the window
        self.show()
    
    def connect_signals(self):
        """Connect all signals to their respective slots"""
        self.comboMemberID.currentTextChanged.connect(self.on_member_selected)
        self.tableIssuedBooks.itemSelectionChanged.connect(self.on_book_selected)
        self.btnCheckIn.clicked.connect(self.check_in_book)
        self.btnBack.clicked.connect(self.go_back)
    
    def setup_autocomplete(self):
        """Set up autocomplete functionality using QCompleter"""
        # For Member ID
        member_completer = QCompleter([self.comboMemberID.itemText(i) for i in range(self.comboMemberID.count())])
        member_completer.setCaseSensitivity(Qt.CaseSensitivity.CaseInsensitive)
        member_completer.setFilterMode(Qt.MatchFlag.MatchContains)
        self.comboMemberID.setCompleter(member_completer)
    
    def populate_member_dropdown(self):
        """Populate member dropdown with data from database"""
        try:
            # Get all members who currently have borrowed books
            query = """
            SELECT DISTINCT m.Member_id, m.Name 
            FROM Member m 
            INNER JOIN Borrow b ON m.Member_id = b.Member_Id 
            WHERE b.Return_Date IS NULL
            ORDER BY m.Member_id
            """
            results = self.db.execute_query(query)
            
            self.member_data = {}
            self.comboMemberID.clear()
            
            if results:
                for member_id, name in results:
                    self.member_data[str(member_id)] = name
                    self.comboMemberID.addItem(str(member_id))
            
        except Exception as e:
            print(f"DEBUG: Error loading members: {e}")
            self.show_message("Error", f"Failed to load members: {str(e)}")
    
    def populate_issued_books_table(self, member_id):
        """Populate issued books table with data from database"""
        try:
            # Clear the table
            self.tableIssuedBooks.setRowCount(0)
            
            # Get currently borrowed books for this member (where Return_Date IS NULL)
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
            ORDER BY bor.Borrow_Date DESC
            """
            
            results = self.db.execute_query(query, (int(member_id),))
            
            if results:
                # Set column headers to include all relevant information
                self.tableIssuedBooks.setColumnCount(5)  # 5 columns now
                self.tableIssuedBooks.setHorizontalHeaderLabels([
                    "Book Title", 
                    "ISBN", 
                    "Authors", 
                    "Borrow Date", 
                    "Due Date"  # Added Due Date column
                ])
                
                for row, (title, isbn, authors, borrow_date, due_date) in enumerate(results):
                    self.tableIssuedBooks.insertRow(row)
                    self.tableIssuedBooks.setItem(row, 0, QTableWidgetItem(title))
                    self.tableIssuedBooks.setItem(row, 1, QTableWidgetItem(str(isbn)))
                    self.tableIssuedBooks.setItem(row, 2, QTableWidgetItem(authors if authors else "Unknown Author"))
                    self.tableIssuedBooks.setItem(row, 3, QTableWidgetItem(str(borrow_date)))
                    self.tableIssuedBooks.setItem(row, 4, QTableWidgetItem(str(due_date)))  # Added Due Date
                
                # Resize columns to content
                self.tableIssuedBooks.resizeColumnsToContents()
            else:
                # Show "No books issued" message
                self.tableIssuedBooks.setRowCount(1)
                self.tableIssuedBooks.setColumnCount(5)  # 5 columns now
                self.tableIssuedBooks.setItem(0, 0, QTableWidgetItem("No books currently issued to this member"))
                for col in range(1, 5):
                    self.tableIssuedBooks.setItem(0, col, QTableWidgetItem(""))
                    
        except Exception as e:
            print(f"DEBUG: Error loading issued books: {e}")
            self.tableIssuedBooks.setRowCount(1)
            self.tableIssuedBooks.setColumnCount(5)  # 5 columns now
            self.tableIssuedBooks.setItem(0, 0, QTableWidgetItem("Error loading issued books"))
    
    def update_member_name_display(self):
        """Update member name display for the currently selected ID"""
        current_member_id = self.comboMemberID.currentText()
        self.on_member_selected(current_member_id)
    
    def on_member_selected(self, member_id):
        """When member ID is selected or entered, display member name and issued books"""
        if member_id in self.member_data:
            member_name = self.member_data[member_id]
            self.lineEditMemberName.setText(member_name)
            # Populate issued books for this member
            self.populate_issued_books_table(member_id)
            # Reset selected book
            self.selected_book_data = None  
        else:
            self.lineEditMemberName.setText("")
            self.tableIssuedBooks.setRowCount(0)
            self.selected_book_data = None
    
    def on_book_selected(self):
        """When a book is selected from the table"""
        current_row = self.tableIssuedBooks.currentRow()
        if current_row >= 0:
            try:
                # Get the selected book data from the table (now 5 columns)
                title = self.tableIssuedBooks.item(current_row, 0).text()
                isbn = self.tableIssuedBooks.item(current_row, 1).text()
                authors = self.tableIssuedBooks.item(current_row, 2).text()
                borrow_date = self.tableIssuedBooks.item(current_row, 3).text()
                due_date = self.tableIssuedBooks.item(current_row, 4).text()  # Get due date
                
                self.selected_book_data = {
                    'title': title,
                    'isbn': isbn,
                    'author': authors,  # Now contains all authors
                    'borrow_date': borrow_date,
                    'due_date': due_date  # Store due date as well
                }
                
            except Exception as e:
                print(f"DEBUG: Error selecting book: {e}")
                self.selected_book_data = None
    
    def get_librarian_id(self):
        """Get the librarian ID from the username"""
        try:
            query = "SELECT Librarian_id FROM Librarian WHERE Name = ?"
            result = self.db.execute_scalar(query, (self.admin_username,))
            if result:
                return int(result)
            return None
        except Exception as e:
            print(f"DEBUG: Error getting librarian ID: {e}")
            return None
    
    def check_in_book(self):
        """Handle book check-in process with database transactions"""
        # Get selected values
        member_id = self.comboMemberID.currentText()
        
        # Validation
        if not member_id:
            self.show_message("Error", "Please select or enter a Member ID")
            return
        
        if member_id not in self.member_data:
            self.show_message("Error", "Invalid Member ID. Please select from the list.")
            return
        
        if not self.selected_book_data:
            self.show_message("Error", "Please select a book from the table to check in")
            return
        
        book_title = self.selected_book_data['title']
        isbn = self.selected_book_data['isbn']
        
        # Get librarian ID
        librarian_id = self.get_librarian_id()
        if not librarian_id:
            self.show_message("Error", "Librarian not found in system")
            return
        
        # Get current timestamp ONCE for display AND database
        current_timestamp = datetime.now()
        
        # Confirm check-in
        reply = QMessageBox.question(self, "Confirm Check In", 
                                f"Check in '{book_title}' from {self.lineEditMemberName.text()}?\n\n"
                                f"Check-in Date: {current_timestamp.strftime('%Y-%m-%d %H:%M:%S')}",
                                QMessageBox.StandardButton.Yes | QMessageBox.StandardButton.No)
        
        if reply == QMessageBox.StandardButton.Yes:
            try:
                # Convert IDs to appropriate types
                member_id_int = int(member_id)
                
                # 1. Update Borrow table - use our timestamp
                update_borrow_query = """
                UPDATE Borrow 
                SET Return_Date = ? 
                WHERE Member_Id = ? AND ISBN = ? AND Return_Date IS NULL
                """
                borrow_success = self.db.execute_update(update_borrow_query, 
                    (current_timestamp, member_id_int, isbn))  
                
                if not borrow_success:
                    self.show_message("Error", "Failed to update borrowing record")
                    return
                
                # 2. Insert into Check_in table - use SAME timestamp
                checkin_query = """
                INSERT INTO Check_in (Librarian_Id, ISBN, Check_in_date) 
                VALUES (?, ?, ?)
                """
                checkin_success = self.db.execute_update(checkin_query, 
                    (librarian_id, isbn, current_timestamp))  
                
                if not checkin_success:
                    self.show_message("Error", "Failed to record check-in transaction")
                    return
                
                # 3. Update book copies (increment by 1)
                update_copies_query = "UPDATE Book SET No_of_copies = No_of_copies + 1 WHERE ISBN = ?"
                update_success = self.db.execute_update(update_copies_query, (isbn,))
                
                if not update_success:
                    self.show_message("Error", "Failed to update book inventory")
                    return
                
                # SUCCESS - all operations completed
                self.show_message("Success", 
                                f"Book checked in successfully!\n\n"
                                f"Member: {self.lineEditMemberName.text()}\n"
                                f"Book: {book_title}\n"
                                f"Check-in Date: {current_timestamp.strftime('%Y-%m-%d %H:%M:%S')}\n"
                                f"Book has been returned to inventory.")
                
                # Refresh the issued books table to remove the returned book
                self.populate_issued_books_table(member_id)
                # Reset selection
                self.selected_book_data = None  
                
                # Refresh member dropdown in case this member has no more books
                self.populate_member_dropdown()
                
            except Exception as e:
                print(f"DEBUG: Error during book check-in: {e}")
                self.show_message("Error", f"Failed to check in book: {str(e)}")
    
    def go_back(self):
        """Return to admin dashboard"""
        from admin_dashboard import AdminDashboardUI
        self.admin_dashboard = AdminDashboardUI(self.admin_username)
        self.admin_dashboard.show()
        self.hide()
    
    def show_message(self, title, message):
        """Show message box"""
        msg = QMessageBox()
        msg.setWindowTitle(title)
        msg.setText(message)
        msg.exec()

from PyQt6 import QtWidgets, uic
from PyQt6.QtWidgets import QApplication, QWidget, QMessageBox, QCompleter
from PyQt6.QtCore import Qt
import sys
from datetime import datetime, timedelta
from database import DatabaseConnection

class CheckoutUI(QWidget):
    def __init__(self, admin_username="Admin"):
        super().__init__()
        
        # Load the UI file
        uic.loadUi('admin_checkout.ui', self)
        
        # Store admin username for reference
        self.admin_username = admin_username
        
        # Initialize database connection
        self.db = DatabaseConnection()
        
        # Populate dropdowns with data from database
        self.populate_member_dropdown()
        self.populate_book_dropdown()
        
        # Set up autocomplete using Qts builtin completer
        self.setup_autocomplete()
        
        # Connect signals and slots
        self.connect_signals()
        
        # Show the member name for the initially selected ID
        self.update_member_name_display()
        
        # Show the window
        self.show()
    
    def connect_signals(self):
        """Connect all signals to their respective slots"""
        self.comboMemberID.currentTextChanged.connect(self.on_member_selected)
        self.btnIssue.clicked.connect(self.issue_book)
        self.btnBack.clicked.connect(self.go_back)
    
    def setup_autocomplete(self):
        """Set up autocomplete functionality using QCompleter"""
        # For Member ID
        member_completer = QCompleter([self.comboMemberID.itemText(i) for i in range(self.comboMemberID.count())])
        member_completer.setCaseSensitivity(Qt.CaseSensitivity.CaseInsensitive)
        member_completer.setFilterMode(Qt.MatchFlag.MatchContains)
        self.comboMemberID.setCompleter(member_completer)
        
        # For Book
        book_completer = QCompleter([self.comboBook.itemText(i) for i in range(self.comboBook.count())])
        book_completer.setCaseSensitivity(Qt.CaseSensitivity.CaseInsensitive)
        book_completer.setFilterMode(Qt.MatchFlag.MatchContains)
        self.comboBook.setCompleter(book_completer)
    
    def populate_member_dropdown(self):
        """Populate member dropdown with data from database"""
        try:
            # Get all active members from database
            query = "SELECT Member_id, Name FROM Member ORDER BY Member_id"
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
    
    def populate_book_dropdown(self):
        """Populate book dropdown with data from database (only available books)"""
        try:
            # Get books that have available copies
            query = """
            SELECT b.ISBN, b.Name, b.No_of_copies 
            FROM Book b 
            WHERE b.No_of_copies > 0 
            ORDER BY b.Name
            """
            results = self.db.execute_query(query)
            
            self.book_data = {}
            self.comboBook.clear()
            
            if results:
                for isbn, name, copies in results:
                    display_text = f"{name} (ISBN: {isbn}) - {copies} available"
                    self.book_data[display_text] = {
                        "isbn": isbn,
                        "title": name,
                        "copies": copies
                    }
                    self.comboBook.addItem(display_text)
            
        except Exception as e:
            print(f"DEBUG: Error loading books: {e}")
            self.show_message("Error", f"Failed to load books: {str(e)}")
    
    def update_member_name_display(self):
        """Update member name display for the currently selected ID"""
        current_member_id = self.comboMemberID.currentText()
        self.on_member_selected(current_member_id)
    
    def on_member_selected(self, member_id):
        """When member ID is selected or entered, display member name"""
        if member_id in self.member_data:
            member_name = self.member_data[member_id]
            self.lineEditMemberName.setText(member_name)
        else:
            # Clear if invalid ID
            self.lineEditMemberName.setText("")  
    
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
    
    def check_if_member_can_borrow(self, member_id):
        """Check if member can borrow more books (max 5 books at a time)"""
        try:
            query = """
            SELECT COUNT(*) FROM Borrow 
            WHERE Member_Id = ? AND Return_Date IS NULL
            """
            current_borrowed_count = self.db.execute_scalar(query, (int(member_id),))
            
            MAX_BOOKS_ALLOWED = 5
            if current_borrowed_count >= MAX_BOOKS_ALLOWED:
                return False, f"Cannot borrow more than {MAX_BOOKS_ALLOWED} books at a time"
            
            return True, "Member can borrow"
            
        except Exception as e:
            print(f"DEBUG: Error checking borrowing limit: {e}")
            return False, "Error checking borrowing status"
    
    def check_if_member_already_has_book(self, member_id, isbn):
        """Check if member already has this book borrowed and not returned"""
        try:
            query = """
            SELECT COUNT(*) FROM Borrow 
            WHERE Member_Id = ? AND ISBN = ? AND Return_Date IS NULL
            """
            count = self.db.execute_scalar(query, (int(member_id), isbn))
            return count > 0 if count else False
        except Exception as e:
            print(f"DEBUG: Error checking if member already has book: {e}")
            return False
    
    def issue_book(self):
        """Handle REAL book issuance process with database transactions"""
        # Get selected values
        member_id = self.comboMemberID.currentText()
        book_display_text = self.comboBook.currentText()
        
        # Validation
        if not member_id:
            self.show_message("Error", "Please select or enter a Member ID")
            return
        
        if member_id not in self.member_data:
            self.show_message("Error", "Invalid Member ID. Please select from the list.")
            return
        
        if not book_display_text:
            self.show_message("Error", "Please select or enter a Book")
            return
        
        if book_display_text not in self.book_data:
            self.show_message("Error", "Invalid Book. Please select from the list.")
            return
        
        # Extract book info
        book_info = self.book_data[book_display_text]
        isbn = book_info["isbn"]
        book_title = book_info["title"]
        current_copies = book_info["copies"]
        
        # Get librarian ID
        librarian_id = self.get_librarian_id()
        if not librarian_id:
            self.show_message("Error", "Librarian not found in system")
            return
        
        # Check if member already has this book borrowed and not returned
        if self.check_if_member_already_has_book(member_id, isbn):
            self.show_message("Borrowing Error", 
                            f"Member {self.lineEditMemberName.text()} already has '{book_title}' borrowed.\n"
                            f"They must return it before borrowing another copy.")
            return
        
        # Check if member can borrow
        can_borrow, message = self.check_if_member_can_borrow(member_id)
        if not can_borrow:
            self.show_message("Borrowing Limit", message)
            return
        
        # Get current timestamp for display AND database
        current_timestamp = datetime.now()
        return_due_date = current_timestamp + timedelta(days=30)
        
        # Confirm issuance
        reply = QMessageBox.question(self, "Confirm Issue", 
                                f"Issue '{book_title}' to {self.lineEditMemberName.text()}?\n\n"
                                f"Due Date: {return_due_date.strftime('%Y-%m-%d')}",
                                QMessageBox.StandardButton.Yes | QMessageBox.StandardButton.No)
        
        if reply == QMessageBox.StandardButton.Yes:
            try:
                # Convert member ID to integer
                member_id_int = int(member_id)
                
                # 1. Insert into Borrow table - use our timestamp
                borrow_query = """
                INSERT INTO Borrow (Member_Id, ISBN, Borrow_Date, Return_Due_Date, Return_Date) 
                VALUES (?, ?, ?, DATEADD(month, 1, ?), NULL)
                """
                borrow_success = self.db.execute_update(borrow_query, 
                    (member_id_int, isbn, current_timestamp, current_timestamp))  
                
                if not borrow_success:
                    self.show_message("Error", "Failed to record borrowing transaction")
                    return
                
                # 2. Insert into Check_out table - use SAME timestamp
                checkout_query = """
                INSERT INTO Check_out (Librarian_Id, ISBN, Check_out_date) 
                VALUES (?, ?, ?)
                """
                checkout_success = self.db.execute_update(checkout_query, 
                    (librarian_id, isbn, current_timestamp))  

                if not checkout_success:
                    self.show_message("Error", "Failed to record check-out transaction")
                    return
                
                # 3. Update book copies (decrement by 1)
                update_copies_query = "UPDATE Book SET No_of_copies = No_of_copies - 1 WHERE ISBN = ?"
                update_success = self.db.execute_update(update_copies_query, (isbn,))
                
                if not update_success:
                    self.show_message("Error", "Failed to update book inventory")
                    return
                
                # SUCCESS - all operations completed
                self.show_message("Success", 
                                f"Book issued successfully!\n\n"
                                f"Member: {self.lineEditMemberName.text()}\n"
                                f"Book: {book_title}\n"
                                f"Due Date: {return_due_date.strftime('%Y-%m-%d')}\n"
                                f"Remaining copies: {current_copies - 1}")
                
                # Refresh dropdowns to reflect updated availability
                self.populate_book_dropdown()
                self.setup_autocomplete()
                
                #Clear both member ID and name fields
                self.comboMemberID.setCurrentIndex(-1) 
                self.lineEditMemberName.clear()
                
            except ValueError as ve:
                print(f"DEBUG: Value error during book issuance: {ve}")
                self.show_message("Error", f"Invalid data format: {str(ve)}")
            except Exception as e:
                print(f"DEBUG: Error during book issuance: {e}")
                self.show_message("Error", f"Failed to issue book: {str(e)}")
    
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

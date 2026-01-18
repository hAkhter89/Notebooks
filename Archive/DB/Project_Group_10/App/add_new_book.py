from PyQt6 import QtWidgets, uic
from PyQt6.QtWidgets import QApplication, QWidget, QMessageBox, QListWidgetItem
from PyQt6.QtCore import Qt
import sys
from database import add_new_book_to_database, get_all_authors, add_new_author_to_database, check_isbn_exists

class AddBookPage(QWidget):
    def __init__(self, admin_username="Admin", parent=None):
        super().__init__(parent)
        
        # Load the UI file
        uic.loadUi('add_new_book.ui', self)
        
        # Store admin username for reference
        self.admin_username = admin_username
        
        # Connect signals and slots
        self.connect_signals()
        
        # Initialize authors list from database
        self.initialize_authors_list()
        
        # Show the window
        self.show()
    
    def connect_signals(self):
        """Connect all signals to their respective slots"""
        # Author management
        self.pushButtonAddAuthor.clicked.connect(self.add_new_author)
        self.lineEditNewAuthor.returnPressed.connect(self.add_new_author)
        
        # ISBN validation (check for duplicates as user types)
        self.lineEditISBN.textChanged.connect(self.validate_isbn)
        
        # Page navigation
        self.pushButtonAddBook.clicked.connect(self.add_book)
        self.pushButtonBack.clicked.connect(self.go_back)
    
    def initialize_authors_list(self):
        """Initialize the authors list with authors from database"""
        authors = get_all_authors()
        
        # Clear existing items and add authors from database
        self.listWidgetAuthors.clear()
        for author in authors:
            item = QListWidgetItem(author)
            self.listWidgetAuthors.addItem(item)
    
    def validate_isbn(self):
        """Validate ISBN and check for duplicates in real-time"""
        isbn = self.lineEditISBN.text().strip()
        if isbn:
            # Green color for valid, red for invalid
            if check_isbn_exists(isbn):
                self.lineEditISBN.setStyleSheet("background-color: #FFCCCC; color: #8B0000; font-weight: bold;")
            else:
                self.lineEditISBN.setStyleSheet("background-color: #CCFFCC; color: #006400; font-weight: bold;") 
        else:
            self.lineEditISBN.setStyleSheet("")
    
    def add_new_author(self):
        """Add a new author to the authors list"""
        author_name = self.lineEditNewAuthor.text().strip()
        
        if author_name:
            # Check if author already exists in the list
            existing_authors = []
            for i in range(self.listWidgetAuthors.count()):
                existing_authors.append(self.listWidgetAuthors.item(i).text())
            
            if author_name not in existing_authors:
                # Add to the local list widget
                item = QListWidgetItem(author_name)
                self.listWidgetAuthors.addItem(item)
                
                # Also check if this author exists in the database system
                success, message = add_new_author_to_database(author_name)
                if not success and "already exists" in message:
                    self.show_message("Info", message)
                
                self.lineEditNewAuthor.clear()
                self.show_message("Success", f"Author '{author_name}' added to the list")
            else:
                self.show_message("Warning", f"Author '{author_name}' already exists in the list")
        else:
            self.show_message("Error", "Please enter an author name")
    
    def validate_inputs(self):
        """Validate all input fields"""
        book_name = self.lineEditBookName.text().strip()
        isbn = self.lineEditISBN.text().strip()
        
        # Check required fields
        if not book_name:
            self.show_message("Error", "Please enter a book name")
            return False
        
        if not isbn:
            self.show_message("Error", "Please enter an ISBN")
            return False
        
        # Check if ISBN already exists
        if check_isbn_exists(isbn):
            self.show_message("Error", f"Book with ISBN {isbn} already exists in the system")
            return False
        
        # Validate ISBN length
        if len(isbn) < 10:
            self.show_message("Error", "ISBN should be at least 10 characters long")
            return False
        
        # Check if at least one author is selected
        selected_authors = self.listWidgetAuthors.selectedItems()
        if not selected_authors:
            self.show_message("Error", "Please select at least one author")
            return False
        
        # Check if at least one genre is selected
        selected_genres = self.listWidgetGenres.selectedItems()
        if not selected_genres:
            self.show_message("Error", "Please select at least one genre")
            return False
        
        return True
    
    def get_selected_authors(self):
        """Get list of selected authors"""
        return [item.text() for item in self.listWidgetAuthors.selectedItems()]
    
    def get_selected_genres(self):
        """Get list of selected genres"""
        return [item.text() for item in self.listWidgetGenres.selectedItems()]
    
    def add_book(self):
        """Add the new book to the database system"""
        if not self.validate_inputs():
            return
        
        # Get book data
        book_name = self.lineEditBookName.text().strip()
        isbn = self.lineEditISBN.text().strip()
        copies = self.spinBoxCopies.value()
        authors = self.get_selected_authors()
        genres = self.get_selected_genres()
        
        # Prepare book data
        book_data = {
            "title": book_name,
            "isbn": isbn,
            "copies": copies,
            "authors": authors,
            "genres": genres
        }
        
        # Show confirmation dialog
        confirmation_msg = (
            f"Please confirm book details:\n\n"
            f"Title: {book_name}\n"
            f"ISBN: {isbn}\n"
            f"Copies: {copies}\n"
            f"Authors: {', '.join(authors)}\n"
            f"Genres: {', '.join(genres)}\n\n"
            f"Add this book to the library?"
        )
        
        reply = QMessageBox.question(self, "Confirm Book Addition", 
                                confirmation_msg,
                                QMessageBox.StandardButton.Yes | QMessageBox.StandardButton.No)
        
        if reply == QMessageBox.StandardButton.Yes:
            # Add book to database
            success, message = add_new_book_to_database(book_data)
            
            if success:
                # Verify the insertion worked
                from database import verify_book_insertion
                if verify_book_insertion(isbn):
                    self.show_message("Success", message + "\n\nBook added successfully!")
                else:
                    self.show_message("Warning", message + "\n\nWarning: Please verify the book was added correctly.")
                
                # Clear form for next entry
                self.clear_form()
            else:
                self.show_message("Error", message)
            
    def clear_form(self):
        """Clear the form for next entry"""
        self.lineEditBookName.clear()
        self.lineEditISBN.clear()
        self.spinBoxCopies.setValue(1)
        self.lineEditNewAuthor.clear()
        
        # Clear selections
        self.listWidgetAuthors.clearSelection()
        self.listWidgetGenres.clearSelection()
        
        # Reset ISBN background color and text color to default
        self.lineEditISBN.setStyleSheet("") 
    
    def go_back(self):
        """Go back to the previous page"""
        if self.parent():
            # If opened from another page, go back to it
            self.parent().show()
            self.hide()
        else:
            # If opened directly, open the inventory page
            from manage_current_books import ManageCurrentBooksUI
            self.inventory_page = ManageCurrentBooksUI(self.admin_username)
            self.inventory_page.show()
            self.hide()
    
    def show_message(self, title, message):
        """Show message box"""
        msg = QMessageBox()
        msg.setWindowTitle(title)
        msg.setText(message)
        msg.exec()